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

  d->_listView = new NXPivotView(this);
  d->_listView->setMinimumHeight(0);
  d->_listView->setFlow(QListView::LeftToRight);
  d->_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->_listView->setAutoScroll(false);
  d->_listModel = new NXPivotModel(this);
  d->_listView->setModel(d->_listModel);
  d->_listStyle = new NXPivotStyle(d->_listView->style());
  d->_listView->setStyle(d->_listStyle);
  d->_listView->setPivotStyle(d->_listStyle);

  QFont textFont = this->font();
  textFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
  textFont.setPixelSize(nxApp->getFontPixelSize() + 1);
  d->_listView->setFont(textFont);

  QScroller::grabGesture(d->_listView->viewport(), QScroller::LeftMouseButtonGesture);
  QScroller *scroller            = QScroller::scroller(d->_listView->viewport());
  QScrollerProperties properties = scroller->scrollerProperties();
  properties.setScrollMetric(QScrollerProperties::MousePressEventDelay, 0);
  properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOn);
  properties.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor, 0.35);
  properties.setScrollMetric(QScrollerProperties::OvershootScrollTime, 0.5);
  properties.setScrollMetric(QScrollerProperties::FrameRate, QScrollerProperties::Fps60);
  scroller->setScrollerProperties(properties);

  connect(scroller, &QScroller::stateChanged, this, [=](QScroller::State newstate) {
    if (newstate == QScroller::Pressed) {
      d->_listView->setPressIndexAndUpdate(d->_listView->indexAt(d->_listView->mapFromGlobal(QCursor::pos())));
    } else if (newstate == QScroller::Scrolling || newstate == QScroller::Inactive) {
      d->_listView->setPressIndexAndUpdate(QModelIndex());
    }
  });
  connect(d->_listView, &NXPivotView::clicked, this, [=](const QModelIndex &index) {
    if (d->_listView->doCurrentIndexChangedAnimation(index)) {
      Q_EMIT pCurrentIndexChanged();
    }
    Q_EMIT pivotClicked(index.row());
  });
  connect(d->_listView, &NXPivotView::doubleClicked, this, [=](const QModelIndex &index) {
    Q_EMIT pivotDoubleClicked(index.row());
  });
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(d->_listView);
}

NXPivot::~NXPivot()
{
  Q_D(NXPivot);
  delete d->_listStyle;
}

void NXPivot::appendPivot(const QPixmap &pixmap)
{
  appendPivot(QString {}, pixmap);
}

void NXPivot::appendPivot(const QString &pivotTitle, const QPixmap &pixmap)
{
  Q_D(NXPivot);
  const int index = d->_listModel->getPivotListCount();
  d->_listModel->appendPivot(pivotTitle, pixmap);
  setCurrentIndex(index);
  Q_EMIT pivotInserted(index);
}

void NXPivot::insertPivot(int index, const QPixmap &pixmap)
{
  insertPivot(index, QString {}, pixmap);
}

void NXPivot::insertPivot(int index, const QString &pivotTitle, const QPixmap &pixmap)
{
  Q_D(NXPivot);
  const int insertIndex = qBound(0, index, d->_listModel->getPivotListCount());
  d->_listModel->insertPivot(insertIndex, pivotTitle, pixmap);
  setCurrentIndex(insertIndex);
  Q_EMIT pivotInserted(insertIndex);
}

void NXPivot::removePivot(int index)
{
  Q_D(NXPivot);
  if (index < 0 || index >= d->_listModel->getPivotListCount()) [[unlikely]] {
    return;
  }

  d->_listModel->removePivot(index);
  const int count = d->_listModel->getPivotListCount();

  if (count == 0) {
    d->_listView->setCommittedIndexAndUpdate(QModelIndex {});
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

void NXPivot::removePivot(const QString &pivotTitle)
{
  Q_D(NXPivot);
  const QList<NXPivotModel::PivotItem> &items = d->_listModel->getPivotList();
  for (int i = items.count() - 1; i >= 0; --i) {
    if (items[i].text == pivotTitle) {
      removePivot(i);
      return;
    }
  }
}

void NXPivot::setPivot(int index, const QPixmap &pixmap)
{
  Q_D(NXPivot);
  if (index < 0 || index >= d->_listModel->getPivotListCount()) [[unlikely]] {
    return;
  }
  d->_listModel->setPivot(index, pixmap);
  Q_EMIT pivotChanged(index);
}

void NXPivot::setPivot(int index, const QString &pivotTitle, const QPixmap &pixmap)
{
  Q_D(NXPivot);
  if (index < 0 || index >= d->_listModel->getPivotListCount()) [[unlikely]] {
    return;
  }
  d->_listModel->setPivot(index, pivotTitle, pixmap);
  Q_EMIT pivotChanged(index);
}

void NXPivot::clearPivot()
{
  Q_D(NXPivot);
  if (d->_listModel->getPivotListCount() == 0) {
    return;
  }
  d->_listModel->clearPivot();
  d->_listView->setCommittedIndexAndUpdate(QModelIndex {});
  Q_EMIT pCurrentIndexChanged();
}

QStringList NXPivot::getPivotTitle() const
{
  Q_D(const NXPivot);
  const QList<NXPivotModel::PivotItem> &items = d->_listModel->getPivotList();
  QStringList titles;
  titles.reserve(items.count());
  for (const auto &item : items) {
    titles.append(item.text);
  }
  return titles;
}

void NXPivot::setCurrentTitle(const QString &pivotTitle)
{
  Q_D(NXPivot);
  setCurrentIndex(d->_listModel->getPivotIndex(pivotTitle));
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
  d->_listView->setFont(textFont);
  Q_EMIT pTextPixelSizeChanged();
}

int NXPivot::getTextPixelSize() const
{
  Q_D(const NXPivot);
  return d->_listView->font().pixelSize();
}

void NXPivot::setItemCursor(Qt::CursorShape shape)
{
  Q_D(NXPivot);
  d->_listView->setItemCursor(shape);
  d->_listView->refreshHoverState();
}

Qt::CursorShape NXPivot::getItemCursor() const
{
  Q_D(const NXPivot);
  return d->_listView->getItemCursor();
}

void NXPivot::setCurrentIndex(int currentIndex)
{
  Q_D(NXPivot);
  if (currentIndex < 0 || currentIndex >= d->_listModel->getPivotListCount()) [[unlikely]] {
    return;
  }

  const QModelIndex index = d->_listModel->index(currentIndex, 0);
  if (d->_listView->doCurrentIndexChangedAnimation(index)) {
    Q_EMIT pCurrentIndexChanged();
  }
}

int NXPivot::getCurrentIndex() const
{
  Q_D(const NXPivot);
  return d->_listView->getCommittedIndex().row();
}

void NXPivot::setIndicatorWidth(int markWidth)
{
  Q_D(NXPivot);
  d->_listView->setIndicatorWidth(markWidth);
  d->_listView->setIndicatorAnimationWidth(markWidth);
  d->_listView->viewport()->update();
  Q_EMIT pIndicatorWidthChanged();
}

int NXPivot::getIndicatorWidth() const
{
  Q_D(const NXPivot);
  return d->_listView->getIndicatorWidth();
}

void NXPivot::setTextNormalColor(const QColor &color)
{
  Q_D(NXPivot);
  d->_listStyle->setTextNormalColor(color);
  d->_listView->viewport()->update();
}

QColor NXPivot::getTextNormalColor() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getTextNormalColor();
}

void NXPivot::setTextPressedColor(const QColor &color)
{
  Q_D(NXPivot);
  d->_listStyle->setTextPressedColor(color);
  d->_listView->viewport()->update();
}

QColor NXPivot::getTextPressedColor() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getTextPressedColor();
}

void NXPivot::setTextFocusColor(const QColor &color)
{
  Q_D(NXPivot);
  d->_listStyle->setTextFocusColor(color);
  d->_listView->viewport()->update();
}

QColor NXPivot::getTextFocusColor() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getTextFocusColor();
}

void NXPivot::setIndicatorColor(const QColor &color)
{
  Q_D(NXPivot);
  d->_listStyle->setIndicatorColor(color);
  d->_listView->viewport()->update();
}

QColor NXPivot::getIndicatorColor() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getIndicatorColor();
}

void NXPivot::setItemBackgroundColor(const QColor &color)
{
  Q_D(NXPivot);
  d->_listStyle->setItemBackgroundColor(color);

  const QModelIndex hoverIndex = d->_listView->getHoverIndex();
  if (hoverIndex.isValid()) {
    const QRect hoverRect = d->_listView->visualRect(hoverIndex).adjusted(-2, -2, 2, 2);
    if (hoverRect.isValid()) {
      d->_listView->viewport()->update(hoverRect);
    }
  }
}

QColor NXPivot::getItemBackgroundColor() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getItemBackgroundColor();
}

void NXPivot::setItemHorizontalSpacing(int ItemHorizontalSpacing)
{
  Q_D(NXPivot);
  d->_listStyle->setItemHorizontalSpacing(ItemHorizontalSpacing);
  d->_listView->doItemsLayout();
  Q_EMIT pItemHorizontalSpacingChanged();
}

int NXPivot::getItemHorizontalSpacing() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getItemHorizontalSpacing();
}

void NXPivot::setItemVerticalSpacing(int ItemVerticalSpacing)
{
  Q_D(NXPivot);
  d->_listStyle->setItemVerticalSpacing(ItemVerticalSpacing);
  d->_listView->doItemsLayout();
  Q_EMIT pItemVerticalSpacingChanged();
}

int NXPivot::getItemVerticalSpacing() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getItemVerticalSpacing();
}

void NXPivot::setTextIconSpacing(int textIconSpacing)
{
  Q_D(NXPivot);
  d->_listStyle->setTextIconSpacing(textIconSpacing);
  d->_listView->viewport()->update();
}

int NXPivot::getTextIconSpacing() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getTextIconSpacing();
}

void NXPivot::setDisplayMode(NXPivotType::DisplayMode DisplayMode)
{
  Q_D(NXPivot);
  d->_listStyle->setDisplayMode(DisplayMode);
  d->_listView->doItemsLayout();
  d->_listView->viewport()->update();
}

NXPivotType::DisplayMode NXPivot::getDisplayMode() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getDisplayMode();
}

void NXPivot::setIndicatorTypeFlags(NXPivotType::IndicatorTypeFlags flags)
{
  Q_D(NXPivot);
  d->_listStyle->setIndicatorTypeFlags(flags);
  d->_listView->viewport()->update();
}

NXPivotType::IndicatorTypeFlags NXPivot::getIndicatorTypeFlags() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getIndicatorTypeFlags();
}

void NXPivot::setOverlayTextOffsetFromCenterAnchor(QPoint OverlayTextOffsetFromCenterAnchor)
{
  Q_D(NXPivot);
  d->_listStyle->setOverlayTextOffsetFromCenterAnchor(OverlayTextOffsetFromCenterAnchor);
  d->_listView->viewport()->update();
}

QPoint NXPivot::getOverlayTextOffsetFromCenterAnchor() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getOverlayTextOffsetFromCenterAnchor();
}

void NXPivot::setItemFixedSize(QSize pivotFixedSize)
{
  Q_D(NXPivot);
  d->_listStyle->setItemFixedSize(pivotFixedSize);
  d->_listView->viewport()->update();
}

QSize NXPivot::getItemFixedSize() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getItemFixedSize();
}

void NXPivot::setIsItemFixedSize(bool isFixedSize)
{
  Q_D(NXPivot);
  d->_listStyle->setIsItemFixedSize(isFixedSize);
  d->_listView->viewport()->update();
}

bool NXPivot::getIsItemFixedSize() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getIsItemFixedSize();
}

void NXPivot::setIsHoverItemBackgroundEnable(bool enabled)
{
  Q_D(NXPivot);
  d->_listStyle->setIsHoverItemBackgroundEnable(enabled);
  d->_listView->viewport()->update();
}

bool NXPivot::getIsHoverItemBackgroundEnable() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getIsHoverItemBackgroundEnable();
}

void NXPivot::setIsAutoAdaptiveItemHeight(bool IsAutoAdaptiveItemHeight)
{
  Q_D(const NXPivot);
  d->_listStyle->setIsAutoAdaptiveItemHeight(IsAutoAdaptiveItemHeight);
  d->_listView->viewport()->update();
}

bool NXPivot::getIsAutoAdaptiveItemHeight() const
{
  Q_D(const NXPivot);
  return d->_listStyle->getIsAutoAdaptiveItemHeight();
}
