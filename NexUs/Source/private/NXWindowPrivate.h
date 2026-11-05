#ifndef NXWINDOWPRIVATE_H
#define NXWINDOWPRIVATE_H

#include <QLinearGradient>
#include <QMap>
#include <QMovie>
#include <QObject>

#include "NXDef.h"

class NXEvent;
class NXWindow;
class NXNavigationBar;
class NXAppBar;
class NXCentralStackedWidget;
class QHBoxLayout;
class QVBoxLayout;
class NXThemeAnimationWidget;

class NXWindowPrivate : public QObject
{
  friend class NXWindowStackChangeCommand;
  Q_OBJECT
  Q_D_CREATE(NXWindow)
  Q_PROPERTY_CREATE_D(int, ThemeChangeTime)
  Q_PROPERTY_CREATE_D(NXNavigationType::NavigationDisplayMode, NavigationBarDisplayMode)
  Q_PROPERTY_CREATE_D(NXWindowType::StackSwitchMode, StackSwitchMode)
  Q_PROPERTY_CREATE_D(NXWindowType::PaintMode, WindowPaintMode)

public:
  explicit NXWindowPrivate(QObject *parent = nullptr);
  ~NXWindowPrivate();
  Q_SLOT void onNavigationButtonClicked();
  Q_INVOKABLE void onWMWindowClickedEvent(QVariantMap const &data);
  Q_SLOT void onThemeReadyChange();
  Q_SLOT void onThemeModeChanged(NXThemeType::ThemeMode themeMode);
  Q_SLOT void onWindowDisplayModeChanged();
  Q_SLOT void
  onNavigationNodeClicked(NXNavigationType::NavigationNodeType nodeType, QString const &nodeKey, bool isRouteBack);
  Q_SLOT void
  onNavigationNodeAdded(NXNavigationType::NavigationNodeType nodeType, QString const &nodeKey, QWidget *page);
  Q_SLOT void onNavigationNodeRemoved(NXNavigationType::NavigationNodeType nodeType, QString const &nodeKey);
  Q_SLOT void onNavigationRouterStateChanged(NXNavigationRouterType::RouteMode routeMode);
  Q_INVOKABLE void onNavigationRoute(QVariantMap const &routeData);

private:
  bool m_IsInitFinished { false };
  bool m_IsNavigationDisplayModeChanged { false };
  bool m_IsNavigationEnable { true };
  bool m_IsNavigationBarExpanded { false };
  bool m_IsNavigationBarFloat { false };
  NXThemeType::ThemeMode m_ThemeMode;
  NXApplicationType::WindowDisplayMode m_WindowDisplayMode;
  int m_ContentsMargins { 5 };
  NXNavigationType::NavigationDisplayMode m_CurrentNavigationBarDisplayMode { NXNavigationType::Maximal };
  int m_CentralStackTargetIndex { 0 };
  int m_NavigationTargetIndex { 0 };
  QMap<QString, QMetaObject const *> m_PageMetaMap;
  QMap<QString, QWidget *> m_RouteMap; // key__nodeKey title可以一致  value__Page
  QString m_LightWindowMoviePath { QStringLiteral("") };
  QString m_DarkWindowMoviePath { QStringLiteral("") };
  QMovie *m_WindowPaintMovie { nullptr };
  QPixmap *m_LightWindowPix;
  QPixmap *m_DarkWindowPix;
  NXEvent *m_FocusEvent { nullptr };
  NXNavigationBar *m_NavigationBar { nullptr };
  NXCentralStackedWidget *m_CenterStackedWidget { nullptr };
  NXCentralStackedWidget *m_NavigationCenterStackedWidget { nullptr };
  NXAppBar *m_AppBar { nullptr };
  QHBoxLayout *m_CenterLayout { nullptr };
  NXThemeAnimationWidget *m_AnimationWidget { nullptr };


  qreal _distance(QPoint point1, QPoint point2);
  void _resetWindowLayout(bool isAnimation);
  void _doNavigationDisplayModeChange();
};

#endif // NXWINDOWPRIVATE_H
