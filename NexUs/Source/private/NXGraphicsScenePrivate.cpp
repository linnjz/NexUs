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

QDataStream & operator<< (QDataStream &stream, NXGraphicsScenePrivate const *data)
{
  QStringList keyList              = data->m_Items.keys();
  QList<NXGraphicsItem *> itemList = data->m_Items.values();
  stream << keyList;
  for (int i = 0; i < keyList.count(); i++) {
    stream << itemList[i];
  }
  stream << data->m_ItemsLink;
  return stream;
}

QDataStream & operator>> (QDataStream &stream, NXGraphicsScenePrivate *data)
{
  QStringList keyList;
  stream >> keyList;
  QList<NXGraphicsItem *> itemList = data->_serializeItem(keyList.count());
  for (int i = 0; i < keyList.count(); i++) {
    stream >> itemList[i];
  }
  for (int i = 0; i < keyList.count(); i++) {
    data->m_Items.insert(keyList[i], itemList[i]);
  }
  stream >> data->m_ItemsLink;
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
    m_CurrentZ++;
  }
  return itemList;
}

void NXGraphicsScenePrivate::_removeLinkLineItem()
{
  Q_Q(NXGraphicsScene);
  if (m_LinkLineItem) {
    q->QGraphicsScene::removeItem(m_LinkLineItem);
    delete m_LinkLineItem;
    m_LinkLineItem = nullptr;
  }
}

void NXGraphicsScenePrivate::_deserializeLink()
{
  Q_Q(NXGraphicsScene);
  for (int i = 0; i < m_ItemsLink.count(); i++) {
    QVariantMap itemLinkData     = m_ItemsLink[i];
    QList<QString> uidList       = itemLinkData.keys();
    QList<QVariant> portList     = itemLinkData.values();
    NXGraphicsLineItem *lineItem = new NXGraphicsLineItem(m_Items.value(uidList[0]), m_Items.value(uidList[1]), portList[0].toInt(), portList[1].toInt());
    q->QGraphicsScene::addItem(lineItem);
    m_LineItemsList.append(lineItem);
  }
}
