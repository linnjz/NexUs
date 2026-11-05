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
    , m_PressIndex {}
    , m_HoverIndex {}
    , m_CommittedIndex {}
    , m_ItemCursor { Qt::PointingHandCursor }
    , m_IndicatorWidth { 40 }
    , m_IndicatorAnimationWidth { 0 }
    , m_IndicatorX { 0 }
{
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

bool NXPivotView::doCurrentIndexChangedAnimation(QModelIndex const &index)
{
  QListView::setCurrentIndex(index);
  QModelIndex const previousIndex = m_CommittedIndex;
  setCommittedIndexAndUpdate(index);
  // 当前项相同且指示器已显示时跳过；初次选中但宽度仍为 0 时仍需播放生长动画
  if (previousIndex == index && previousIndex.isValid() && m_IndicatorAnimationWidth > 0) {
    return false;
  }
  if (!this->isVisible() ||
      !(m_PivotStyle->getIndicatorTypeFlags() & (NXPivotType::IndicatorTop | NXPivotType::IndicatorBottom))) {
    if (index.isValid()) {
      m_CommittedIndexRect = visualRect(index);
    } else {
      m_CommittedIndexRect = QRect {};
    }
    return previousIndex != index;
  }
  QRect const previousIndexRect = m_CommittedIndexRect;
  qreal indicatorXStart         = 0;
  qreal indicatorXEnd           = 0;
  int indicatorWidthEnd         = 0;
  if (index.isValid()) {
    m_CommittedIndexRect = visualRect(index);
    indicatorXEnd       = m_CommittedIndexRect.center().x() - m_IndicatorWidth / 2;
    if (previousIndex.isValid()) {
      // 以快照矩形为基准补偿布局位移；静止时位移为 0，不会叠加目标位移
      int const layoutShift =
          previousIndexRect.isValid() ? visualRect(previousIndex).center().x() - previousIndexRect.center().x() : 0;
      indicatorXStart = m_IndicatorX + layoutShift;
    } else {
      indicatorXStart = m_CommittedIndexRect.center().x();
    }
    indicatorWidthEnd = m_IndicatorWidth;
  } else {
    if (!previousIndex.isValid()) {
      return false;
    }
    m_CommittedIndexRect = visualRect(previousIndex);
    indicatorXStart     = m_CommittedIndexRect.center().x() - m_IndicatorWidth / 2;
    indicatorXEnd       = m_CommittedIndexRect.center().x();
    indicatorWidthEnd   = 0;
  }
  QRect const newRect = m_CommittedIndexRect;
  int const left      = std::min(previousIndexRect.isValid() ? previousIndexRect.x() : newRect.x(), newRect.x()) - 2;
  int const right =
      std::max(previousIndexRect.isValid() ? previousIndexRect.right() : newRect.right(), newRect.right()) + 2;
  QRect const dirtyRect(left, 0, right - left, viewport()->height());
  m_IsAnimationFinished = false;
  _startIndicatorAnimation(indicatorXStart, indicatorXEnd, m_IndicatorAnimationWidth, indicatorWidthEnd, dirtyRect);
  return true;
}

void NXPivotView::_startIndicatorAnimation(
    qreal indicatorXStart, qreal indicatorXEnd, int indicatorWidthStart, int indicatorWidthEnd, QRect const &dirtyRect)
{
  QPropertyAnimation *indicatorXAnimation = new QPropertyAnimation(this, "IndicatorX");
  connect(indicatorXAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    _updateIndicatorRegion(dirtyRect);
  });
  connect(indicatorXAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_IsAnimationFinished = true;
    _updateIndicatorRegion(dirtyRect);
  });
  indicatorXAnimation->setDuration(300);
  indicatorXAnimation->setEasingCurve(QEasingCurve::InOutSine);
  indicatorXAnimation->setStartValue(indicatorXStart);
  indicatorXAnimation->setEndValue(indicatorXEnd);
  indicatorXAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  QPropertyAnimation *indicatorWidthAnimation = new QPropertyAnimation(this, "IndicatorAnimationWidth");
  connect(indicatorWidthAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    _updateIndicatorRegion(dirtyRect);
  });
  indicatorWidthAnimation->setDuration(300);
  indicatorWidthAnimation->setEasingCurve(QEasingCurve::InOutSine);
  indicatorWidthAnimation->setStartValue(indicatorWidthStart);
  indicatorWidthAnimation->setEndValue(indicatorWidthEnd);
  indicatorWidthAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXPivotView::setPressIndexAndUpdate(QModelIndex const &index)
{
  if (m_PressIndex == index) {
    return;
  }
  QModelIndex const oldIndex = m_PressIndex;
  m_PressIndex               = index;
  _updateIndexPair(oldIndex, index);
}

void NXPivotView::setCommittedIndexAndUpdate(QModelIndex const &index)
{
  if (m_CommittedIndex == index) {
    return;
  }
  QModelIndex const oldIndex = m_CommittedIndex;
  m_CommittedIndex           = index;
  _updateIndexPair(oldIndex, index);
}

void NXPivotView::refreshHoverState()
{
  QPoint const viewportPos         = viewport()->mapFromGlobal(QCursor::pos());
  QModelIndex const nextHoverIndex = viewport()->rect().contains(viewportPos) ? indexAt(viewportPos) : QModelIndex {};
  _setHoverIndexInternal(nextHoverIndex);
  _syncHoverCursor();
}

void NXPivotView::_updateIndicatorRegion(QRect rect)
{
  auto flags = m_PivotStyle->getIndicatorTypeFlags();
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
  if (m_CommittedIndex.isValid()) {
    NXPivotType::IndicatorTypeFlags flags = m_PivotStyle->getIndicatorTypeFlags();
    if (!(flags & (NXPivotType::IndicatorTop | NXPivotType::IndicatorBottom))) {
      return;
    }
    QWidget *widget    = viewport();
    QRect viewPortRect = widget->rect();

    QPainter painter(widget);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_PivotStyle->getIndicatorColor());

    QRect currentIndexRect = visualRect(m_CommittedIndex);
    qreal halfWidth        = m_IndicatorAnimationWidth / 2.0;
    int const layoutShift  = m_CommittedIndexRect.isValid() ? currentIndexRect.x() - m_CommittedIndexRect.x() : 0;
    qreal centerX          = m_IsAnimationFinished ? currentIndexRect.center().x() : m_IndicatorX + layoutShift + halfWidth;

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

void NXPivotView::_setHoverIndexInternal(QModelIndex const &index)
{
  if (m_HoverIndex == index) {
    return;
  }
  QModelIndex const oldIndex = m_HoverIndex;
  m_HoverIndex               = index;
  _updateIndexPair(oldIndex, index);
}

void NXPivotView::_updateIndexPair(QModelIndex const &first, QModelIndex const &second)
{
  _updateIndexRect(first);
  if (second != first) {
    _updateIndexRect(second);
  }
}

void NXPivotView::_updateIndexRect(QModelIndex const &index)
{
  if (!index.isValid()) {
    return;
  }
  QRect const itemRect = visualRect(index);
  if (!itemRect.isValid()) {
    return;
  }
  viewport()->update(itemRect.adjusted(-2, -2, 2, 2));
}

void NXPivotView::_syncHoverCursor()
{
  if (m_HoverIndex.isValid()) {
    viewport()->setCursor(m_ItemCursor);
  } else {
    viewport()->unsetCursor();
  }
}
