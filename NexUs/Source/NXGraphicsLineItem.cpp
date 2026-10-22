#include "NXGraphicsLineItem.h"

#include <QGraphicsScene>
#include <QPainter>

#include "NXGraphicsItem.h"
#include "private/NXGraphicsLineItemPrivate.h"
Q_PRIVATE_CREATE_CPP(NXGraphicsLineItem, QPointF, StartPoint)
Q_PRIVATE_CREATE_CPP(NXGraphicsLineItem, QPointF, EndPoint)
Q_PRIVATE_CREATE_CPP(NXGraphicsLineItem, NXGraphicsItem *, StartItem)
Q_PRIVATE_CREATE_CPP(NXGraphicsLineItem, NXGraphicsItem *, EndItem)
Q_PRIVATE_CREATE_CPP(NXGraphicsLineItem, int, StartItemPort)
Q_PRIVATE_CREATE_CPP(NXGraphicsLineItem, int, EndItemPort)

NXGraphicsLineItem::NXGraphicsLineItem(
    NXGraphicsItem *startItem, NXGraphicsItem *endItem, int startItemPort, int endItemPort, QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
    , d_ptr(new NXGraphicsLineItemPrivate())
{
  Q_D(NXGraphicsLineItem);
  d->q_ptr           = this;
  d->_pStartItem     = startItem;
  d->_pEndItem       = endItem;
  d->_pStartItemPort = startItemPort;
  d->_pEndItemPort   = endItemPort;
  d->_linkItemMap.insert(d->_pStartItem, d->_pStartItemPort);
  d->_linkItemMap.insert(d->_pEndItem, d->_pEndItemPort);
  setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable | ItemAcceptsInputMethod);
}

NXGraphicsLineItem::NXGraphicsLineItem(QPointF startPoint, QPointF endPoint, QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
    , d_ptr(new NXGraphicsLineItemPrivate())
{
  Q_D(NXGraphicsLineItem);
  d->q_ptr             = this;
  d->_pStartPoint      = startPoint;
  d->_pEndPoint        = endPoint;
  d->_isCreateWithItem = false;
  setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable | ItemAcceptsInputMethod);
}

NXGraphicsLineItem::~NXGraphicsLineItem()
{
}

bool NXGraphicsLineItem::isTargetLink(NXGraphicsItem *item) const
{
  Q_D(const NXGraphicsLineItem);
  if (d->_linkItemMap.contains(item)) {
    return true;
  }
  return false;
}

bool NXGraphicsLineItem::isTargetLink(NXGraphicsItem *item1, NXGraphicsItem *item2) const
{
  Q_D(const NXGraphicsLineItem);
  if (d->_linkItemMap.contains(item1) && d->_linkItemMap.contains(item2)) {
    return true;
  }
  return false;
}

bool NXGraphicsLineItem::isTargetLink(NXGraphicsItem *item1, NXGraphicsItem *item2, int port1, int port2) const
{
  Q_D(const NXGraphicsLineItem);
  if (d->_linkItemMap.value(item1) == port1 && d->_linkItemMap.value(item2) == port2) {
    return true;
  }
  return false;
}

void NXGraphicsLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_D(NXGraphicsLineItem);
  painter->save();
  painter->setRenderHints(QPainter::Antialiasing);
  painter->setPen(QPen(Qt::black, 3));
  QPainterPath path;
  qreal pathXStart = 0;
  qreal pathYStart = 0;
  qreal pathXEnd   = 0;
  qreal pathYEnd   = 0;
  if (d->_isCreateWithItem) {
    pathXStart = d->_pStartItem->x();
    pathYStart = d->_pStartItem->y();
    pathXEnd   = d->_pEndItem->x();
    pathYEnd   = d->_pEndItem->y();
  } else {
    pathXStart = d->_pStartPoint.x();
    pathYStart = d->_pStartPoint.y();
    pathXEnd   = d->_pEndPoint.x();
    pathYEnd   = d->_pEndPoint.y();
  }
  path.moveTo(pathXStart, pathYStart); // 设置起始点
  path.cubicTo((pathXStart + pathXEnd) / 2, pathYStart, (pathXStart + pathXEnd) / 2, pathYEnd, pathXEnd, pathYEnd);
  setPath(path);
  painter->drawPath(path);
  painter->restore();
}

QRectF NXGraphicsLineItem::boundingRect() const
{
  if (!QGraphicsPathItem::boundingRect().isValid()) {
    return scene()->sceneRect();
  }
  return QGraphicsPathItem::boundingRect();
}
