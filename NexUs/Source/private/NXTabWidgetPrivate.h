#ifndef NXTABWIDGETPRIVATE_H
#define NXTABWIDGETPRIVATE_H

#include <QDrag>
#include <QObject>
#include <QPixmap>

#include "LinnSingleton.h"
#include "NXProperty.h"

class NXDragMonitor : public QObject
{
  Q_OBJECT
  Q_SINGLETON_CREATE(QSS_UNIQUE(NXDragMonitor))
  Q_PRIVATE_CREATE(bool, IsInDrag)

private:
  explicit NXDragMonitor(QObject *parent = nullptr);
  ~NXDragMonitor();
};

class NXTabWidget;
class NXTabBar;

class NXTabWidgetPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXTabWidget)
  Q_PROPERTY_CREATE_D(QSize, TabSize)
  Q_PROPERTY_CREATE_D(QSize, FloatWidgetSize)
  Q_PROPERTY_CREATE_D(bool, IsTabTransparent)
  Q_PROPERTY_CREATE_D(bool, IsContainerAcceptDrops)

public:
  explicit NXTabWidgetPrivate(QObject *parent = nullptr);
  ~NXTabWidgetPrivate();
  Q_SLOT void onTabDragCreate(QMimeData *mimeData);
  Q_SLOT void onTabDragEnter(QMimeData *mimeData);
  Q_SLOT void onTabDragLeave(QMimeData *mimeData);
  Q_SLOT void onTabDragDrop(QMimeData *mimeData);
  Q_SLOT void onTabCloseRequested(int index);

private:
  friend class NXCustomTabWidget;
  NXTabBar *m_TabBar { nullptr };
  NXTabBar *m_CustomTabBar { nullptr };
  // 页面拖出后仍可能被 customwidget 归还，因此跟随页面生命周期而非 tab 成员关系
  QList<QPointer<QWidget>> m_AllTabWidgetList;
  QList<QPointer<NXCustomTabWidget>> m_CustomTabWidgetList;
  void _addTabWidgetToList(QWidget *widget);
  void _removeTabWidgetFromList(QWidget *widget);
  void _registerCustomTabWidget(NXCustomTabWidget *customTabWidget);
  void _unregisterCustomTabWidget(NXCustomTabWidget *customTabWidget, NXTabBar *customTabBar);
  void _clearAllTabWidgetList();
};

#endif // NXTABWIDGETPRIVATE_H
