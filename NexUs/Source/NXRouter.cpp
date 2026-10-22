#include "NXRouter.h"

#include "NXNavigationRouter.h"
#include "NXWindow.h"
#include "private/NXRouterPrivate.h"

NXRouter::NXRouter(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXRouterPrivate())
{
  Q_D(NXRouter);
  d->q_ptr = this;
}

NXRouter::~NXRouter()
{
}

void NXRouter::bindWindow(NXWindow *window)
{
  Q_D(NXRouter);
  d->_window = window;
}

NXWindow * NXRouter::getBoundWindow() const
{
  return d_ptr->_window;
}

NXRouterType::NavigationResult NXRouter::addRoute(const NXRouteConfig &config)
{
  Q_D(NXRouter);
  NXRouterType::NavigationResult result = d->addRouteToTree(config);
  if (result == NXRouterType::Success) {
    Q_EMIT routeTableChanged();
  }
  return result;
}

NXRouterType::NavigationResult NXRouter::addRoutes(const QVector<NXRouteConfig> &configs)
{
  Q_D(NXRouter);
  for (const auto &config : configs) {
    NXRouterType::NavigationResult result = d->addRouteToTree(config);
    if (result != NXRouterType::Success) {
      return result;
    }
  }
  Q_EMIT routeTableChanged();
  return NXRouterType::Success;
}

NXRouterType::NavigationResult NXRouter::addDynamicRoute(const QString &parentPath, const NXRouteConfig &config)
{
  Q_D(NXRouter);
  QString normalizedParent = NXRouterPrivate::normalizePath(parentPath);
  if (!d->_pathIndex.contains(normalizedParent)) {
    return NXRouterType::ParentRouteNotFound;
  }
  NXRouterType::NavigationResult result = d->addRouteToTree(config, normalizedParent);
  if (result == NXRouterType::Success) {
    NXRouteNode *parentNode = d->_pathIndex[normalizedParent];
    if (d->_window && parentNode->isRegisteredInWindow) {
      QString fullPath     = NXRouterPrivate::joinPath(normalizedParent, config.path);
      NXRouteNode *newNode = d->_pathIndex.value(fullPath);
      if (newNode) {
        d->installNodeTree(newNode, parentNode->expanderKey);
      }
    }
    Q_EMIT routeTableChanged();
  }
  return result;
}

NXRouterType::NavigationResult NXRouter::removeRoute(const QString &path)
{
  Q_D(NXRouter);
  QString normalizedPath = NXRouterPrivate::normalizePath(path);
  if (!d->_pathIndex.contains(normalizedPath)) {
    return NXRouterType::RouteNotFound;
  }
  NXRouteNode *node = d->_pathIndex[normalizedPath];

  QVector<QString> pathsToRemove;
  std::function<void(NXRouteNode *)> collectPaths = [&](NXRouteNode *n) {
    pathsToRemove.append(n->path);
    for (auto *child : n->children) {
      collectPaths(child);
    }
  };
  collectPaths(node);

  for (const auto &p : pathsToRemove) {
    d->_pathIndex.remove(p);
  }

  if (node->isRegisteredInWindow && d->_window) {
    d->_window->removeNavigationNode(node->pageKey);
  }

  if (node->parent) {
    node->parent->children.removeOne(node);
  } else {
    d->_rootNodes.removeOne(node);
  }
  delete node;

  Q_EMIT routeTableChanged();
  return NXRouterType::Success;
}

bool NXRouter::hasRoute(const QString &path) const
{
  return d_ptr->_pathIndex.contains(NXRouterPrivate::normalizePath(path));
}

QStringList NXRouter::getRoutePaths() const
{
  return d_ptr->_pathIndex.keys();
}

QVariantMap NXRouter::getRouteMeta(const QString &path) const
{
  QString normalizedPath = NXRouterPrivate::normalizePath(path);
  if (d_ptr->_pathIndex.contains(normalizedPath)) {
    return d_ptr->_pathIndex[normalizedPath]->meta;
  }
  return {};
}

NXRouterType::NavigationResult NXRouter::push(const QString &path, const QVariantMap &params)
{
  Q_D(NXRouter);
  return d->doNavigation(path, params, true);
}

NXRouterType::NavigationResult NXRouter::replace(const QString &path, const QVariantMap &params)
{
  Q_D(NXRouter);
  return d->doNavigation(path, params, false);
}

void NXRouter::back()
{
  Q_D(NXRouter);
  if (d->_history.isEmpty()) {
    return;
  }
  bool isOffRoute = !d->_isOnRouterPage();
  int targetIndex = isOffRoute ? d->_historyIndex : d->_historyIndex - 1;
  if (targetIndex < 0) {
    return;
  }
  d->_historyIndex        = targetIndex;
  const auto &entry       = d->_history[d->_historyIndex];
  d->_isNavigatingHistory = true;
  d->doNavigation(entry.path, entry.params, false);
  d->_isNavigatingHistory = false;
}

void NXRouter::forward()
{
  Q_D(NXRouter);
  if (d->_history.isEmpty()) {
    return;
  }
  bool isOffRoute = !d->_isOnRouterPage();
  int targetIndex = isOffRoute ? d->_historyIndex : d->_historyIndex + 1;
  if (targetIndex >= d->_history.size()) {
    return;
  }
  d->_historyIndex        = targetIndex;
  const auto &entry       = d->_history[d->_historyIndex];
  d->_isNavigatingHistory = true;
  d->doNavigation(entry.path, entry.params, false);
  d->_isNavigatingHistory = false;
}

QString NXRouter::getCurrentPath() const
{
  return d_ptr->_currentPath;
}

QVariantMap NXRouter::getCurrentParams() const
{
  return d_ptr->_currentParams;
}

int NXRouter::beforeEach(const NXRouteGuard &guard)
{
  Q_D(NXRouter);
  int id = d->_nextGuardId++;
  d->_beforeGuards.append({ id, guard });
  return id;
}

int NXRouter::afterEach(const NXRouteAfterHook &hook)
{
  Q_D(NXRouter);
  int id = d->_nextGuardId++;
  d->_afterHooks.append({ id, hook });
  return id;
}

void NXRouter::removeBeforeGuard(int guardId)
{
  Q_D(NXRouter);
  for (int i = 0; i < d->_beforeGuards.size(); ++i) {
    if (d->_beforeGuards[i].id == guardId) {
      d->_beforeGuards.removeAt(i);
      return;
    }
  }
}

void NXRouter::removeAfterHook(int hookId)
{
  Q_D(NXRouter);
  for (int i = 0; i < d->_afterHooks.size(); ++i) {
    if (d->_afterHooks[i].id == hookId) {
      d->_afterHooks.removeAt(i);
      return;
    }
  }
}

void NXRouter::setRouteBeforeEnter(const QString &path, const NXRouteGuard &guard)
{
  Q_D(NXRouter);
  QString normalizedPath = NXRouterPrivate::normalizePath(path);
  if (d->_pathIndex.contains(normalizedPath)) {
    d->_pathIndex[normalizedPath]->beforeEnter = guard;
  }
}

void NXRouter::installRoutes()
{
  Q_D(NXRouter);
  if (!d->_window) {
    return;
  }
  for (auto *node : d->_rootNodes) {
    d->installNodeTree(node);
  }
}

void NXRouter::resetRouter()
{
  Q_D(NXRouter);
  qDeleteAll(d->_rootNodes);
  d->_rootNodes.clear();
  d->_pathIndex.clear();
  d->_currentPath.clear();
  d->_currentParams.clear();
  d->_beforeGuards.clear();
  d->_afterHooks.clear();
  Q_EMIT routeTableChanged();
}
