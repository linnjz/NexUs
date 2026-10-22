#ifndef NXROUTERPRIVATE_H
#define NXROUTERPRIVATE_H

#include <functional>

#include <QMap>
#include <QObject>
#include <QVector>

#include "NXDef.h"
#include "NXRouter.h"

class NXWindow;

struct NXRouteNode
{
  QString path;
  QString title;
  NXIconType::IconName icon = NXIconType::None;
  QVariantMap meta;
  std::function<QWidget *()> factory;
  int keyPoints = 0;

  NXRouteNode *parent = nullptr;
  QVector<NXRouteNode *> children;

  QWidget *widget = nullptr;
  QString pageKey;
  bool isRegisteredInWindow = false;
  QString expanderKey;

  NXRouteGuard beforeEnter;

  ~NXRouteNode() { qDeleteAll(children); }
};

struct NXGuardEntry
{
  int id;
  NXRouteGuard guard;
};

struct NXAfterHookEntry
{
  int id;
  NXRouteAfterHook hook;
};

class NXRouterPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXRouter)

public:
  explicit NXRouterPrivate(QObject *parent = nullptr);
  ~NXRouterPrivate();

private:
  friend class NXRouter;

  NXWindow *_window = nullptr;

  QVector<NXRouteNode *> _rootNodes;
  QMap<QString, NXRouteNode *> _pathIndex;

  QString _currentPath;
  QVariantMap _currentParams;

  struct HistoryEntry
  {
    QString path;
    QVariantMap params;
  };

  QVector<HistoryEntry> _history;
  int _historyIndex         = -1;
  bool _isNavigatingHistory = false;

  QVector<NXGuardEntry> _beforeGuards;
  QVector<NXAfterHookEntry> _afterHooks;
  int _nextGuardId = 1;

  static QString normalizePath(const QString &path);
  static QString joinPath(const QString &parent, const QString &child);

  bool _isOnRouterPage() const;
  NXRouterType::NavigationResult addRouteToTree(const NXRouteConfig &config, const QString &parentPathPrefix = QString());
  bool runBeforeGuards(const QString &to, const QString &from, const QVariantMap &params, NXRouteNode *toNode);
  void runAfterHooks(const QString &to, const QString &from, const QVariantMap &params);
  QWidget *ensureWidget(NXRouteNode *node);
  bool registerNodeInWindow(NXRouteNode *node);
  void installNodeTree(NXRouteNode *node, const QString &parentExpanderKey = QString());
  NXRouterType::NavigationResult doNavigation(const QString &path, const QVariantMap &params, bool pushToHistory);
};

#endif // NXROUTERPRIVATE_H
