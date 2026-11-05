#ifndef NXGRAPHICSLINEITEM_H
#define NXGRAPHICSLINEITEM_H

#include <QGraphicsPathItem>

#include "NXProperty.h"
class NXGraphicsItem;
class NXGraphicsLineItemPrivate;

class NX_EXPORT NXGraphicsLineItem : public QGraphicsPathItem
{
  Q_Q_CREATE(NXGraphicsLineItem)
  Q_PRIVATE_CREATE_H(QPointF, StartPoint)
  Q_PRIVATE_CREATE_H(QPointF, EndPoint)
  Q_PRIVATE_CREATE_H(NXGraphicsItem *, StartItem)
  Q_PRIVATE_CREATE_H(NXGraphicsItem *, EndItem)
  Q_PRIVATE_CREATE_H(int, StartItemPort)
  Q_PRIVATE_CREATE_H(int, EndItemPort)

public:
  explicit NXGraphicsLineItem(NXGraphicsItem *startItem, NXGraphicsItem *endItem, int startItemPort, int endItemPort, QGraphicsItem *parent = nullptr);
  explicit NXGraphicsLineItem(QPointF startPoint, QPointF endPoint, QGraphicsItem *parent = nullptr);
  ~NXGraphicsLineItem();

  bool isTargetLink(NXGraphicsItem *item) const;
  bool isTargetLink(NXGraphicsItem *item1, NXGraphicsItem *item2) const;
  bool isTargetLink(NXGraphicsItem *item1, NXGraphicsItem *item2, int port1, int port2) const;

protected:
  void paint(QPainter *painter, QStyleOptionGraphicsItem const *option, QWidget *widget = nullptr) override;
  QRectF boundingRect() const override;
};

#endif // NXGRAPHICSLINEITEM_H
