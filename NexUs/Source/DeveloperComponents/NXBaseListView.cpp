#include "NXBaseListView.h"

#include <QMouseEvent>

#include "NXScrollBar.h"

NXBaseListView::NXBaseListView(QWidget *parent)
    : QListView(parent)
{
  setObjectName("NXBaseListView");
  setStyleSheet(QStringLiteral("NXBaseListView{background-color: transparent;border:0px;}"
                               "NXBaseListView::item{border:none;}"));
  setAutoScroll(false);
  setFocusPolicy(Qt::NoFocus);
  setVerticalScrollBar(new NXScrollBar(this));
  setHorizontalScrollBar(new NXScrollBar(this));
  setSelectionMode(QAbstractItemView::NoSelection);
  setMouseTracking(true);
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

NXBaseListView::~NXBaseListView()
{
}

void NXBaseListView::wheelEvent(QWheelEvent *event)
{
  QListView::wheelEvent(event);
  event->accept();
}

void NXBaseListView::mousePressEvent(QMouseEvent *event)
{
  Q_EMIT mousePress(indexAt(event->pos()));
  QListView::mousePressEvent(event);
}

void NXBaseListView::mouseReleaseEvent(QMouseEvent *event)
{
  Q_EMIT mouseRelease(indexAt(event->pos()));
  QListView::mouseReleaseEvent(event);
}

void NXBaseListView::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_EMIT mouseDoubleClick(indexAt(event->pos()));
  QListView::mouseDoubleClickEvent(event);
}
