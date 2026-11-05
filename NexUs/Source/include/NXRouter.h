#ifndef NXROUTER_H
#define NXROUTER_H

#include <functional>

#include <QObject>
#include <QVariantMap>

#include "LinnSingleton.h"
#include "NXDef.h"
#include "NXProperty.h"

class NXWindow;
class NXRouterPrivate;

struct NX_EXPORT NXRouteConfig
{
  QString path;
  QString title;
  NXIconType::IconName icon = NXIconType::None;
  QVariantMap meta;
  std::function<QWidget *()> factory;
  QVector<NXRouteConfig> children;
  int keyPoints = 0;
};

using NXRouteGuard     = std::function<bool(QString const &to, QString const &from, QVariantMap const &params)>;
using NXRouteAfterHook = std::function<void(QString const &to, QString const &from, QVariantMap const &params)>;

class NX_EXPORT NXRouter : public QObject
{
  Q_OBJECT
  Q_Q_CREATE(NXRouter)
  Q_SINGLETON_CREATE(QSS_UNIQUE(NXRouter))

private:
  explicit NXRouter(QObject *parent = nullptr);
  ~NXRouter();

public:
  void bindWindow(NXWindow *window);
  NXWindow *getBoundWindow() const;

  NXRouterType::NavigationResult addRoute(NXRouteConfig const &config);
  NXRouterType::NavigationResult addRoutes(QVector<NXRouteConfig> const &configs);
  NXRouterType::NavigationResult addDynamicRoute(QString const &parentPath, NXRouteConfig const &config);
  NXRouterType::NavigationResult removeRoute(QString const &path);

  bool hasRoute(QString const &path) const;
  QStringList getRoutePaths() const;
  QVariantMap getRouteMeta(QString const &path) const;

  NXRouterType::NavigationResult push(QString const &path, QVariantMap const &params = {});
  NXRouterType::NavigationResult replace(QString const &path, QVariantMap const &params = {});
  void back();
  void forward();

  QString getCurrentPath() const;
  QVariantMap getCurrentParams() const;

  int beforeEach(NXRouteGuard const &guard);
  int afterEach(NXRouteAfterHook const &hook);
  void removeBeforeGuard(int guardId);
  void removeAfterHook(int hookId);

  void setRouteBeforeEnter(QString const &path, NXRouteGuard const &guard);

  void installRoutes();
  void resetRouter();
  Q_SIGNAL void routeChanged(QString const &path, QVariantMap const &params);
  Q_SIGNAL void navigationBlocked(QString const &path);
  Q_SIGNAL void routeTableChanged();
};

#define eRouter NXRouter::getInstance()

#endif // NXROUTER_H
