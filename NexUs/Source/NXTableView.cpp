#include "NXTableView.h"

#include <QHeaderView>
#include <QMouseEvent>

#include "DeveloperComponents/NXTableViewStyle.h"
#include "NXScrollBar.h"
#include "private/NXTableViewPrivate.h"

NXTableView::NXTableView(QWidget *parent)
    : QTableView(parent)
    , d_ptr(new NXTableViewPrivate())
{
  Q_D(NXTableView);
  d->q_ptr = this;
  setMouseTracking(true);
  setObjectName("NXTableView");
  setStyleSheet(QStringLiteral("QTableView{background-color:transparent;}"
                               "QHeaderView{background-color:transparent;border:0px;}"));
  setShowGrid(false);
  setVerticalScrollBar(new NXScrollBar(this));
  setHorizontalScrollBar(new NXScrollBar(this));
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  d->_tableViewStyle = new NXTableViewStyle(style());
  setStyle(d->_tableViewStyle);
}

NXTableView::~NXTableView()
{
  Q_D(NXTableView);
  delete d->_tableViewStyle;
}

void NXTableView::setBorderRadius(int radius)
{
  Q_D(NXTableView);
  d->_tableViewStyle->setBorderRadius(radius);
  update();
}

int NXTableView::getBorderRadius() const
{
  Q_D(const NXTableView);
  return d->_tableViewStyle->getBorderRadius();
}

void NXTableView::setCheckIndicatorWidth(int width)
{
  Q_D(NXTableView);
  d->_tableViewStyle->setCheckIndicatorWidth(width);
  update();
}

int NXTableView::getCheckIndicatorWidth() const
{
  Q_D(const NXTableView);
  return d->_tableViewStyle->getCheckIndicatorWidth();
}

void NXTableView::setHeaderMargin(int headerMargin)
{
  Q_D(NXTableView);
  if (headerMargin >= 0) {
    d->_tableViewStyle->setHeaderMargin(headerMargin);
    doItemsLayout();
  }
}

int NXTableView::getHeaderMargin() const
{
  Q_D(const NXTableView);
  return d->_tableViewStyle->getHeaderMargin();
}

void NXTableView::setIsHoverRowEffectEnable(bool enable)
{
  Q_D(NXTableView);
  d->_tableViewStyle->setIsHoverRowEffectEnable(enable);
  update();
}

bool NXTableView::getIsHoverRowEffectEnable() const
{
  Q_D(const NXTableView);
  return d->_tableViewStyle->getIsHoverRowEffectEnable();
}

void NXTableView::setDefaultPadding(int padding)
{
  Q_D(NXTableView);
  d->_tableViewStyle->setDefaultPadding(padding);
  update();
}

int NXTableView::getDefaultPadding() const
{
  Q_D(const NXTableView);
  return d->_tableViewStyle->getDefaultPadding();
}

void NXTableView::setColumnPadding(int column, int padding)
{
  Q_D(NXTableView);
  d->_tableViewStyle->setColumnPadding(column, padding);
  update();
}

int NXTableView::getColumnPadding(int column) const
{
  Q_D(const NXTableView);
  return d->_tableViewStyle->getColumnPadding(column);
}

void NXTableView::clearColumnPadding(int column)
{
  Q_D(NXTableView);
  d->_tableViewStyle->clearColumnPadding(column);
  update();
}

QRect NXTableView::headerCheckIndicatorRect(int section) const
{
  Q_D(const NXTableView);
  if (!model()) {
    qWarning() << Q_FUNC_INFO << ": { model is nullptr }";
    return QRect {};
  }
  if (!model()->headerData(section, Qt::Horizontal, Qt::CheckStateRole).isValid()) {
    qWarning()
        << Q_FUNC_INFO
        << ": { model headerData() function did not set orientation<Qt::Horizontal> and role<Qt::CheckStateRole> up}";
    return QRect {};
  }
  QHeaderView *header = horizontalHeader();
  if (!header) {
    qWarning() << Q_FUNC_INFO << ": { horizontalHeader is nullptr }";
    return QRect {};
  }
  if (section < 0 || section >= header->count()) [[unlikely]] {
    qWarning() << Q_FUNC_INFO << ": { section is out of range }";
    return QRect {};
  }

  int padding = d->_tableViewStyle->getColumnPadding(section);
  int cw      = d->_tableViewStyle->getCheckIndicatorWidth();

  int x = header->sectionViewportPosition(section);
  int w = header->sectionSize(section);
  int h = header->height();
  QRect sectionRect(x, 0, w, h); // 相对于表头

  // 按绘制逻辑计算复选框矩形（与 NXTableViewStyle::drawControl 中 CE_HeaderSection 一致）
  QRect checkRect(sectionRect.left() + padding + 3, sectionRect.center().y() - cw / 2, cw, cw);

  QPoint offset = header->mapTo(this, QPoint(0, 0));
  checkRect.translate(offset);
  return checkRect;
}

QRect NXTableView::checkIndicatorRect(int row, int column) const
{
  Q_D(const NXTableView);
  if (!model()) {
    qWarning() << Q_FUNC_INFO << ": { model is nullptr }";
    return QRect {};
  }
  if (row < 0 || column < 0 || row >= model()->rowCount() || column >= model()->columnCount()) [[unlikely]] {
    qWarning() << Q_FUNC_INFO << ": { row or column is out of range }";
    return QRect {};
  }

  QModelIndex index = model()->index(row, column);
  if (!index.isValid()) [[unlikely]] {
    return QRect {};
  }

  QVariant checkStateData = model()->data(index, Qt::CheckStateRole);
  if (!checkStateData.isValid()) [[unlikely]] {
    qWarning() << Q_FUNC_INFO << ": { model data() function did not set Qt::CheckStateRole up }";
    return QRect {};
  }

  QRect cellRect = visualRect(index);
  if (!cellRect.isValid()) [[unlikely]] {
    return QRect {};
  }

  QStyleOptionViewItem option;
  option.initFrom(this);
  option.rect       = cellRect;
  option.index      = index;
  option.features   = QStyleOptionViewItem::HasCheckIndicator;
  option.checkState = checkStateData.value<Qt::CheckState>();

  QRect checkRect = d->_tableViewStyle->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &option, this);
  if (checkRect.isNull())
    return QRect();

  int padding = d->_tableViewStyle->getColumnPadding(column);

  // 应用水平内边距（与 NXTableViewStyle::drawControl 中 CE_ItemViewItem 分支一致）
  checkRect.adjust(padding, 0, padding, 0);

  QPoint offset = viewport()->mapTo(this, QPoint(0, 0));
  checkRect.translate(offset);
  return checkRect;
}

void NXTableView::showEvent(QShowEvent *event)
{
  Q_EMIT tableViewShow();
  QTableView::showEvent(event);
}

void NXTableView::hideEvent(QHideEvent *event)
{
  Q_EMIT tableViewHide();
  QTableView::hideEvent(event);
}

void NXTableView::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXTableView);
  if (selectionBehavior() == QAbstractItemView::SelectRows) {
    const QModelIndex &currentIndex  = indexAt(event->pos());
    const QModelIndex &oldHoverIndex = d->_tableViewStyle->getCurrentHoverIndex();
    if (currentIndex.isValid() && currentIndex.row() != oldHoverIndex.row()) {
      QRect rowRect = visualRect(currentIndex);
      rowRect.setX(0);
      rowRect.setWidth(viewport()->width());
      update(rowRect);
    }
    d->_tableViewStyle->setCurrentHoverIndex(currentIndex);
    Q_EMIT hoverIndexChanged(currentIndex);
  }
  QTableView::mouseMoveEvent(event);
}

void NXTableView::leaveEvent(QEvent *event)
{
  Q_D(NXTableView);
  if (selectionBehavior() == QAbstractItemView::SelectRows) {
    d->_tableViewStyle->setCurrentHoverIndex(QModelIndex {});
    update();
    Q_EMIT hoverIndexChanged(QModelIndex {});
  }
  QTableView::leaveEvent(event);
}
