#include "NXPivotView.h"

#include <QCursor>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QtGlobal>

#include "NXPivotStyle.h"
#include "NXScrollBar.h"

NXPivotView::NXPivotView(QWidget *parent)
    : QListView(parent)
{
  _pIndicatorX              = 0;
  _pIndicatorWidth          = 40;
  _pIndicatorAnimationWidth = 0;
  _pPressIndex              = {};
  _pHoverIndex              = {};
  _pCommittedIndex          = {};
  _pItemCursor              = Qt::PointingHandCursor;
  setObjectName("NXPivotView");
  setStyleSheet(QStringLiteral("#NXPivotView{background-color:transparent;}"));
  setMouseTracking(true);
  setVerticalScrollBar(new NXScrollBar(this));
  setHorizontalScrollBar(new NXScrollBar(this));
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

NXPivotView::~NXPivotView()
{
}

bool NXPivotView::doCurrentIndexChangedAnimation(const QModelIndex &index)
{
  QListView::setCurrentIndex(index);
  const QModelIndex previousIndex = _pCommittedIndex;
  setCommittedIndexAndUpdate(index);
  // 当前项相同且指示器已显示时跳过；初次选中但宽度仍为 0 时仍需播放生长动画
  if (previousIndex == index && previousIndex.isValid() && _pIndicatorAnimationWidth > 0) {
    return false;
  }
  if (!this->isVisible() ||
      !(_pPivotStyle->getIndicatorTypeFlags() & (NXPivotType::IndicatorTop | NXPivotType::IndicatorBottom))) {
    if (index.isValid()) {
      _committedIndexRect = visualRect(index);
    } else {
      _committedIndexRect = QRect {};
    }
    return previousIndex != index;
  }
  const QRect previousIndexRect = _committedIndexRect;
  qreal indicatorXStart         = 0;
  qreal indicatorXEnd           = 0;
  int indicatorWidthEnd         = 0;
  if (index.isValid()) {
    _committedIndexRect = visualRect(index);
    indicatorXEnd       = _committedIndexRect.center().x() - _pIndicatorWidth / 2;
    if (previousIndex.isValid()) {
      // 以快照矩形为基准补偿布局位移；静止时位移为 0，不会叠加目标位移
      const int layoutShift =
          previousIndexRect.isValid() ? visualRect(previousIndex).center().x() - previousIndexRect.center().x() : 0;
      indicatorXStart = _pIndicatorX + layoutShift;
    } else {
      indicatorXStart = _committedIndexRect.center().x();
    }
    indicatorWidthEnd = _pIndicatorWidth;
  } else {
    if (!previousIndex.isValid()) {
      return false;
    }
    _committedIndexRect = visualRect(previousIndex);
    indicatorXStart     = _committedIndexRect.center().x() - _pIndicatorWidth / 2;
    indicatorXEnd       = _committedIndexRect.center().x();
    indicatorWidthEnd   = 0;
  }
  const QRect newRect = _committedIndexRect;
  const int left      = std::min(previousIndexRect.isValid() ? previousIndexRect.x() : newRect.x(), newRect.x()) - 2;
  const int right =
      std::max(previousIndexRect.isValid() ? previousIndexRect.right() : newRect.right(), newRect.right()) + 2;
  const QRect dirtyRect(left, 0, right - left, viewport()->height());
  _isAnimationFinished = false;
  _startIndicatorAnimation(indicatorXStart, indicatorXEnd, _pIndicatorAnimationWidth, indicatorWidthEnd, dirtyRect);
  return true;
}

void NXPivotView::_startIndicatorAnimation(
    qreal indicatorXStart, qreal indicatorXEnd, int indicatorWidthStart, int indicatorWidthEnd, const QRect &dirtyRect)
{
  QPropertyAnimation *indicatorXAnimation = new QPropertyAnimation(this, "pIndicatorX");
  connect(indicatorXAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    _updateIndicatorRegion(dirtyRect);
  });
  connect(indicatorXAnimation, &QPropertyAnimation::finished, this, [=]() {
    _isAnimationFinished = true;
    _updateIndicatorRegion(dirtyRect);
  });
  indicatorXAnimation->setDuration(300);
  indicatorXAnimation->setEasingCurve(QEasingCurve::InOutSine);
  indicatorXAnimation->setStartValue(indicatorXStart);
  indicatorXAnimation->setEndValue(indicatorXEnd);
  indicatorXAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  QPropertyAnimation *indicatorWidthAnimation = new QPropertyAnimation(this, "pIndicatorAnimationWidth");
  connect(indicatorWidthAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    _updateIndicatorRegion(dirtyRect);
  });
  indicatorWidthAnimation->setDuration(300);
  indicatorWidthAnimation->setEasingCurve(QEasingCurve::InOutSine);
  indicatorWidthAnimation->setStartValue(indicatorWidthStart);
  indicatorWidthAnimation->setEndValue(indicatorWidthEnd);
  indicatorWidthAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXPivotView::setPressIndexAndUpdate(const QModelIndex &index)
{
  if (_pPressIndex == index) {
    return;
  }
  const QModelIndex oldIndex = _pPressIndex;
  _pPressIndex               = index;
  _updateIndexPair(oldIndex, index);
}

void NXPivotView::setCommittedIndexAndUpdate(const QModelIndex &index)
{
  if (_pCommittedIndex == index) {
    return;
  }
  const QModelIndex oldIndex = _pCommittedIndex;
  _pCommittedIndex           = index;
  _updateIndexPair(oldIndex, index);
}

void NXPivotView::refreshHoverState()
{
  const QPoint viewportPos         = viewport()->mapFromGlobal(QCursor::pos());
  const QModelIndex nextHoverIndex = viewport()->rect().contains(viewportPos) ? indexAt(viewportPos) : QModelIndex {};
  _setHoverIndexInternal(nextHoverIndex);
  _syncHoverCursor();
}

void NXPivotView::_updateIndicatorRegion(QRect rect)
{
  auto flags = _pPivotStyle->getIndicatorTypeFlags();
  if (flags & (NXPivotType::IndicatorTop | NXPivotType::IndicatorBottom)) {
    viewport()->update(rect);
  }
}

void NXPivotView::mouseDoubleClickEvent(QMouseEvent *event)
{
  setPressIndexAndUpdate(indexAt(event->pos()));
  QListView::mouseDoubleClickEvent(event);
}

void NXPivotView::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
    setPressIndexAndUpdate(QModelIndex {});
  }
  QListView::mouseReleaseEvent(event);
}

void NXPivotView::mouseMoveEvent(QMouseEvent *event)
{
  QListView::mouseMoveEvent(event);
  _setHoverIndexInternal(indexAt(event->pos()));
  _syncHoverCursor();
}

void NXPivotView::leaveEvent(QEvent *event)
{
  _setHoverIndexInternal(QModelIndex {});
  _syncHoverCursor();
  QListView::leaveEvent(event);
}

void NXPivotView::wheelEvent(QWheelEvent *event)
{
  QListView::wheelEvent(event);
  event->accept();
}

void NXPivotView::paintEvent(QPaintEvent *event)
{
  QListView::paintEvent(event);
  if (_pCommittedIndex.isValid()) {
    NXPivotType::IndicatorTypeFlags flags = _pPivotStyle->getIndicatorTypeFlags();
    if (!(flags & (NXPivotType::IndicatorTop | NXPivotType::IndicatorBottom))) {
      return;
    }
    QWidget *widget    = viewport();
    QRect viewPortRect = widget->rect();

    QPainter painter(widget);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(_pPivotStyle->getIndicatorColor());

    QRect currentIndexRect = visualRect(_pCommittedIndex);
    qreal halfWidth        = _pIndicatorAnimationWidth / 2.0;
    const int layoutShift  = _committedIndexRect.isValid() ? currentIndexRect.x() - _committedIndexRect.x() : 0;
    qreal centerX          = _isAnimationFinished ? currentIndexRect.center().x() : _pIndicatorX + layoutShift + halfWidth;

    QPainterPath path;
    QPolygonF polygon;
    if (flags.testFlag(NXPivotType::IndicatorBottom)) {
      QPointF A(centerX, viewPortRect.bottom() - 4.8);
      path.addPolygon({
          A,                            // 顶部中心
          A + QPointF(2.0, 2.8),        // 右侧内点
          A + QPointF(halfWidth, 5.0),  // 最右侧点
          A + QPointF(2.0, 5.0),        // 右侧底部内点
          A + QPointF(0.0, 2.0),        // 底部中心尖点
          A + QPointF(-2.0, 5.0),       // 左侧底部内点（对称补全）
          A + QPointF(-halfWidth, 5.0), // 最左侧点
          A + QPointF(-2.0, 2.8)        // 左侧内点
      });
    }
    if (flags.testFlag(NXPivotType::IndicatorTop)) {
      QPointF A_Mirror(centerX, viewPortRect.top() + 5.2);
      path.addPolygon({
          A_Mirror,                             // 底部中心（原顶部中心）
          A_Mirror + QPointF(2.0, -2.8),        // 右侧内点（Y取反）
          A_Mirror + QPointF(halfWidth, -5.0),  // 最右侧点（Y取反）
          A_Mirror + QPointF(2.0, -5.0),        // 右侧顶部内点（Y取反）
          A_Mirror + QPointF(0.0, -2.0),        // 顶部中心尖点（Y取反）
          A_Mirror + QPointF(-2.0, -5.0),       // 左侧顶部内点（Y取反，对称）
          A_Mirror + QPointF(-halfWidth, -5.0), // 最左侧点（Y取反，对称）
          A_Mirror + QPointF(-2.0, -2.8)        // 左侧内点（Y取反，对称）
      });
    }
    painter.drawPath(path);
    painter.restore();
  }
}

void NXPivotView::_setHoverIndexInternal(const QModelIndex &index)
{
  if (_pHoverIndex == index) {
    return;
  }
  const QModelIndex oldIndex = _pHoverIndex;
  _pHoverIndex               = index;
  _updateIndexPair(oldIndex, index);
}

void NXPivotView::_updateIndexPair(const QModelIndex &first, const QModelIndex &second)
{
  _updateIndexRect(first);
  if (second != first) {
    _updateIndexRect(second);
  }
}

void NXPivotView::_updateIndexRect(const QModelIndex &index)
{
  if (!index.isValid()) {
    return;
  }
  const QRect itemRect = visualRect(index);
  if (!itemRect.isValid()) {
    return;
  }
  viewport()->update(itemRect.adjusted(-2, -2, 2, 2));
}

void NXPivotView::_syncHoverCursor()
{
  if (_pHoverIndex.isValid()) {
    viewport()->setCursor(_pItemCursor);
  } else {
    viewport()->unsetCursor();
  }
}
