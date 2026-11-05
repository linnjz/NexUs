#include "NXGraphicsItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainterPath>
#include <QUuid>

#include "NXGraphicsScene.h"
#include "private/NXGraphicsItemPrivate.h"
#include "private/NXGraphicsScenePrivate.h"
Q_PROPERTY_CREATE_CPP(NXGraphicsItem, QPS_SCREF_PAEMIT(QImage), ItemImage)
Q_PROPERTY_CREATE_CPP(NXGraphicsItem, QPS_SCREF_PAEMIT(QImage), ItemSelectedImage)
Q_PROPERTY_CREATE_CPP(NXGraphicsItem, QPS_SCREF_PAEMIT(QString), ItemName)
Q_PROPERTY_CREATE_CPP(NXGraphicsItem, QPS_SCREF_PAEMIT(QVariantMap), DataRoutes)
Q_PROPERTY_CREATE_CPP(NXGraphicsItem, QPS_PAEMIT(int), Width)
Q_PROPERTY_CREATE_CPP(NXGraphicsItem, QPS_PAEMIT(int), Height)

NXGraphicsItem::NXGraphicsItem(QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , d_ptr(new NXGraphicsItemPrivate())
{
  Q_D(NXGraphicsItem);
  d->q_ptr = this;
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable | ItemAcceptsInputMethod);
  d->m_Width             = 50;
  d->m_Height            = 50;
  d->m_ItemUID            = QUuid::createUuid()
                               .toString()
                               .remove(QStringLiteral("{"))
                               .remove(QStringLiteral("}"))
                               .remove(QStringLiteral("-"));
  d->m_ItemImage         = QImage(QStringLiteral(":/Resource/Image/Moon.jpg"));
  d->m_ItemSelectedImage = QImage(QStringLiteral(":/Resource/Image/Cirno.jpg"));
  d->m_ItemName          = QStringLiteral("");
  d->m_MaxLinkPortCount  = 1;
  d->m_CurrentLinkPortState.resize(1);
  d->m_CurrentLinkPortState.fill(false);
}

NXGraphicsItem::NXGraphicsItem(int width, int height, QGraphicsItem *parent)
    : NXGraphicsItem(parent)
{
  Q_D(NXGraphicsItem);
  d->m_Width  = width;
  d->m_Height = height;
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
  d->m_MaxLinkPortCount = maxLinkPortCount;
  if (d->m_CurrentLinkPortState.count() > maxLinkPortCount) {
    while (d->m_CurrentLinkPortState.count() > maxLinkPortCount) {
      d->m_CurrentLinkPortState.removeLast();
    }
  } else {
    while (d->m_CurrentLinkPortState.count() < maxLinkPortCount) {
      d->m_CurrentLinkPortState.append(false);
    }
  }
}

int NXGraphicsItem::getMaxLinkPortCount() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXGraphicsItem);
  return d->m_MaxLinkPortCount;
}

QString NXGraphicsItem::getItemUID() const
{
  return d_ptr->m_ItemUID;
}

void NXGraphicsItem::setLinkPortState(bool isFullLink)
{
  Q_D(NXGraphicsItem);
  d->m_CurrentLinkPortState.fill(isFullLink);
}

void NXGraphicsItem::setLinkPortState(bool isLink, int portIndex)
{
  Q_D(NXGraphicsItem);
  if (portIndex >= 0 && portIndex < d->m_MaxLinkPortCount) {
    d->m_CurrentLinkPortState[portIndex] = isLink;
  }
}

QVector<bool> NXGraphicsItem::getLinkPortState() const
{
  Q_CONST_D(NXGraphicsItem);
  return d->m_CurrentLinkPortState;
}

bool NXGraphicsItem::getLinkPortState(int portIndex) const
{
  Q_CONST_D(NXGraphicsItem);
  if (portIndex >= 0 && portIndex < d->m_MaxLinkPortCount) {
    return d->m_CurrentLinkPortState[portIndex];
  }
  return false;
}

int NXGraphicsItem::getUsedLinkPortCount() const
{
  Q_CONST_D(NXGraphicsItem);
  int currentLinkPortCount = 0;
  for (auto isLink : d->m_CurrentLinkPortState) {
    if (isLink) {
      currentLinkPortCount++;
    }
  }
  return currentLinkPortCount;
}

QVector<int> NXGraphicsItem::getUsedLinkPort() const
{
  Q_CONST_D(NXGraphicsItem);
  QVector<int> usedPortVector;
  for (int i = 0; i < d->m_MaxLinkPortCount; i++) {
    if (d->m_CurrentLinkPortState[i]) {
      usedPortVector.append(i);
    }
  }
  return usedPortVector;
}

int NXGraphicsItem::getUnusedLinkPortCount() const
{
  Q_CONST_D(NXGraphicsItem);
  return d->m_MaxLinkPortCount - getUsedLinkPortCount();
}

QVector<int> NXGraphicsItem::getUnusedLinkPort() const
{
  Q_CONST_D(NXGraphicsItem);
  QVector<int> unusedPortVector;
  for (int i = 0; i < d->m_MaxLinkPortCount; i++) {
    if (!(d->m_CurrentLinkPortState[i])) {
      unusedPortVector.append(i);
    }
  }
  return unusedPortVector;
}

QRectF NXGraphicsItem::boundingRect() const
{
  return QRect(-d_ptr->m_Width / 2, -d_ptr->m_Height / 2, d_ptr->m_Width, d_ptr->m_Height);
}

void NXGraphicsItem::paint(QPainter *painter, QStyleOptionGraphicsItem const *option, QWidget *widget)
{
  Q_D(NXGraphicsItem);
  painter->save();
  painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  if (isSelected()) {
    painter->drawImage(boundingRect(), d->m_ItemSelectedImage);
  } else {
    painter->drawImage(boundingRect(), d->m_ItemImage);
  }
  painter->restore();
}

QDataStream &
operator<< (QDataStream &stream, NXGraphicsItem const *item)
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
