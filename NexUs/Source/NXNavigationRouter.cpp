#include "NXNavigationRouter.h"

#include "private/NXNavigationRouterPrivate.h"

Q_PROPERTY_CREATE_CPP(NXNavigationRouter, QPS_PAEMIT(int), MaxRouteCount)

NXNavigationRouter::NXNavigationRouter(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXNavigationRouterPrivate())
{
  Q_D(NXNavigationRouter);
  d->q_ptr           = this;
  d->m_MaxRouteCount = 25;
}

NXNavigationRouter::~NXNavigationRouter()
{
}

NXNavigationRouterType::NavigationRouteType NXNavigationRouter::navigationRoute(QObject *routeObject, QString const &routeFunctionName, QVariantMap const &routeData, Qt::ConnectionType connectionType)
{
  Q_D(NXNavigationRouter);
  if (!routeObject) {
    return NXNavigationRouterType::ObjectInvalid;
  }
  if (routeFunctionName.isEmpty()) {
    return NXNavigationRouterType::FunctionNameInvalid;
  }
  if (d->m_RouteList.count() == 0) {
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::BackValid);
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::ForwardInvalid);
  } else {
    if (d->m_RouteList.count() >= d->m_MaxRouteCount) {
      d->m_RouteList.removeFirst();
      d->m_CurrentIndex -= 1;
    }
  }
  QVariantMap saveData;
  saveData.insert(QStringLiteral("NXRouteObject"), QVariant::fromValue<QObject *>(routeObject));
  saveData.insert(QStringLiteral("NXRouteFunctionName"), routeFunctionName);
  saveData.insert(QStringLiteral("NXRouteData"), routeData);
  saveData.insert(QStringLiteral("NXRouteConnectionType"), QVariant::fromValue<Qt::ConnectionType>(connectionType));
  if (d->m_CurrentIndex != d->m_RouteList.count() - 1) {
    d->m_RouteList.remove(d->m_CurrentIndex + 1, d->m_RouteList.count() - d->m_CurrentIndex - 1);
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::ForwardInvalid);
  }
  if (d->m_CurrentIndex <= 0) {
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::BackValid);
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::ForwardInvalid);
  }
  d->m_RouteList.append(saveData);
  d->m_CurrentIndex = d->m_RouteList.count() - 1;
  return NXNavigationRouterType::Success;
}

void NXNavigationRouter::clearNavigationRoute()
{
  Q_D(NXNavigationRouter);
  d->m_CurrentIndex = -1;
  d->m_RouteList.clear();
  Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::BackInvalid);
  Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::ForwardInvalid);
}

void NXNavigationRouter::navigationRouteBack()
{
  Q_D(NXNavigationRouter);
  if (d->m_RouteList.isEmpty()) {
    return;
  }
  if (d->m_CurrentIndex == 0) {
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::BackInvalid);
  }
  if (d->m_CurrentIndex == d->m_RouteList.size() - 1) {
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::ForwardValid);
  }
  QVariantMap routeSaveData = d->m_RouteList[d->m_CurrentIndex];
  d->m_CurrentIndex -= 1;
  QObject *routeObject      = routeSaveData.value(QStringLiteral("NXRouteObject")).value<QObject *>();
  QString routeFunctionName = routeSaveData.value(QStringLiteral("NXRouteFunctionName")).toString();
  QVariantMap routeData     = routeSaveData.value(QStringLiteral("NXRouteData")).toMap();
  routeData.insert(QStringLiteral("NXRouteBackMode"), true);
  Qt::ConnectionType connectionType =
      routeSaveData.value(QStringLiteral("NXRouteConnectionType")).value<Qt::ConnectionType>();
  QMetaObject::invokeMethod(routeObject, routeFunctionName.toLocal8Bit().constData(), connectionType, Q_ARG(QVariantMap, routeData));
}

void NXNavigationRouter::navigationRouteForward()
{
  Q_D(NXNavigationRouter);
  if (d->m_RouteList.isEmpty()) {
    return;
  }
  if (d->m_CurrentIndex <= 0) {
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::BackValid);
  }
  if (d->m_CurrentIndex == d->m_RouteList.size() - 2) {
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::ForwardInvalid);
  }
  if (d->m_CurrentIndex < d->m_RouteList.size() - 1) {
    d->m_CurrentIndex += 1;
  }
  QVariantMap routeSaveData = d->m_RouteList[d->m_CurrentIndex];
  QObject *routeObject      = routeSaveData.value(QStringLiteral("NXRouteObject")).value<QObject *>();
  QString routeFunctionName = routeSaveData.value(QStringLiteral("NXRouteFunctionName")).toString();
  QVariantMap routeData     = routeSaveData.value(QStringLiteral("NXRouteData")).toMap();
  routeData.insert(QStringLiteral("NXRouteBackMode"), false);
  Qt::ConnectionType connectionType =
      routeSaveData.value(QStringLiteral("NXRouteConnectionType")).value<Qt::ConnectionType>();
  QMetaObject::invokeMethod(routeObject, routeFunctionName.toLocal8Bit().constData(), connectionType, Q_ARG(QVariantMap, routeData));
}

NXNavigationRouterType::NavigationRouteType NXNavigationRouter::navigationRoute(QObject *context, QObject *routeObject, QString const &routeFunctionName, QVariantMap const &routeData, Qt::ConnectionType connectionType)
{
  Q_D(NXNavigationRouter);
  if (!routeObject) {
    return NXNavigationRouterType::ObjectInvalid;
  }
  if (routeFunctionName.isEmpty()) {
    return NXNavigationRouterType::FunctionNameInvalid;
  }
  auto &ctx = d->m_ContextMap[context];
  if (ctx.routeList.count() == 0) {
    Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::BackValid);
    Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::ForwardInvalid);
  } else {
    if (ctx.routeList.count() >= d->m_MaxRouteCount) {
      ctx.routeList.removeFirst();
      ctx.currentIndex -= 1;
    }
  }
  QVariantMap saveData;
  saveData.insert(QStringLiteral("NXRouteObject"), QVariant::fromValue<QObject *>(routeObject));
  saveData.insert(QStringLiteral("NXRouteFunctionName"), routeFunctionName);
  saveData.insert(QStringLiteral("NXRouteData"), routeData);
  saveData.insert(QStringLiteral("NXRouteConnectionType"), QVariant::fromValue<Qt::ConnectionType>(connectionType));
  if (ctx.currentIndex != ctx.routeList.count() - 1) {
    if (ctx.currentIndex == -1) {
      ctx.routeList.clear();
    } else {
      ctx.routeList.remove(ctx.currentIndex + 1, ctx.routeList.count() - ctx.currentIndex - 1);
    }
    Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::ForwardInvalid);
  }
  if (ctx.currentIndex <= 0) {
    Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::BackValid);
    Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::ForwardInvalid);
  }
  ctx.routeList.append(saveData);
  ctx.currentIndex = ctx.routeList.count() - 1;
  return NXNavigationRouterType::Success;
}

void NXNavigationRouter::clearNavigationRoute(QObject *context)
{
  Q_D(NXNavigationRouter);
  auto &ctx        = d->m_ContextMap[context];
  ctx.currentIndex = -1;
  ctx.routeList.clear();
  Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::BackInvalid);
  Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::ForwardValid);
}

void NXNavigationRouter::navigationRouteBack(QObject *context)
{
  Q_D(NXNavigationRouter);
  if (!d->m_ContextMap.contains(context)) {
    return;
  }
  auto &ctx = d->m_ContextMap[context];
  if (ctx.routeList.isEmpty()) {
    return;
  }
  if (ctx.currentIndex == 0) {
    Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::BackInvalid);
  }
  if (ctx.currentIndex == ctx.routeList.size() - 1) {
    Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::ForwardValid);
  }
  QVariantMap routeSaveData = ctx.routeList[ctx.currentIndex];
  ctx.currentIndex -= 1;
  QObject *routeObject      = routeSaveData.value(QStringLiteral("NXRouteObject")).value<QObject *>();
  QString routeFunctionName = routeSaveData.value(QStringLiteral("NXRouteFunctionName")).toString();
  QVariantMap routeData     = routeSaveData.value(QStringLiteral("NXRouteData")).toMap();
  routeData.insert(QStringLiteral("NXRouteBackMode"), true);
  Qt::ConnectionType connectionType =
      routeSaveData.value(QStringLiteral("NXRouteConnectionType")).value<Qt::ConnectionType>();
  QMetaObject::invokeMethod(routeObject, routeFunctionName.toLocal8Bit().constData(), connectionType, Q_ARG(QVariantMap, routeData));
}

void NXNavigationRouter::navigationRouteForward(QObject *context)
{
  Q_D(NXNavigationRouter);
  if (!d->m_ContextMap.contains(context)) {
    return;
  }
  auto &ctx = d->m_ContextMap[context];
  if (ctx.routeList.isEmpty()) {
    return;
  }
  if (ctx.currentIndex <= 0) {
    Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::BackValid);
  }
  if (ctx.currentIndex == ctx.routeList.size() - 2) {
    Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::ForwardInvalid);
  }
  if (ctx.currentIndex < ctx.routeList.size() - 1) {
    ctx.currentIndex += 1;
  }
  QVariantMap routeSaveData = ctx.routeList[ctx.currentIndex];
  QObject *routeObject      = routeSaveData.value(QStringLiteral("NXRouteObject")).value<QObject *>();
  QString routeFunctionName = routeSaveData.value(QStringLiteral("NXRouteFunctionName")).toString();
  QVariantMap routeData     = routeSaveData.value(QStringLiteral("NXRouteData")).toMap();
  routeData.insert(QStringLiteral("NXRouteBackMode"), false);
  Qt::ConnectionType connectionType =
      routeSaveData.value(QStringLiteral("NXRouteConnectionType")).value<Qt::ConnectionType>();
  QMetaObject::invokeMethod(routeObject, routeFunctionName.toLocal8Bit().constData(), connectionType, Q_ARG(QVariantMap, routeData));
}
