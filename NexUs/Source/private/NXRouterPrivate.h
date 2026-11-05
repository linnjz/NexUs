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

  NXWindow *m_Window = nullptr;

  QVector<NXRouteNode *> m_RootNodes;
  QMap<QString, NXRouteNode *> m_PathIndex;

  QString m_CurrentPath;
  QVariantMap m_CurrentParams;

  struct HistoryEntry
  {
    QString path;
    QVariantMap params;
  };

  QVector<HistoryEntry> m_History;
  int m_HistoryIndex         = -1;
  bool m_IsNavigatingHistory = false;

  QVector<NXGuardEntry> m_BeforeGuards;
  QVector<NXAfterHookEntry> m_AfterHooks;
  int m_NextGuardId = 1;

  static QString normalizePath(QString const &path);
  static QString joinPath(QString const &parent, QString const &child);

  bool _isOnRouterPage() const;
  NXRouterType::NavigationResult addRouteToTree(NXRouteConfig const &config, QString const &parentPathPrefix = QString());
  bool runBeforeGuards(QString const &to, QString const &from, QVariantMap const &params, NXRouteNode *toNode);
  void runAfterHooks(QString const &to, QString const &from, QVariantMap const &params);
  QWidget *ensureWidget(NXRouteNode *node);
  bool registerNodeInWindow(NXRouteNode *node);
  void installNodeTree(NXRouteNode *node, QString const &parentExpanderKey = QString());
  NXRouterType::NavigationResult doNavigation(QString const &path, QVariantMap const &params, bool pushToHistory);
};

#endif // NXROUTERPRIVATE_H
