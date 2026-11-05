#ifndef NXNAVIGATIONROUTER_H
#define NXNAVIGATIONROUTER_H

#include <QObject>
#include <QVariantMap>

#include "LinnSingleton.h"
#include "NXDef.h"

class NXNavigationRouterPrivate;

class NX_EXPORT NXNavigationRouter : public QObject
{
  Q_OBJECT
  Q_Q_CREATE(NXNavigationRouter)
  Q_SINGLETON_CREATE(QSS_UNIQUE(NXNavigationRouter))
  Q_PROPERTY_CREATE_H(int, MaxRouteCount)

private:
  explicit NXNavigationRouter(QObject *parent = nullptr);
  ~NXNavigationRouter();

public:
  // 全局路由（向后兼容）
  NXNavigationRouterType::NavigationRouteType navigationRoute(QObject *routeObject, QString const &routeFunctionName, QVariantMap const &routeData = {}, Qt::ConnectionType connectionType = Qt::AutoConnection);
  void clearNavigationRoute();
  void navigationRouteBack();
  void navigationRouteForward();

  // 每窗口独立路由
  NXNavigationRouterType::NavigationRouteType navigationRoute(QObject *context, QObject *routeObject, QString const &routeFunctionName, QVariantMap const &routeData = {}, Qt::ConnectionType connectionType = Qt::AutoConnection);
  void clearNavigationRoute(QObject *context);
  void navigationRouteBack(QObject *context);
  void navigationRouteForward(QObject *context);
  Q_SIGNAL void navigationRouterStateChanged(NXNavigationRouterType::RouteMode routeMode);
  Q_SIGNAL void windowRouterStateChanged(QObject *context, NXNavigationRouterType::RouteMode routeMode);
};

#endif // NXNAVIGATIONROUTER_H
