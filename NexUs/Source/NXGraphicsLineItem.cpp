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
  d->m_StartItem     = startItem;
  d->m_EndItem       = endItem;
  d->m_StartItemPort = startItemPort;
  d->m_EndItemPort   = endItemPort;
  d->m_LinkItemMap.insert(d->m_StartItem, d->m_StartItemPort);
  d->m_LinkItemMap.insert(d->m_EndItem, d->m_EndItemPort);
  setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable | ItemAcceptsInputMethod);
}

NXGraphicsLineItem::NXGraphicsLineItem(QPointF startPoint, QPointF endPoint, QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
    , d_ptr(new NXGraphicsLineItemPrivate())
{
  Q_D(NXGraphicsLineItem);
  d->q_ptr             = this;
  d->m_StartPoint      = startPoint;
  d->m_EndPoint        = endPoint;
  d->m_IsCreateWithItem = false;
  setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable | ItemAcceptsInputMethod);
}

NXGraphicsLineItem::~NXGraphicsLineItem()
{
}

bool NXGraphicsLineItem::isTargetLink(NXGraphicsItem *item) const
{
  Q_CONST_D(NXGraphicsLineItem);
  if (d->m_LinkItemMap.contains(item)) {
    return true;
  }
  return false;
}

bool NXGraphicsLineItem::isTargetLink(NXGraphicsItem *item1, NXGraphicsItem *item2) const
{
  Q_CONST_D(NXGraphicsLineItem);
  if (d->m_LinkItemMap.contains(item1) && d->m_LinkItemMap.contains(item2)) {
    return true;
  }
  return false;
}

bool NXGraphicsLineItem::isTargetLink(NXGraphicsItem *item1, NXGraphicsItem *item2, int port1, int port2) const
{
  Q_CONST_D(NXGraphicsLineItem);
  if (d->m_LinkItemMap.value(item1) == port1 && d->m_LinkItemMap.value(item2) == port2) {
    return true;
  }
  return false;
}

void NXGraphicsLineItem::paint(QPainter *painter, QStyleOptionGraphicsItem const *option, QWidget *widget)
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
  if (d->m_IsCreateWithItem) {
    pathXStart = d->m_StartItem->x();
    pathYStart = d->m_StartItem->y();
    pathXEnd   = d->m_EndItem->x();
    pathYEnd   = d->m_EndItem->y();
  } else {
    pathXStart = d->m_StartPoint.x();
    pathYStart = d->m_StartPoint.y();
    pathXEnd   = d->m_EndPoint.x();
    pathYEnd   = d->m_EndPoint.y();
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
