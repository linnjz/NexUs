#include "NXNavigationRouter.h"

#include "private/NXNavigationRouterPrivate.h"

Q_PROPERTY_CREATE_CPP(NXNavigationRouter, int, MaxRouteCount)

NXNavigationRouter::NXNavigationRouter(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXNavigationRouterPrivate())
{
  Q_D(NXNavigationRouter);
  d->q_ptr           = this;
  d->_pMaxRouteCount = 25;
}

NXNavigationRouter::~NXNavigationRouter()
{
}

NXNavigationRouterType::NavigationRouteType NXNavigationRouter::navigationRoute(QObject *routeObject, const QString &routeFunctionName, const QVariantMap &routeData, Qt::ConnectionType connectionType)
{
  Q_D(NXNavigationRouter);
  if (!routeObject) {
    return NXNavigationRouterType::ObjectInvalid;
  }
  if (routeFunctionName.isEmpty()) {
    return NXNavigationRouterType::FunctionNameInvalid;
  }
  if (d->_routeList.count() == 0) {
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::BackValid);
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::ForwardInvalid);
  } else {
    if (d->_routeList.count() >= d->_pMaxRouteCount) {
      d->_routeList.removeFirst();
      d->_currentIndex -= 1;
    }
  }
  QVariantMap saveData;
  saveData.insert(QStringLiteral("NXRouteObject"), QVariant::fromValue<QObject *>(routeObject));
  saveData.insert(QStringLiteral("NXRouteFunctionName"), routeFunctionName);
  saveData.insert(QStringLiteral("NXRouteData"), routeData);
  saveData.insert(QStringLiteral("NXRouteConnectionType"), QVariant::fromValue<Qt::ConnectionType>(connectionType));
  if (d->_currentIndex != d->_routeList.count() - 1) {
    d->_routeList.remove(d->_currentIndex + 1, d->_routeList.count() - d->_currentIndex - 1);
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::ForwardInvalid);
  }
  if (d->_currentIndex <= 0) {
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::BackValid);
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::ForwardInvalid);
  }
  d->_routeList.append(saveData);
  d->_currentIndex = d->_routeList.count() - 1;
  return NXNavigationRouterType::Success;
}

void NXNavigationRouter::clearNavigationRoute()
{
  Q_D(NXNavigationRouter);
  d->_currentIndex = -1;
  d->_routeList.clear();
  Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::BackInvalid);
  Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::ForwardInvalid);
}

void NXNavigationRouter::navigationRouteBack()
{
  Q_D(NXNavigationRouter);
  if (d->_routeList.isEmpty()) {
    return;
  }
  if (d->_currentIndex == 0) {
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::BackInvalid);
  }
  if (d->_currentIndex == d->_routeList.size() - 1) {
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::ForwardValid);
  }
  QVariantMap routeSaveData = d->_routeList[d->_currentIndex];
  d->_currentIndex -= 1;
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
  if (d->_routeList.isEmpty()) {
    return;
  }
  if (d->_currentIndex <= 0) {
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::BackValid);
  }
  if (d->_currentIndex == d->_routeList.size() - 2) {
    Q_EMIT navigationRouterStateChanged(NXNavigationRouterType::ForwardInvalid);
  }
  if (d->_currentIndex < d->_routeList.size() - 1) {
    d->_currentIndex += 1;
  }
  QVariantMap routeSaveData = d->_routeList[d->_currentIndex];
  QObject *routeObject      = routeSaveData.value(QStringLiteral("NXRouteObject")).value<QObject *>();
  QString routeFunctionName = routeSaveData.value(QStringLiteral("NXRouteFunctionName")).toString();
  QVariantMap routeData     = routeSaveData.value(QStringLiteral("NXRouteData")).toMap();
  routeData.insert(QStringLiteral("NXRouteBackMode"), false);
  Qt::ConnectionType connectionType =
      routeSaveData.value(QStringLiteral("NXRouteConnectionType")).value<Qt::ConnectionType>();
  QMetaObject::invokeMethod(routeObject, routeFunctionName.toLocal8Bit().constData(), connectionType, Q_ARG(QVariantMap, routeData));
}

NXNavigationRouterType::NavigationRouteType NXNavigationRouter::navigationRoute(QObject *context, QObject *routeObject, const QString &routeFunctionName, const QVariantMap &routeData, Qt::ConnectionType connectionType)
{
  Q_D(NXNavigationRouter);
  if (!routeObject) {
    return NXNavigationRouterType::ObjectInvalid;
  }
  if (routeFunctionName.isEmpty()) {
    return NXNavigationRouterType::FunctionNameInvalid;
  }
  auto &ctx = d->_contextMap[context];
  if (ctx.routeList.count() == 0) {
    Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::BackValid);
    Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::ForwardInvalid);
  } else {
    if (ctx.routeList.count() >= d->_pMaxRouteCount) {
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
  auto &ctx        = d->_contextMap[context];
  ctx.currentIndex = -1;
  ctx.routeList.clear();
  Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::BackInvalid);
  Q_EMIT windowRouterStateChanged(context, NXNavigationRouterType::ForwardValid);
}

void NXNavigationRouter::navigationRouteBack(QObject *context)
{
  Q_D(NXNavigationRouter);
  if (!d->_contextMap.contains(context)) {
    return;
  }
  auto &ctx = d->_contextMap[context];
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
  if (!d->_contextMap.contains(context)) {
    return;
  }
  auto &ctx = d->_contextMap[context];
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
