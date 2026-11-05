#include "NXFlowLayout.h"

#include <QPropertyAnimation>
#include <QWidget>

#include "private/NXFlowLayoutPrivate.h"

NXFlowLayout::NXFlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent)
    , d_ptr(new NXFlowLayoutPrivate())
{
  Q_D(NXFlowLayout);
  d->q_ptr     = this;
  d->m_HSpacing = hSpacing;
  d->m_VSpacing = vSpacing;
  setContentsMargins(margin, margin, margin, margin);
}

NXFlowLayout::NXFlowLayout(int margin, int hSpacing, int vSpacing)
    : d_ptr(new NXFlowLayoutPrivate())
{
  Q_D(NXFlowLayout);
  d->q_ptr     = this;
  d->m_HSpacing = hSpacing;
  d->m_VSpacing = vSpacing;
  setContentsMargins(margin, margin, margin, margin);
}

NXFlowLayout::~NXFlowLayout()
{
  QLayoutItem *item;
  while ((item = NXFlowLayout::takeAt(0))) {
    delete item;
  }
}

void NXFlowLayout::addItem(QLayoutItem *item)
{
  Q_D(NXFlowLayout);
  d->m_ItemList.append(item);
}

int NXFlowLayout::horizontalSpacing() const
{
  Q_CONST_D(NXFlowLayout);
  if (d->m_HSpacing >= 0) {
    return d->m_HSpacing;
  } else {
    return d->_smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
  }
}

int NXFlowLayout::verticalSpacing() const
{
  Q_CONST_D(NXFlowLayout);
  if (d->m_VSpacing >= 0) {
    return d->m_VSpacing;
  } else {
    return d->_smartSpacing(QStyle::PM_LayoutVerticalSpacing);
  }
}

int NXFlowLayout::count() const
{
  Q_CONST_D(NXFlowLayout);
  return d->m_ItemList.size();
}

QLayoutItem *NXFlowLayout::itemAt(int index) const
{
  Q_CONST_D(NXFlowLayout);
  return d->m_ItemList.value(index);
}

QLayoutItem *NXFlowLayout::takeAt(int index)
{
  Q_D(NXFlowLayout);
  if (index >= 0 && index < d->m_ItemList.size()) {
    return d->m_ItemList.takeAt(index);
  }
  return nullptr;
}

void NXFlowLayout::setIsAnimation(bool isAnimation)
{
  Q_D(NXFlowLayout);
  d->m_IsAnimation = isAnimation;
}

Qt::Orientations NXFlowLayout::expandingDirections() const
{
  return {};
}

bool NXFlowLayout::hasHeightForWidth() const
{
  return true;
}

int NXFlowLayout::heightForWidth(int width) const
{
  Q_CONST_D(NXFlowLayout);
  return d->_doLayout(QRect(0, 0, width, 0), true);
}

void NXFlowLayout::setGeometry(QRect const &rect)
{
  Q_D(NXFlowLayout);
  QLayout::setGeometry(rect);
  d->_doLayout(rect, false);
}

QSize NXFlowLayout::sizeHint() const
{
  return minimumSize();
}

QSize NXFlowLayout::minimumSize() const
{
  Q_CONST_D(NXFlowLayout);
  QSize size;
  for (QLayoutItem const *item : d->m_ItemList) {
    size = size.expandedTo(item->minimumSize());
  }
  QMargins const margins = contentsMargins();
  size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
  return size;
}
