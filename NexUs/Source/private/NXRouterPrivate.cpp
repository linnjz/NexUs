#include "NXRouterPrivate.h"

#include <QWidget>

#include "NXWindow.h"

NXRouterPrivate::NXRouterPrivate(QObject *parent)
    : QObject { parent }
{
}

NXRouterPrivate::~NXRouterPrivate()
{
  qDeleteAll(_rootNodes);
}

QString NXRouterPrivate::normalizePath(const QString &path)
{
  QString p = path;
  if (!p.startsWith('/')) {
    p.prepend('/');
  }
  while (p.length() > 1 && p.endsWith('/')) {
    p.chop(1);
  }
  return p;
}

QString NXRouterPrivate::joinPath(const QString &parent, const QString &child)
{
  QString c = child;
  while (c.startsWith('/')) {
    c = c.mid(1);
  }
  if (parent == QStringLiteral("/")) {
    return normalizePath(QStringLiteral("/") + c);
  }
  return normalizePath(parent + QStringLiteral("/") + c);
}

bool NXRouterPrivate::_isOnRouterPage() const
{
  if (!_window || _history.isEmpty() || _historyIndex < 0) {
    return false;
  }
  QString currentPageKey = _window->getCurrentNavigationPageKey();
  const auto &entry      = _history[_historyIndex];
  if (_pathIndex.contains(entry.path)) {
    NXRouteNode *node = _pathIndex[entry.path];
    return node->pageKey == currentPageKey;
  }
  return false;
}

NXRouterType::NavigationResult NXRouterPrivate::addRouteToTree(const NXRouteConfig &config, const QString &parentPathPrefix)
{
  QString fullPath;
  if (parentPathPrefix.isEmpty()) {
    fullPath = normalizePath(config.path);
  } else {
    fullPath = joinPath(parentPathPrefix, config.path);
  }

  if (_pathIndex.contains(fullPath)) {
    return NXRouterType::RouteAlreadyExists;
  }

  NXRouteNode *node = new NXRouteNode();
  node->path        = fullPath;
  node->title       = config.title;
  node->icon        = config.icon;
  node->meta        = config.meta;
  node->factory     = config.factory;
  node->keyPoints   = config.keyPoints;

  if (!parentPathPrefix.isEmpty() && _pathIndex.contains(parentPathPrefix)) {
    NXRouteNode *parentNode = _pathIndex[parentPathPrefix];
    node->parent            = parentNode;
    parentNode->children.append(node);
  } else {
    _rootNodes.append(node);
  }

  _pathIndex.insert(fullPath, node);

  for (const auto &child : config.children) {
    NXRouterType::NavigationResult result = addRouteToTree(child, fullPath);
    if (result != NXRouterType::Success) {
      return result;
    }
  }
  return NXRouterType::Success;
}

bool NXRouterPrivate::runBeforeGuards(const QString &to, const QString &from, const QVariantMap &params, NXRouteNode *toNode)
{
  for (const auto &entry : _beforeGuards) {
    if (!entry.guard(to, from, params)) {
      return false;
    }
  }
  if (toNode && toNode->beforeEnter) {
    if (!toNode->beforeEnter(to, from, params)) {
      return false;
    }
  }
  return true;
}

void NXRouterPrivate::runAfterHooks(const QString &to, const QString &from, const QVariantMap &params)
{
  for (const auto &entry : _afterHooks) {
    entry.hook(to, from, params);
  }
}

QWidget * NXRouterPrivate::ensureWidget(NXRouteNode *node)
{
  if (node->widget) {
    bool keepAlive = node->meta.value(QStringLiteral("keepAlive"), true).toBool();
    if (keepAlive) {
      return node->widget;
    }
    node->widget->deleteLater();
    node->widget               = nullptr;
    node->isRegisteredInWindow = false;
  }
  if (node->factory) {
    node->widget = node->factory();
  }
  return node->widget;
}

bool NXRouterPrivate::registerNodeInWindow(NXRouteNode *node)
{
  if (!_window || node->isRegisteredInWindow) {
    return node->isRegisteredInWindow;
  }
  if (!node->widget) {
    return false;
  }

  NXNodeResultExpected result;
  if (node->parent && !node->parent->expanderKey.isEmpty()) {
    result = _window->addPageNode(node->title, node->widget, node->parent->expanderKey, node->keyPoints, node->icon);
  } else {
    result = _window->addPageNode(node->title, node->widget, node->keyPoints, node->icon);
  }

  if (result.has_value()) {
    node->pageKey              = node->widget->property("NXPageKey").toString();
    node->isRegisteredInWindow = true;
    return true;
  }
  return false;
}

void NXRouterPrivate::installNodeTree(NXRouteNode *node, const QString &parentExpanderKey)
{
  if (!_window) {
    return;
  }

  bool hasChildren = !node->children.isEmpty();

  if (hasChildren) {
    if (node->expanderKey.isEmpty()) {
      if (parentExpanderKey.isEmpty()) {
        node->expanderKey = _window->addExpanderNode(node->title, node->icon);
      } else {
        node->expanderKey = _window->addExpanderNode(node->title, parentExpanderKey, node->icon).value();
      }
    }

    for (auto *child : node->children) {
      installNodeTree(child, node->expanderKey);
    }
  } else if (!node->isRegisteredInWindow) {
    QWidget *widget = ensureWidget(node);
    if (widget) {
      NXNodeResultExpected result;
      if (parentExpanderKey.isEmpty()) {
        result = _window->addPageNode(node->title, widget, node->keyPoints, node->icon);
      } else {
        result = _window->addPageNode(node->title, widget, parentExpanderKey, node->keyPoints, node->icon);
      }
      if (result.has_value()) {
        node->pageKey              = widget->property("NXPageKey").toString();
        node->isRegisteredInWindow = true;
      }
    }
  }
}

NXRouterType::NavigationResult NXRouterPrivate::doNavigation(const QString &path, const QVariantMap &params, bool pushToHistory)
{
  Q_Q(NXRouter);
  if (!_window) {
    return NXRouterType::WindowNotBound;
  }

  QString normalizedPath = normalizePath(path);
  if (!_pathIndex.contains(normalizedPath)) {
    return NXRouterType::RouteNotFound;
  }

  NXRouteNode *node = _pathIndex[normalizedPath];

  if (!node->children.isEmpty()) {
    return NXRouterType::RouteNotFound;
  }

  QString fromPath = _currentPath;

  if (!runBeforeGuards(normalizedPath, fromPath, params, node)) {
    Q_EMIT q->navigationBlocked(normalizedPath);
    return NXRouterType::GuardRejected;
  }

  QWidget *widget = ensureWidget(node);
  if (!widget) {
    return NXRouterType::FactoryFailed;
  }

  if (!node->isRegisteredInWindow) {
    if (!registerNodeInWindow(node)) {
      return NXRouterType::FactoryFailed;
    }
  }

  _window->navigation(node->pageKey);

  _currentPath   = normalizedPath;
  _currentParams = params;

  if (pushToHistory && !_isNavigatingHistory) {
    if (_historyIndex < _history.size() - 1) {
      _history.resize(_historyIndex + 1);
    }
    _history.append({ normalizedPath, params });
    _historyIndex = _history.size() - 1;
  }

  runAfterHooks(normalizedPath, fromPath, params);
  Q_EMIT q->routeChanged(normalizedPath, params);

  return NXRouterType::Success;
}
