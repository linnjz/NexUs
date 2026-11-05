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
  d->m_Window = window;
}

NXWindow *NXRouter::getBoundWindow() const
{
  return d_ptr->m_Window;
}

NXRouterType::NavigationResult NXRouter::addRoute(NXRouteConfig const &config)
{
  Q_D(NXRouter);
  NXRouterType::NavigationResult result = d->addRouteToTree(config);
  if (result == NXRouterType::Success) {
    Q_EMIT routeTableChanged();
  }
  return result;
}

NXRouterType::NavigationResult NXRouter::addRoutes(QVector<NXRouteConfig> const &configs)
{
  Q_D(NXRouter);
  for (auto const &config : configs) {
    NXRouterType::NavigationResult result = d->addRouteToTree(config);
    if (result != NXRouterType::Success) {
      return result;
    }
  }
  Q_EMIT routeTableChanged();
  return NXRouterType::Success;
}

NXRouterType::NavigationResult NXRouter::addDynamicRoute(QString const &parentPath, NXRouteConfig const &config)
{
  Q_D(NXRouter);
  QString normalizedParent = NXRouterPrivate::normalizePath(parentPath);
  if (!d->m_PathIndex.contains(normalizedParent)) {
    return NXRouterType::ParentRouteNotFound;
  }
  NXRouterType::NavigationResult result = d->addRouteToTree(config, normalizedParent);
  if (result == NXRouterType::Success) {
    NXRouteNode *parentNode = d->m_PathIndex[normalizedParent];
    if (d->m_Window && parentNode->isRegisteredInWindow) {
      QString fullPath     = NXRouterPrivate::joinPath(normalizedParent, config.path);
      NXRouteNode *newNode = d->m_PathIndex.value(fullPath);
      if (newNode) {
        d->installNodeTree(newNode, parentNode->expanderKey);
      }
    }
    Q_EMIT routeTableChanged();
  }
  return result;
}

NXRouterType::NavigationResult NXRouter::removeRoute(QString const &path)
{
  Q_D(NXRouter);
  QString normalizedPath = NXRouterPrivate::normalizePath(path);
  if (!d->m_PathIndex.contains(normalizedPath)) {
    return NXRouterType::RouteNotFound;
  }
  NXRouteNode *node = d->m_PathIndex[normalizedPath];

  QVector<QString> pathsToRemove;
  std::function<void(NXRouteNode *)> collectPaths = [&](NXRouteNode *n) {
    pathsToRemove.append(n->path);
    for (auto *child : n->children) {
      collectPaths(child);
    }
  };
  collectPaths(node);

  for (auto const &p : pathsToRemove) {
    d->m_PathIndex.remove(p);
  }

  if (node->isRegisteredInWindow && d->m_Window) {
    d->m_Window->removeNavigationNode(node->pageKey);
  }

  if (node->parent) {
    node->parent->children.removeOne(node);
  } else {
    d->m_RootNodes.removeOne(node);
  }
  delete node;

  Q_EMIT routeTableChanged();
  return NXRouterType::Success;
}

bool NXRouter::hasRoute(QString const &path) const
{
  return d_ptr->m_PathIndex.contains(NXRouterPrivate::normalizePath(path));
}

QStringList NXRouter::getRoutePaths() const
{
  return d_ptr->m_PathIndex.keys();
}

QVariantMap NXRouter::getRouteMeta(QString const &path) const
{
  QString normalizedPath = NXRouterPrivate::normalizePath(path);
  if (d_ptr->m_PathIndex.contains(normalizedPath)) {
    return d_ptr->m_PathIndex[normalizedPath]->meta;
  }
  return {};
}

NXRouterType::NavigationResult NXRouter::push(QString const &path, QVariantMap const &params)
{
  Q_D(NXRouter);
  return d->doNavigation(path, params, true);
}

NXRouterType::NavigationResult NXRouter::replace(QString const &path, QVariantMap const &params)
{
  Q_D(NXRouter);
  return d->doNavigation(path, params, false);
}

void NXRouter::back()
{
  Q_D(NXRouter);
  if (d->m_History.isEmpty()) {
    return;
  }
  bool isOffRoute = !d->_isOnRouterPage();
  int targetIndex = isOffRoute ? d->m_HistoryIndex : d->m_HistoryIndex - 1;
  if (targetIndex < 0) {
    return;
  }
  d->m_HistoryIndex        = targetIndex;
  auto const &entry       = d->m_History[d->m_HistoryIndex];
  d->m_IsNavigatingHistory = true;
  d->doNavigation(entry.path, entry.params, false);
  d->m_IsNavigatingHistory = false;
}

void NXRouter::forward()
{
  Q_D(NXRouter);
  if (d->m_History.isEmpty()) {
    return;
  }
  bool isOffRoute = !d->_isOnRouterPage();
  int targetIndex = isOffRoute ? d->m_HistoryIndex : d->m_HistoryIndex + 1;
  if (targetIndex >= d->m_History.size()) {
    return;
  }
  d->m_HistoryIndex        = targetIndex;
  auto const &entry       = d->m_History[d->m_HistoryIndex];
  d->m_IsNavigatingHistory = true;
  d->doNavigation(entry.path, entry.params, false);
  d->m_IsNavigatingHistory = false;
}

QString NXRouter::getCurrentPath() const
{
  return d_ptr->m_CurrentPath;
}

QVariantMap NXRouter::getCurrentParams() const
{
  return d_ptr->m_CurrentParams;
}

int NXRouter::beforeEach(NXRouteGuard const &guard)
{
  Q_D(NXRouter);
  int id = d->m_NextGuardId++;
  d->m_BeforeGuards.append({ id, guard });
  return id;
}

int NXRouter::afterEach(NXRouteAfterHook const &hook)
{
  Q_D(NXRouter);
  int id = d->m_NextGuardId++;
  d->m_AfterHooks.append({ id, hook });
  return id;
}

void NXRouter::removeBeforeGuard(int guardId)
{
  Q_D(NXRouter);
  for (int i = 0; i < d->m_BeforeGuards.size(); ++i) {
    if (d->m_BeforeGuards[i].id == guardId) {
      d->m_BeforeGuards.removeAt(i);
      return;
    }
  }
}

void NXRouter::removeAfterHook(int hookId)
{
  Q_D(NXRouter);
  for (int i = 0; i < d->m_AfterHooks.size(); ++i) {
    if (d->m_AfterHooks[i].id == hookId) {
      d->m_AfterHooks.removeAt(i);
      return;
    }
  }
}

void NXRouter::setRouteBeforeEnter(QString const &path, NXRouteGuard const &guard)
{
  Q_D(NXRouter);
  QString normalizedPath = NXRouterPrivate::normalizePath(path);
  if (d->m_PathIndex.contains(normalizedPath)) {
    d->m_PathIndex[normalizedPath]->beforeEnter = guard;
  }
}

void NXRouter::installRoutes()
{
  Q_D(NXRouter);
  if (!d->m_Window) {
    return;
  }
  for (auto *node : d->m_RootNodes) {
    d->installNodeTree(node);
  }
}

void NXRouter::resetRouter()
{
  Q_D(NXRouter);
  qDeleteAll(d->m_RootNodes);
  d->m_RootNodes.clear();
  d->m_PathIndex.clear();
  d->m_CurrentPath.clear();
  d->m_CurrentParams.clear();
  d->m_BeforeGuards.clear();
  d->m_AfterHooks.clear();
  Q_EMIT routeTableChanged();
}
