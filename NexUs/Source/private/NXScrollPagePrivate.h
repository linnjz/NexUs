#ifndef NXSCROLLPAGEPRIVATE_H
#define NXSCROLLPAGEPRIVATE_H

#include <QMap>
#include <QObject>

#include "NXProperty.h"
class NXScrollPage;
class NXScrollArea;
class QHBoxLayout;
class QVBoxLayout;
class NXText;
class QStackedWidget;
class NXBreadcrumbBar;

class NXScrollPagePrivate : public QObject
{
  friend class NXScrollPageRouteCommand;
  Q_OBJECT
  Q_D_CREATE(NXScrollPage)

public:
  explicit NXScrollPagePrivate(QObject *parent = nullptr);
  ~NXScrollPagePrivate();
  Q_INVOKABLE void onNavigationRoute(QVariantMap const &routeData);

private:
  bool m_IsGrabGesture { false };
  int m_NavigationTargetIndex { 0 };
  int m_PageTitleSpacing { 0 };
  QMap<QString, int> m_CentralWidgetMap;
  QWidget *m_TopCustomWidget { nullptr };
  QWidget *m_BottomCustomWidget { nullptr };
  QHBoxLayout *m_PageTitleLayout { nullptr };
  QVBoxLayout *m_MainLayout { nullptr };
  QStackedWidget *m_CentralStackedWidget { nullptr };
  NXBreadcrumbBar *m_BreadcrumbBar { nullptr };
  void _switchCentralStackIndex(int targetIndex, int lastIndex);
};

#endif // NXSCROLLPAGEPRIVATE_H
