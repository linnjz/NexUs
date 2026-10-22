#ifndef NXPIVOTVIEW_H
#define NXPIVOTVIEW_H

#include <QListView>
#include <QModelIndex>

#include "NXProperty.h"
class NXPivotStyle;

class NXPivotView : public QListView
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QS_SET_CREF(QModelIndex), PressIndex)
  Q_PRIVATE_CREATE(QS_SET_CREF(QModelIndex), HoverIndex)
  Q_PRIVATE_CREATE(QS_SET_CREF(QModelIndex), CommittedIndex)
  Q_PRIVATE_CREATE(NXPivotStyle *, PivotStyle)
  Q_PRIVATE_CREATE(Qt::CursorShape, ItemCursor)
  Q_PROPERTY_CREATE(int, IndicatorWidth)
  Q_PROPERTY_CREATE(int, IndicatorAnimationWidth)
  Q_PROPERTY_CREATE(int, IndicatorX)

public:
  explicit NXPivotView(QWidget *parent = nullptr);
  ~NXPivotView() override;
  bool doCurrentIndexChangedAnimation(const QModelIndex &index);
  void setPressIndexAndUpdate(const QModelIndex &index);
  void setCommittedIndexAndUpdate(const QModelIndex &index);
  void refreshHoverState();

protected:
  void wheelEvent(QWheelEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

private:
  bool _isAnimationFinished { true };
  QRect _committedIndexRect {}; // 提交索引在上次动画/提交时的快照矩形，用于布局位移补偿

  void _startIndicatorAnimation(qreal indicatorXStart, qreal indicatorXEnd, int indicatorWidthStart, int indicatorWidthEnd, const QRect &dirtyRect);

  void _setHoverIndexInternal(const QModelIndex &index);
  void _updateIndicatorRegion(QRect rect);
  void _updateIndexRect(const QModelIndex &index);
  void _updateIndexPair(const QModelIndex &first, const QModelIndex &second);
  void _syncHoverCursor();
};

#endif // NXPIVOTVIEW_H
