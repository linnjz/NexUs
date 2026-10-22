#include "NXGraphicsItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainterPath>
#include <QUuid>

#include "NXGraphicsScene.h"
#include "private/NXGraphicsItemPrivate.h"
#include "private/NXGraphicsScenePrivate.h"
Q_PROPERTY_CREATE_CPP(NXGraphicsItem, QS_SET_CREF(QImage), ItemImage)
Q_PROPERTY_CREATE_CPP(NXGraphicsItem, QS_SET_CREF(QImage), ItemSelectedImage)
Q_PROPERTY_CREATE_CPP(NXGraphicsItem, QS_SET_CREF(QString), ItemName)
Q_PROPERTY_CREATE_CPP(NXGraphicsItem, QS_SET_CREF(QVariantMap), DataRoutes)
Q_PROPERTY_CREATE_CPP(NXGraphicsItem, int, Width)
Q_PROPERTY_CREATE_CPP(NXGraphicsItem, int, Height)

NXGraphicsItem::NXGraphicsItem(QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , d_ptr(new NXGraphicsItemPrivate())
{
  Q_D(NXGraphicsItem);
  d->q_ptr = this;
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable | ItemAcceptsInputMethod);
  d->_pWidth             = 50;
  d->_pHeight            = 50;
  d->_itemUID            = QUuid::createUuid()
                               .toString()
                               .remove(QStringLiteral("{"))
                               .remove(QStringLiteral("}"))
                               .remove(QStringLiteral("-"));
  d->_pItemImage         = QImage(QStringLiteral(":/Resource/Image/Moon.jpg"));
  d->_pItemSelectedImage = QImage(QStringLiteral(":/Resource/Image/Cirno.jpg"));
  d->_pItemName          = QStringLiteral("");
  d->_pMaxLinkPortCount  = 1;
  d->_currentLinkPortState.resize(1);
  d->_currentLinkPortState.fill(false);
}

NXGraphicsItem::NXGraphicsItem(int width, int height, QGraphicsItem *parent)
    : NXGraphicsItem(parent)
{
  Q_D(NXGraphicsItem);
  d->_pWidth  = width;
  d->_pHeight = height;
}

NXGraphicsItem::~NXGraphicsItem()
{
}

void NXGraphicsItem::setMaxLinkPortCount(int maxLinkPortCount)
{
  Q_D(NXGraphicsItem);
  if (maxLinkPortCount < 0) {
    maxLinkPortCount = 0;
  }
  d->_pMaxLinkPortCount = maxLinkPortCount;
  if (d->_currentLinkPortState.count() > maxLinkPortCount) {
    while (d->_currentLinkPortState.count() > maxLinkPortCount) {
      d->_currentLinkPortState.removeLast();
    }
  } else {
    while (d->_currentLinkPortState.count() < maxLinkPortCount) {
      d->_currentLinkPortState.append(false);
    }
  }
}

int NXGraphicsItem::getMaxLinkPortCount() const
{
  Q_D(const NXGraphicsItem);
  return d->_pMaxLinkPortCount;
}

QString NXGraphicsItem::getItemUID() const
{
  return d_ptr->_itemUID;
}

void NXGraphicsItem::setLinkPortState(bool isFullLink)
{
  Q_D(NXGraphicsItem);
  d->_currentLinkPortState.fill(isFullLink);
}

void NXGraphicsItem::setLinkPortState(bool isLink, int portIndex)
{
  Q_D(NXGraphicsItem);
  if (portIndex >= 0 && portIndex < d->_pMaxLinkPortCount) {
    d->_currentLinkPortState[portIndex] = isLink;
  }
}

QVector<bool> NXGraphicsItem::getLinkPortState() const
{
  Q_D(const NXGraphicsItem);
  return d->_currentLinkPortState;
}

bool NXGraphicsItem::getLinkPortState(int portIndex) const
{
  Q_D(const NXGraphicsItem);
  if (portIndex >= 0 && portIndex < d->_pMaxLinkPortCount) {
    return d->_currentLinkPortState[portIndex];
  }
  return false;
}

int NXGraphicsItem::getUsedLinkPortCount() const
{
  Q_D(const NXGraphicsItem);
  int currentLinkPortCount = 0;
  for (auto isLink : d->_currentLinkPortState) {
    if (isLink) {
      currentLinkPortCount++;
    }
  }
  return currentLinkPortCount;
}

QVector<int> NXGraphicsItem::getUsedLinkPort() const
{
  Q_D(const NXGraphicsItem);
  QVector<int> usedPortVector;
  for (int i = 0; i < d->_pMaxLinkPortCount; i++) {
    if (d->_currentLinkPortState[i]) {
      usedPortVector.append(i);
    }
  }
  return usedPortVector;
}

int NXGraphicsItem::getUnusedLinkPortCount() const
{
  Q_D(const NXGraphicsItem);
  return d->_pMaxLinkPortCount - getUsedLinkPortCount();
}

QVector<int> NXGraphicsItem::getUnusedLinkPort() const
{
  Q_D(const NXGraphicsItem);
  QVector<int> unusedPortVector;
  for (int i = 0; i < d->_pMaxLinkPortCount; i++) {
    if (!(d->_currentLinkPortState[i])) {
      unusedPortVector.append(i);
    }
  }
  return unusedPortVector;
}

QRectF NXGraphicsItem::boundingRect() const
{
  return QRect(-d_ptr->_pWidth / 2, -d_ptr->_pHeight / 2, d_ptr->_pWidth, d_ptr->_pHeight);
}

void NXGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_D(NXGraphicsItem);
  painter->save();
  painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  if (isSelected()) {
    painter->drawImage(boundingRect(), d->_pItemSelectedImage);
  } else {
    painter->drawImage(boundingRect(), d->_pItemImage);
  }
  painter->restore();
}

QDataStream &
operator<< (QDataStream &stream, const NXGraphicsItem *item)
{
  stream << item->x();
  stream << item->y();
  stream << item->zValue();
  stream << item->getWidth();
  stream << item->getHeight();
  stream << item->d_func();
  return stream;
}

QDataStream &
operator>> (QDataStream &stream, NXGraphicsItem *item)
{
  qreal itemX;
  qreal itemY;
  qreal itemZ;
  int itemWidth;
  int itemHeight;
  stream >> itemX >> itemY >> itemZ >> itemWidth >> itemHeight;
  item->setX(itemX);
  item->setY(itemY);
  item->setZValue(itemZ);
  item->setWidth(itemWidth);
  item->setHeight(itemHeight);
  stream >> item->d_func();
  return stream;
}
