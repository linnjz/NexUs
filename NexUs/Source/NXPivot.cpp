#include "NXPivot.h"

#include <QCursor>
#include <QPainter>
#include <QScroller>
#include <QVBoxLayout>
#include <QtGlobal>

#include "DeveloperComponents/NXPivotModel.h"
#include "DeveloperComponents/NXPivotStyle.h"
#include "DeveloperComponents/NXPivotView.h"
#include "NXApplication.h"
#include "private/NXPivotPrivate.h"

NXPivot::NXPivot(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXPivotPrivate())
{
  Q_D(NXPivot);
  d->q_ptr = this;
  setFixedHeight(35);
  setObjectName("NXPivot");
  setStyleSheet(QStringLiteral("#NXPivot{background-color:transparent;}"));
  setMouseTracking(true);

  d->m_ListView = new NXPivotView(this);
  d->m_ListView->setMinimumHeight(0);
  d->m_ListView->setFlow(QListView::LeftToRight);
  d->m_ListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->m_ListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->m_ListView->setAutoScroll(false);
  d->m_ListModel = new NXPivotModel(this);
  d->m_ListView->setModel(d->m_ListModel);
  d->m_ListStyle = new NXPivotStyle(d->m_ListView->style());
  d->m_ListView->setStyle(d->m_ListStyle);
  d->m_ListView->setPivotStyle(d->m_ListStyle);

  QFont textFont = this->font();
  textFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
  textFont.setPixelSize(nxApp->getFontPixelSize() + 1);
  d->m_ListView->setFont(textFont);

  QScroller::grabGesture(d->m_ListView->viewport(), QScroller::LeftMouseButtonGesture);
  QScroller *scroller            = QScroller::scroller(d->m_ListView->viewport());
  QScrollerProperties properties = scroller->scrollerProperties();
  properties.setScrollMetric(QScrollerProperties::MousePressEventDelay, 0);
  properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOn);
  properties.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor, 0.35);
  properties.setScrollMetric(QScrollerProperties::OvershootScrollTime, 0.5);
  properties.setScrollMetric(QScrollerProperties::FrameRate, QScrollerProperties::Fps60);
  scroller->setScrollerProperties(properties);

  connect(scroller, &QScroller::stateChanged, this, [=](QScroller::State newstate) {
    if (newstate == QScroller::Pressed) {
      d->m_ListView->setPressIndexAndUpdate(d->m_ListView->indexAt(d->m_ListView->mapFromGlobal(QCursor::pos())));
    } else if (newstate == QScroller::Scrolling || newstate == QScroller::Inactive) {
      d->m_ListView->setPressIndexAndUpdate(QModelIndex());
    }
  });
  connect(d->m_ListView, &NXPivotView::clicked, this, [=](QModelIndex const &index) {
    if (d->m_ListView->doCurrentIndexChangedAnimation(index)) {
      Q_EMIT signalCurrentIndexChanged();
    }
    Q_EMIT pivotClicked(index.row());
  });
  connect(d->m_ListView, &NXPivotView::doubleClicked, this, [=](QModelIndex const &index) {
    Q_EMIT pivotDoubleClicked(index.row());
  });
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(d->m_ListView);
}

NXPivot::~NXPivot()
{
  Q_D(NXPivot);
  delete d->m_ListStyle;
}

void NXPivot::appendPivot(QPixmap const &pixmap)
{
  appendPivot(QString {}, pixmap);
}

void NXPivot::appendPivot(QString const &pivotTitle, QPixmap const &pixmap)
{
  Q_D(NXPivot);
  int const index = d->m_ListModel->getPivotListCount();
  d->m_ListModel->appendPivot(pivotTitle, pixmap);
  setCurrentIndex(index);
  Q_EMIT pivotInserted(index);
}

void NXPivot::insertPivot(int index, QPixmap const &pixmap)
{
  insertPivot(index, QString {}, pixmap);
}

void NXPivot::insertPivot(int index, QString const &pivotTitle, QPixmap const &pixmap)
{
  Q_D(NXPivot);
  int const insertIndex = qBound(0, index, d->m_ListModel->getPivotListCount());
  d->m_ListModel->insertPivot(insertIndex, pivotTitle, pixmap);
  setCurrentIndex(insertIndex);
  Q_EMIT pivotInserted(insertIndex);
}

void NXPivot::removePivot(int index)
{
  Q_D(NXPivot);
  if (index < 0 || index >= d->m_ListModel->getPivotListCount()) [[unlikely]] {
    return;
  }

  d->m_ListModel->removePivot(index);
  int const count = d->m_ListModel->getPivotListCount();

  if (count == 0) {
    d->m_ListView->setCommittedIndexAndUpdate(QModelIndex {});
    Q_EMIT pivotRemoved(index);
    return;
  }

  int nextCurrentIndex = getCurrentIndex();
  if (nextCurrentIndex < 0 || nextCurrentIndex == index) {
    nextCurrentIndex = qMin(index, count - 1);
  } else if (nextCurrentIndex > index) {
    nextCurrentIndex -= 1;
  }
  setCurrentIndex(nextCurrentIndex);

  Q_EMIT pivotRemoved(index);
}

void NXPivot::removePivot(QString const &pivotTitle)
{
  Q_D(NXPivot);
  QList<NXPivotModel::PivotItem> const &items = d->m_ListModel->getPivotList();
  for (int i = items.count() - 1; i >= 0; --i) {
    if (items[i].text == pivotTitle) {
      removePivot(i);
      return;
    }
  }
}

void NXPivot::setPivot(int index, QPixmap const &pixmap)
{
  Q_D(NXPivot);
  if (index < 0 || index >= d->m_ListModel->getPivotListCount()) [[unlikely]] {
    return;
  }
  d->m_ListModel->setPivot(index, pixmap);
  Q_EMIT pivotChanged(index);
}

void NXPivot::setPivot(int index, QString const &pivotTitle, QPixmap const &pixmap)
{
  Q_D(NXPivot);
  if (index < 0 || index >= d->m_ListModel->getPivotListCount()) [[unlikely]] {
    return;
  }
  d->m_ListModel->setPivot(index, pivotTitle, pixmap);
  Q_EMIT pivotChanged(index);
}

void NXPivot::clearPivot()
{
  Q_D(NXPivot);
  if (d->m_ListModel->getPivotListCount() == 0) {
    return;
  }
  d->m_ListModel->clearPivot();
  d->m_ListView->setCommittedIndexAndUpdate(QModelIndex {});
  Q_EMIT signalCurrentIndexChanged();
}

QStringList NXPivot::getPivotTitle() const
{
  Q_CONST_D(NXPivot);
  QList<NXPivotModel::PivotItem> const &items = d->m_ListModel->getPivotList();
  QStringList titles;
  titles.reserve(items.count());
  for (auto const &item : items) {
    titles.append(item.text);
  }
  return titles;
}

void NXPivot::setCurrentTitle(QString const &pivotTitle)
{
  Q_D(NXPivot);
  setCurrentIndex(d->m_ListModel->getPivotIndex(pivotTitle));
}

void NXPivot::setTextPixelSize(int textPixelSize)
{
  Q_D(NXPivot);
  if (textPixelSize <= 0) [[unlikely]] {
    return;
  }
  QFont textFont = this->font();
  textFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
  textFont.setPixelSize(textPixelSize);
  d->m_ListView->setFont(textFont);
  Q_EMIT signalTextPixelSizeChanged();
}

int NXPivot::getTextPixelSize() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListView->font().pixelSize();
}

void NXPivot::setItemCursor(Qt::CursorShape shape)
{
  Q_D(NXPivot);
  d->m_ListView->setItemCursor(shape);
  d->m_ListView->refreshHoverState();
}

Qt::CursorShape NXPivot::getItemCursor() const noexcept(std::is_nothrow_copy_constructible_v<Qt::CursorShape>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListView->getItemCursor();
}

void NXPivot::setCurrentIndex(int currentIndex)
{
  Q_D(NXPivot);
  if (currentIndex < 0 || currentIndex >= d->m_ListModel->getPivotListCount()) [[unlikely]] {
    return;
  }

  QModelIndex const index = d->m_ListModel->index(currentIndex, 0);
  if (d->m_ListView->doCurrentIndexChangedAnimation(index)) {
    Q_EMIT signalCurrentIndexChanged();
  }
}

int NXPivot::getCurrentIndex() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListView->getCommittedIndex().row();
}

void NXPivot::setIndicatorWidth(int markWidth)
{
  Q_D(NXPivot);
  d->m_ListView->setIndicatorWidth(markWidth);
  d->m_ListView->setIndicatorAnimationWidth(markWidth);
  d->m_ListView->viewport()->update();
  Q_EMIT signalIndicatorWidthChanged();
}

int NXPivot::getIndicatorWidth() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListView->getIndicatorWidth();
}

void NXPivot::setTextNormalColor(QColor const &color)
{
  Q_D(NXPivot);
  d->m_ListStyle->setTextNormalColor(color);
  d->m_ListView->viewport()->update();
}

QColor NXPivot::getTextNormalColor() const noexcept(std::is_nothrow_copy_constructible_v<QColor>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getTextNormalColor();
}

void NXPivot::setTextPressedColor(QColor const &color)
{
  Q_D(NXPivot);
  d->m_ListStyle->setTextPressedColor(color);
  d->m_ListView->viewport()->update();
}

QColor NXPivot::getTextPressedColor() const noexcept(std::is_nothrow_copy_constructible_v<QColor>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getTextPressedColor();
}

void NXPivot::setTextFocusColor(QColor const &color)
{
  Q_D(NXPivot);
  d->m_ListStyle->setTextFocusColor(color);
  d->m_ListView->viewport()->update();
}

QColor NXPivot::getTextFocusColor() const noexcept(std::is_nothrow_copy_constructible_v<QColor>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getTextFocusColor();
}

void NXPivot::setIndicatorColor(QColor const &color)
{
  Q_D(NXPivot);
  d->m_ListStyle->setIndicatorColor(color);
  d->m_ListView->viewport()->update();
}

QColor NXPivot::getIndicatorColor() const noexcept(std::is_nothrow_copy_constructible_v<QColor>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getIndicatorColor();
}

void NXPivot::setItemBackgroundColor(QColor const &color)
{
  Q_D(NXPivot);
  d->m_ListStyle->setItemBackgroundColor(color);

  QModelIndex const hoverIndex = d->m_ListView->getHoverIndex();
  if (hoverIndex.isValid()) {
    QRect const hoverRect = d->m_ListView->visualRect(hoverIndex).adjusted(-2, -2, 2, 2);
    if (hoverRect.isValid()) {
      d->m_ListView->viewport()->update(hoverRect);
    }
  }
}

QColor NXPivot::getItemBackgroundColor() const noexcept(std::is_nothrow_copy_constructible_v<QColor>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getItemBackgroundColor();
}

void NXPivot::setItemHorizontalSpacing(int ItemHorizontalSpacing)
{
  Q_D(NXPivot);
  d->m_ListStyle->setItemHorizontalSpacing(ItemHorizontalSpacing);
  d->m_ListView->doItemsLayout();
  Q_EMIT signalItemHorizontalSpacingChanged();
}

int NXPivot::getItemHorizontalSpacing() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getItemHorizontalSpacing();
}

void NXPivot::setItemVerticalSpacing(int ItemVerticalSpacing)
{
  Q_D(NXPivot);
  d->m_ListStyle->setItemVerticalSpacing(ItemVerticalSpacing);
  d->m_ListView->doItemsLayout();
  Q_EMIT signalItemVerticalSpacingChanged();
}

int NXPivot::getItemVerticalSpacing() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getItemVerticalSpacing();
}

void NXPivot::setTextIconSpacing(int textIconSpacing)
{
  Q_D(NXPivot);
  d->m_ListStyle->setTextIconSpacing(textIconSpacing);
  d->m_ListView->viewport()->update();
}

int NXPivot::getTextIconSpacing() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getTextIconSpacing();
}

void NXPivot::setDisplayMode(NXPivotType::DisplayMode DisplayMode)
{
  Q_D(NXPivot);
  d->m_ListStyle->setDisplayMode(DisplayMode);
  d->m_ListView->doItemsLayout();
  d->m_ListView->viewport()->update();
}

NXPivotType::DisplayMode NXPivot::getDisplayMode() const noexcept(std::is_nothrow_copy_constructible_v<NXPivotType::DisplayMode>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getDisplayMode();
}

void NXPivot::setIndicatorTypeFlags(NXPivotType::IndicatorTypeFlags flags)
{
  Q_D(NXPivot);
  d->m_ListStyle->setIndicatorTypeFlags(flags);
  d->m_ListView->viewport()->update();
}

NXPivotType::IndicatorTypeFlags NXPivot::getIndicatorTypeFlags() const noexcept(std::is_nothrow_copy_constructible_v<NXPivotType::IndicatorTypeFlags>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getIndicatorTypeFlags();
}

void NXPivot::setOverlayTextOffsetFromCenterAnchor(QPoint OverlayTextOffsetFromCenterAnchor)
{
  Q_D(NXPivot);
  d->m_ListStyle->setOverlayTextOffsetFromCenterAnchor(OverlayTextOffsetFromCenterAnchor);
  d->m_ListView->viewport()->update();
}

QPoint NXPivot::getOverlayTextOffsetFromCenterAnchor() const noexcept(std::is_nothrow_copy_constructible_v<QPoint>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getOverlayTextOffsetFromCenterAnchor();
}

void NXPivot::setItemFixedSize(QSize pivotFixedSize)
{
  Q_D(NXPivot);
  d->m_ListStyle->setItemFixedSize(pivotFixedSize);
  d->m_ListView->viewport()->update();
}

QSize NXPivot::getItemFixedSize() const noexcept(std::is_nothrow_copy_constructible_v<QSize>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getItemFixedSize();
}

void NXPivot::setIsItemFixedSize(bool isFixedSize)
{
  Q_D(NXPivot);
  d->m_ListStyle->setIsItemFixedSize(isFixedSize);
  d->m_ListView->viewport()->update();
}

bool NXPivot::getIsItemFixedSize() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getIsItemFixedSize();
}

void NXPivot::setIsHoverItemBackgroundEnable(bool enabled)
{
  Q_D(NXPivot);
  d->m_ListStyle->setIsHoverItemBackgroundEnable(enabled);
  d->m_ListView->viewport()->update();
}

bool NXPivot::getIsHoverItemBackgroundEnable() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getIsHoverItemBackgroundEnable();
}

void NXPivot::setIsAutoAdaptiveItemHeight(bool IsAutoAdaptiveItemHeight)
{
  Q_CONST_D(NXPivot);
  d->m_ListStyle->setIsAutoAdaptiveItemHeight(IsAutoAdaptiveItemHeight);
  d->m_ListView->viewport()->update();
}

bool NXPivot::getIsAutoAdaptiveItemHeight() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXPivot);
  return d->m_ListStyle->getIsAutoAdaptiveItemHeight();
}
