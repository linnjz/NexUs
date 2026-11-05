#include "NXRouterPrivate.h"

#include <QWidget>

#include "NXWindow.h"

NXRouterPrivate::NXRouterPrivate(QObject *parent)
    : QObject { parent }
{
}

NXRouterPrivate::~NXRouterPrivate()
{
  qDeleteAll(m_RootNodes);
}

QString NXRouterPrivate::normalizePath(QString const &path)
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

QString NXRouterPrivate::joinPath(QString const &parent, QString const &child)
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
  if (!m_Window || m_History.isEmpty() || m_HistoryIndex < 0) {
    return false;
  }
  QString currentPageKey = m_Window->getCurrentNavigationPageKey();
  auto const &entry      = m_History[m_HistoryIndex];
  if (m_PathIndex.contains(entry.path)) {
    NXRouteNode *node = m_PathIndex[entry.path];
    return node->pageKey == currentPageKey;
  }
  return false;
}

NXRouterType::NavigationResult NXRouterPrivate::addRouteToTree(NXRouteConfig const &config, QString const &parentPathPrefix)
{
  QString fullPath;
  if (parentPathPrefix.isEmpty()) {
    fullPath = normalizePath(config.path);
  } else {
    fullPath = joinPath(parentPathPrefix, config.path);
  }

  if (m_PathIndex.contains(fullPath)) {
    return NXRouterType::RouteAlreadyExists;
  }

  NXRouteNode *node = new NXRouteNode();
  node->path        = fullPath;
  node->title       = config.title;
  node->icon        = config.icon;
  node->meta        = config.meta;
  node->factory     = config.factory;
  node->keyPoints   = config.keyPoints;

  if (!parentPathPrefix.isEmpty() && m_PathIndex.contains(parentPathPrefix)) {
    NXRouteNode *parentNode = m_PathIndex[parentPathPrefix];
    node->parent            = parentNode;
    parentNode->children.append(node);
  } else {
    m_RootNodes.append(node);
  }

  m_PathIndex.insert(fullPath, node);

  for (auto const &child : config.children) {
    NXRouterType::NavigationResult result = addRouteToTree(child, fullPath);
    if (result != NXRouterType::Success) {
      return result;
    }
  }
  return NXRouterType::Success;
}

bool NXRouterPrivate::runBeforeGuards(QString const &to, QString const &from, QVariantMap const &params, NXRouteNode *toNode)
{
  for (auto const &entry : m_BeforeGuards) {
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

void NXRouterPrivate::runAfterHooks(QString const &to, QString const &from, QVariantMap const &params)
{
  for (auto const &entry : m_AfterHooks) {
    entry.hook(to, from, params);
  }
}

QWidget *NXRouterPrivate::ensureWidget(NXRouteNode *node)
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
  if (!m_Window || node->isRegisteredInWindow) {
    return node->isRegisteredInWindow;
  }
  if (!node->widget) {
    return false;
  }

  NXNodeResultExpected result;
  if (node->parent && !node->parent->expanderKey.isEmpty()) {
    result = m_Window->addPageNode(node->title, node->widget, node->parent->expanderKey, node->keyPoints, node->icon);
  } else {
    result = m_Window->addPageNode(node->title, node->widget, node->keyPoints, node->icon);
  }

  if (result.has_value()) {
    node->pageKey              = node->widget->property("NXPageKey").toString();
    node->isRegisteredInWindow = true;
    return true;
  }
  return false;
}

void NXRouterPrivate::installNodeTree(NXRouteNode *node, QString const &parentExpanderKey)
{
  if (!m_Window) {
    return;
  }

  bool hasChildren = !node->children.isEmpty();

  if (hasChildren) {
    if (node->expanderKey.isEmpty()) {
      if (parentExpanderKey.isEmpty()) {
        node->expanderKey = m_Window->addExpanderNode(node->title, node->icon);
      } else {
        node->expanderKey = m_Window->addExpanderNode(node->title, parentExpanderKey, node->icon).value();
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
        result = m_Window->addPageNode(node->title, widget, node->keyPoints, node->icon);
      } else {
        result = m_Window->addPageNode(node->title, widget, parentExpanderKey, node->keyPoints, node->icon);
      }
      if (result.has_value()) {
        node->pageKey              = widget->property("NXPageKey").toString();
        node->isRegisteredInWindow = true;
      }
    }
  }
}

NXRouterType::NavigationResult NXRouterPrivate::doNavigation(QString const &path, QVariantMap const &params, bool pushToHistory)
{
  Q_Q(NXRouter);
  if (!m_Window) {
    return NXRouterType::WindowNotBound;
  }

  QString normalizedPath = normalizePath(path);
  if (!m_PathIndex.contains(normalizedPath)) {
    return NXRouterType::RouteNotFound;
  }

  NXRouteNode *node = m_PathIndex[normalizedPath];

  if (!node->children.isEmpty()) {
    return NXRouterType::RouteNotFound;
  }

  QString fromPath = m_CurrentPath;

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

  m_Window->navigation(node->pageKey);

  m_CurrentPath   = normalizedPath;
  m_CurrentParams = params;

  if (pushToHistory && !m_IsNavigatingHistory) {
    if (m_HistoryIndex < m_History.size() - 1) {
      m_History.resize(m_HistoryIndex + 1);
    }
    m_History.append({ normalizedPath, params });
    m_HistoryIndex = m_History.size() - 1;
  }

  runAfterHooks(normalizedPath, fromPath, params);
  Q_EMIT q->routeChanged(normalizedPath, params);

  return NXRouterType::Success;
}
