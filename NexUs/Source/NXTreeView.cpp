#include "NXTreeView.h"

#include "DeveloperComponents/NXTreeViewStyle.h"
#include "NXScrollBar.h"
#include "private/NXTreeViewPrivate.h"

NXTreeView::NXTreeView(QWidget *parent)
    : QTreeView(parent)
    , d_ptr(new NXTreeViewPrivate())
{
  Q_D(NXTreeView);
  d->q_ptr = this;
  setObjectName("NXTreeView");
  setStyleSheet(QStringLiteral("#NXTreeView{background-color:transparent;}"
                               "QHeaderView{background-color:transparent;border:0px;}"));

  setAnimated(true);
  setMouseTracking(true);

  NXScrollBar *hScrollBar = new NXScrollBar(this);
  hScrollBar->setIsAnimation(true);
  connect(hScrollBar, &NXScrollBar::rangeAnimationFinished, this, [=]() {
    doItemsLayout();
  });
  setHorizontalScrollBar(hScrollBar);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  NXScrollBar *vScrollBar = new NXScrollBar(this);
  vScrollBar->setIsAnimation(true);
  connect(vScrollBar, &NXScrollBar::rangeAnimationFinished, this, [=]() {
    doItemsLayout();
  });
  setVerticalScrollBar(vScrollBar);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  d->m_TreeViewStyle = new NXTreeViewStyle(style());
  setStyle(d->m_TreeViewStyle);
}

NXTreeView::~NXTreeView()
{
  Q_D(NXTreeView);
  delete d->m_TreeViewStyle;
}

void NXTreeView::setNXIcon(NXIconType::IconName icon)
{
  Q_D(NXTreeView);
  d->m_TreeViewStyle->setIconName(icon);
}

void NXTreeView::setItemHeight(int itemHeight)
{
  Q_D(NXTreeView);
  if (itemHeight > 0) {
    d->m_TreeViewStyle->setItemHeight(itemHeight);
    doItemsLayout();
  }
}

int NXTreeView::getItemHeight() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXTreeView);
  return d->m_TreeViewStyle->getItemHeight();
}

void NXTreeView::setHeaderMargin(int headerMargin)
{
  Q_D(NXTreeView);
  if (headerMargin >= 0) {
    d->m_TreeViewStyle->setHeaderMargin(headerMargin);
    doItemsLayout();
  }
}

int NXTreeView::getHeaderMargin() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXTreeView);
  return d->m_TreeViewStyle->getHeaderMargin();
}
