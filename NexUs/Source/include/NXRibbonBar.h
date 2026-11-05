#ifndef NXRIBBONBAR_H
#define NXRIBBONBAR_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class QStackedWidget;
class QAction;
class NXRibbonGroup;
class NXRibbonTabBar;
class NXRibbonBarPrivate;

class NX_EXPORT NXRibbonBar : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXRibbonBar)
  Q_PROPERTY_CREATE_H(int, CurrentIndex)

public:
  explicit NXRibbonBar(QWidget *parent = nullptr);
  ~NXRibbonBar();

  void bindTabBar(NXRibbonTabBar *tabBar);
  NXRibbonTabBar *tabBar() const;

  QWidget *addTab(QString const &title);
  NXRibbonGroup *addGroup(QWidget *page, QString const &title);

  int tabCount() const;
  QString tabText(int index) const;

  // 页签内容超出宽度时横向滚动(整体容器级)
  // 生命周期管理:页删除时stack与tabbar三方同步 组删除含布局摘除
  void removePage(int index);
  void removePage(QWidget *page);
  void removeGroup(QWidget *page, NXRibbonGroup *group);
  QList<NXRibbonGroup *> getGroups(QWidget *page) const;

  // 整页启用/禁用 级联子控件并同步置灰对应Tab
  void setRibbonPageEnable(int index, bool isEnable);
  void setRibbonPageEnable(QWidget *page, bool isEnable);
  bool isRibbonPageEnable(int index) const;

  void setCollapsed(bool collapsed);
  bool isCollapsed() const;

  void setPinned(bool pinned);
  bool isPinned() const;

  void setAnimationDuration(int durationMs);
  int getAnimationDuration() const;

  void showPinContextMenu(QPoint globalPos);
  Q_SIGNAL void tabClicked(int index);
  Q_SIGNAL void collapsedChanged(bool collapsed);
  Q_SIGNAL void pinnedChanged(bool pinned);

  // 框架级命令总线 action为按钮的defaultAction 纯占位按钮时为nullptr
  Q_SIGNAL void ribbonActionTriggered(QString const &pageTitle, NXRibbonGroup *group, QAction *action);

protected:
  void paintEvent(QPaintEvent *event) override;
  QSize sizeHint() const override;
  bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // NXRIBBONBAR_H
