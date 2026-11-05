#ifndef NXPIVOTVIEW_H
#define NXPIVOTVIEW_H

#include <QListView>
#include <QModelIndex>

#include "NXProperty.h"
class NXPivotStyle;

class NXPivotView : public QListView
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QPS_SCREF(QModelIndex), PressIndex)
  Q_PRIVATE_CREATE(QPS_SCREF(QModelIndex), HoverIndex)
  Q_PRIVATE_CREATE(QPS_SCREF(QModelIndex), CommittedIndex)
  Q_PRIVATE_CREATE(NXPivotStyle *, PivotStyle)
  Q_PRIVATE_CREATE(Qt::CursorShape, ItemCursor)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), IndicatorWidth)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), IndicatorAnimationWidth)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), IndicatorX)

public:
  explicit NXPivotView(QWidget *parent = nullptr);
  ~NXPivotView() override;
  bool doCurrentIndexChangedAnimation(QModelIndex const &index);
  void setPressIndexAndUpdate(QModelIndex const &index);
  void setCommittedIndexAndUpdate(QModelIndex const &index);
  void refreshHoverState();

protected:
  void wheelEvent(QWheelEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

private:
  bool m_IsAnimationFinished { true };
  QRect m_CommittedIndexRect {}; // 提交索引在上次动画/提交时的快照矩形，用于布局位移补偿

  void _startIndicatorAnimation(qreal indicatorXStart, qreal indicatorXEnd, int indicatorWidthStart, int indicatorWidthEnd, QRect const &dirtyRect);

  void _setHoverIndexInternal(QModelIndex const &index);
  void _updateIndicatorRegion(QRect rect);
  void _updateIndexRect(QModelIndex const &index);
  void _updateIndexPair(QModelIndex const &first, QModelIndex const &second);
  void _syncHoverCursor();
};

#endif // NXPIVOTVIEW_H
