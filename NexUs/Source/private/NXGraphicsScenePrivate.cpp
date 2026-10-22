#include "NXGraphicsScenePrivate.h"

#include "NXGraphicsItem.h"
#include "NXGraphicsLineItem.h"
#include "NXGraphicsScene.h"

NXGraphicsScenePrivate::NXGraphicsScenePrivate(QObject *parent)
    : QObject(parent)
{
}

NXGraphicsScenePrivate::~NXGraphicsScenePrivate()
{
}

QDataStream &
operator<< (QDataStream &stream, const NXGraphicsScenePrivate *data)
{
  QStringList keyList              = data->_items.keys();
  QList<NXGraphicsItem *> itemList = data->_items.values();
  stream << keyList;
  for (int i = 0; i < keyList.count(); i++) {
    stream << itemList[i];
  }
  stream << data->_itemsLink;
  return stream;
}

QDataStream &
operator>> (QDataStream &stream, NXGraphicsScenePrivate *data)
{
  QStringList keyList;
  stream >> keyList;
  QList<NXGraphicsItem *> itemList = data->_serializeItem(keyList.count());
  for (int i = 0; i < keyList.count(); i++) {
    stream >> itemList[i];
  }
  for (int i = 0; i < keyList.count(); i++) {
    data->_items.insert(keyList[i], itemList[i]);
  }
  stream >> data->_itemsLink;
  data->_deserializeLink();
  return stream;
}

QList<NXGraphicsItem *> NXGraphicsScenePrivate::_serializeItem(int count)
{
  Q_Q(NXGraphicsScene);
  QList<NXGraphicsItem *> itemList;
  for (int i = 0; i < count; i++) {
    NXGraphicsItem *item = new NXGraphicsItem();
    item->setParent(q);
    itemList.append(item);
    q->QGraphicsScene::addItem(item);
    _currentZ++;
  }
  return itemList;
}

void NXGraphicsScenePrivate::_removeLinkLineItem()
{
  Q_Q(NXGraphicsScene);
  if (_linkLineItem) {
    q->QGraphicsScene::removeItem(_linkLineItem);
    delete _linkLineItem;
    _linkLineItem = nullptr;
  }
}

void NXGraphicsScenePrivate::_deserializeLink()
{
  Q_Q(NXGraphicsScene);
  for (int i = 0; i < _itemsLink.count(); i++) {
    QVariantMap itemLinkData     = _itemsLink[i];
    QList<QString> uidList       = itemLinkData.keys();
    QList<QVariant> portList     = itemLinkData.values();
    NXGraphicsLineItem *lineItem = new NXGraphicsLineItem(_items.value(uidList[0]), _items.value(uidList[1]), portList[0].toInt(), portList[1].toInt());
    q->QGraphicsScene::addItem(lineItem);
    _lineItemsList.append(lineItem);
  }
}
