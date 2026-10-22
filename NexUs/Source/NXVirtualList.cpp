#include "NXVirtualList.h"

#include <QPainter>
#include <QStandardItemModel>

#include "DeveloperComponents/NXVirtualListStyle.h"
#include "NXScrollBar.h"
#include "NXTheme.h"
#include "private/NXVirtualListPrivate.h"
Q_PROPERTY_CREATE_CPP(NXVirtualList, int, ItemHeight)
Q_PROPERTY_CREATE_CPP(NXVirtualList, bool, IsTransparent)
Q_PROPERTY_CREATE_CPP(NXVirtualList, bool, IsAlternatingRowColors)

NXVirtualList::NXVirtualList(QWidget *parent)
    : QListView(parent)
    , d_ptr(new NXVirtualListPrivate())
{
  Q_D(NXVirtualList);
  d->q_ptr                    = this;
  d->_pItemHeight             = 40;
  d->_pIsTransparent          = true;
  d->_pIsAlternatingRowColors = false;
  d->_themeMode               = nxTheme->getThemeMode();
  setObjectName("NXVirtualList");
  setStyleSheet(QStringLiteral("#NXVirtualList{background-color:transparent;}"));
  setMouseTracking(true);
  setSpacing(0);

  d->_listViewStyle = new NXVirtualListStyle(style());
  d->_listViewStyle->setItemHeight(d->_pItemHeight);
  setStyle(d->_listViewStyle);

  NXScrollBar *vScrollBar = new NXScrollBar(this);
  vScrollBar->setIsAnimation(true);
  setVerticalScrollBar(vScrollBar);
  NXScrollBar *hScrollBar = new NXScrollBar(this);
  setHorizontalScrollBar(hScrollBar);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

  setUniformItemSizes(true);

  setLayoutMode(QListView::Batched);
  setBatchSize(100);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    viewport()->update();
  });
}

NXVirtualList::~NXVirtualList()
{
}

void NXVirtualList::setItemCount(int count)
{
  Q_D(NXVirtualList);
  d->_itemCount = count;
}

int NXVirtualList::getItemCount() const
{
  return d_ptr->_itemCount;
}

void NXVirtualList::paintEvent(QPaintEvent *event)
{
  Q_D(NXVirtualList);
  if (!d->_pIsTransparent) {
    QPainter painter(viewport());
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->_themeMode, DialogBase));
    painter.drawRect(rect());
    painter.restore();
  }
  QListView::paintEvent(event);
}
