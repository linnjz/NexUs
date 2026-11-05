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
  d->m_TableViewStyle = new NXTableViewStyle(style());
  setStyle(d->m_TableViewStyle);
}

NXTableView::~NXTableView()
{
  Q_D(NXTableView);
  delete d->m_TableViewStyle;
}

void NXTableView::setBorderRadius(int radius)
{
  Q_D(NXTableView);
  d->m_TableViewStyle->setBorderRadius(radius);
  update();
}

int NXTableView::getBorderRadius() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXTableView);
  return d->m_TableViewStyle->getBorderRadius();
}

void NXTableView::setCheckIndicatorWidth(int width)
{
  Q_D(NXTableView);
  d->m_TableViewStyle->setCheckIndicatorWidth(width);
  update();
}

int NXTableView::getCheckIndicatorWidth() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXTableView);
  return d->m_TableViewStyle->getCheckIndicatorWidth();
}

void NXTableView::setHeaderMargin(int headerMargin)
{
  Q_D(NXTableView);
  if (headerMargin >= 0) {
    d->m_TableViewStyle->setHeaderMargin(headerMargin);
    doItemsLayout();
  }
}

int NXTableView::getHeaderMargin() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXTableView);
  return d->m_TableViewStyle->getHeaderMargin();
}

void NXTableView::setIsHoverRowEffectEnable(bool enable)
{
  Q_D(NXTableView);
  d->m_TableViewStyle->setIsHoverRowEffectEnable(enable);
  update();
}

bool NXTableView::getIsHoverRowEffectEnable() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXTableView);
  return d->m_TableViewStyle->getIsHoverRowEffectEnable();
}

void NXTableView::setDefaultPadding(int padding)
{
  Q_D(NXTableView);
  d->m_TableViewStyle->setDefaultPadding(padding);
  update();
}

int NXTableView::getDefaultPadding() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXTableView);
  return d->m_TableViewStyle->getDefaultPadding();
}

void NXTableView::setColumnPadding(int column, int padding)
{
  Q_D(NXTableView);
  d->m_TableViewStyle->setColumnPadding(column, padding);
  update();
}

int NXTableView::getColumnPadding(int column) const
{
  Q_CONST_D(NXTableView);
  return d->m_TableViewStyle->getColumnPadding(column);
}

void NXTableView::clearColumnPadding(int column)
{
  Q_D(NXTableView);
  d->m_TableViewStyle->clearColumnPadding(column);
  update();
}

QRect NXTableView::headerCheckIndicatorRect(int section) const
{
  Q_CONST_D(NXTableView);
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

  int padding = d->m_TableViewStyle->getColumnPadding(section);
  int cw      = d->m_TableViewStyle->getCheckIndicatorWidth();

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
  Q_CONST_D(NXTableView);
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

  QRect checkRect = d->m_TableViewStyle->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &option, this);
  if (checkRect.isNull())
    return QRect();

  int padding = d->m_TableViewStyle->getColumnPadding(column);

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
    QModelIndex const &currentIndex  = indexAt(event->pos());
    QModelIndex const &oldHoverIndex = d->m_TableViewStyle->getCurrentHoverIndex();
    if (currentIndex.isValid() && currentIndex.row() != oldHoverIndex.row()) {
      QRect rowRect = visualRect(currentIndex);
      rowRect.setX(0);
      rowRect.setWidth(viewport()->width());
      update(rowRect);
    }
    d->m_TableViewStyle->setCurrentHoverIndex(currentIndex);
    Q_EMIT hoverIndexChanged(currentIndex);
  }
  QTableView::mouseMoveEvent(event);
}

void NXTableView::leaveEvent(QEvent *event)
{
  Q_D(NXTableView);
  if (selectionBehavior() == QAbstractItemView::SelectRows) {
    d->m_TableViewStyle->setCurrentHoverIndex(QModelIndex {});
    update();
    Q_EMIT hoverIndexChanged(QModelIndex {});
  }
  QTableView::leaveEvent(event);
}
