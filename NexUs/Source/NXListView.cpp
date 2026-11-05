#include "NXListView.h"

#include <QMouseEvent>

#include "DeveloperComponents/NXListViewStyle.h"
#include "NXScrollBar.h"
#include "private/NXListViewPrivate.h"

NXListView::NXListView(QWidget *parent)
    : QListView(parent)
    , d_ptr(new NXListViewPrivate())
{
  Q_D(NXListView);
  d->q_ptr = this;
  setObjectName("NXListView");
  setStyleSheet(QStringLiteral("#NXListView{background-color:transparent;}"));
  d->m_ListViewStyle = new NXListViewStyle(style());
  setStyle(d->m_ListViewStyle);
  setMouseTracking(true);
  setVerticalScrollBar(new NXScrollBar(this));
  setHorizontalScrollBar(new NXScrollBar(this));
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

NXListView::~NXListView()
{
  Q_D(NXListView);
  delete d->m_ListViewStyle;
}

void NXListView::setItemHeight(int itemHeight)
{
  Q_D(NXListView);
  if (itemHeight > 0) {
    d->m_ListViewStyle->setItemHeight(itemHeight);
    doItemsLayout();
  }
}

int NXListView::getItemHeight() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXListView);
  return d->m_ListViewStyle->getItemHeight();
}

void NXListView::setIsTransparent(bool isTransparent)
{
  Q_D(NXListView);
  d->m_ListViewStyle->setIsTransparent(isTransparent);
  update();
}

bool NXListView::getIsTransparent() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXListView);
  return d->m_ListViewStyle->getIsTransparent();
}
