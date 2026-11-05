#include "NXVirtualList.h"

#include <QPainter>
#include <QStandardItemModel>

#include "DeveloperComponents/NXVirtualListStyle.h"
#include "NXScrollBar.h"
#include "NXTheme.h"
#include "private/NXVirtualListPrivate.h"
Q_PROPERTY_CREATE_CPP(NXVirtualList, QPS_PAEMIT(int), ItemHeight)
Q_PROPERTY_CREATE_CPP(NXVirtualList, QPS_PAEMIT(bool), IsTransparent)
Q_PROPERTY_CREATE_CPP(NXVirtualList, QPS_PAEMIT(bool), IsAlternatingRowColors)

NXVirtualList::NXVirtualList(QWidget *parent)
    : QListView(parent)
    , d_ptr(new NXVirtualListPrivate())
{
  Q_D(NXVirtualList);
  d->q_ptr                    = this;
  d->m_ItemHeight             = 40;
  d->m_IsTransparent          = true;
  d->m_IsAlternatingRowColors = false;
  d->m_ThemeMode               = nxTheme->getThemeMode();
  setObjectName("NXVirtualList");
  setStyleSheet(QStringLiteral("#NXVirtualList{background-color:transparent;}"));
  setMouseTracking(true);
  setSpacing(0);

  d->m_ListViewStyle = new NXVirtualListStyle(style());
  d->m_ListViewStyle->setItemHeight(d->m_ItemHeight);
  setStyle(d->m_ListViewStyle);

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
    d->m_ThemeMode = themeMode;
    viewport()->update();
  });
}

NXVirtualList::~NXVirtualList()
{
}

void NXVirtualList::setItemCount(int count)
{
  Q_D(NXVirtualList);
  d->m_ItemCount = count;
}

int NXVirtualList::getItemCount() const
{
  return d_ptr->m_ItemCount;
}

void NXVirtualList::paintEvent(QPaintEvent *event)
{
  Q_D(NXVirtualList);
  if (!d->m_IsTransparent) {
    QPainter painter(viewport());
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogBase));
    painter.drawRect(rect());
    painter.restore();
  }
  QListView::paintEvent(event);
}
