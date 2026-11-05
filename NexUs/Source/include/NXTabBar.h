#ifndef NXTABBAR_H
#define NXTABBAR_H

#include <QDrag>
#include <QTabBar>

#include "NXDef.h"
class NXTabBarPrivate;

class NX_EXPORT NXTabBar : public QTabBar
{
  Q_OBJECT
  Q_Q_CREATE(NXTabBar)
  Q_PROPERTY_CREATE_H(QSize, TabSize)
  Q_PROPERTY_CREATE_H(NXTabBarType::TabBarStyle, TabBarStyle)
  Q_PROPERTY_CREATE_H(int, TabCornerRadius)
  Q_PROPERTY_CREATE_H(bool, IsSelectedIndicatorVisible)

  void setTabText(int index, QString const &text);

public:
  explicit NXTabBar(QWidget *parent = nullptr);
  ~NXTabBar();

  // 将内部滚动平滑定位到当前选中Tab 使其位于最左侧
  void locateCurrentTab();

  Q_SIGNAL void tabBarTextChanged(int tabIndex, QString const &tabText);
  Q_SIGNAL void tabDragCreate(QMimeData *mimeData);
  Q_SIGNAL void tabDragEnter(QMimeData *mimeData);
  Q_SIGNAL void tabDragLeave(QMimeData *mimeData);
  Q_SIGNAL void tabDragDrop(QMimeData *mimeData);

protected:
  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;
  void tabInserted(int index) override;
  void tabRemoved(int index) override;
  void resizeEvent(QResizeEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXTABBAR_H
