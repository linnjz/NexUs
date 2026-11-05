#include "NXGraphicsScene.h"

#include <QDebug>
#include <QFile>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QKeyEvent>

#include "NXGraphicsItem.h"
#include "NXGraphicsLineItem.h"
#include "private/NXGraphicsScenePrivate.h"
Q_PROPERTY_CREATE_CPP(NXGraphicsScene, QPS_SCREF_PAEMIT(QString), SerializePath)
Q_PROPERTY_CREATE_CPP(NXGraphicsScene, QPS_PAEMIT(bool), IsCheckLinkPort)

NXGraphicsScene::NXGraphicsScene(QObject *parent)
    : QGraphicsScene(parent)
    , d_ptr(new NXGraphicsScenePrivate())
{
  Q_D(NXGraphicsScene);
  d->q_ptr = this;
  setItemIndexMethod(QGraphicsScene::NoIndex);
  d->m_IsCheckLinkPort = false;
  d->m_SceneMode        = NXGraphicsSceneType::SceneMode::Default;
  d->m_SerializePath   = QStringLiteral("./scene.bin");
}

NXGraphicsScene::~NXGraphicsScene()
{
}

void NXGraphicsScene::addItem(NXGraphicsItem *item)
{
  Q_D(NXGraphicsScene);
  if (!item) {
    return;
  }
  for (auto const &pair : d->m_Items.toStdMap()) {
    if (pair.second == item) {
      return;
    }
  }
  item->setParent(this);
  item->setZValue(d->m_CurrentZ);
  if (item->getItemName().isEmpty()) {
    item->setItemName(QString(QStringLiteral("NXItem%1")).arg(d->m_CurrentZ));
  }
  item->setPos(sceneRect().width() / 2, sceneRect().height() / 2);
  QGraphicsScene::addItem(item);
  d->m_CurrentZ++;
  d->m_Items.insert(item->getItemUID(), item);
}

void NXGraphicsScene::removeItem(NXGraphicsItem *item)
{
  Q_D(NXGraphicsScene);
  if (!item) {
    return;
  }
  d->m_Items.remove(d->m_Items.key(item));
  removeItemLink(item);
  QGraphicsScene::removeItem(item);
  delete item;
  update();
}

void NXGraphicsScene::removeSelectedItems()
{
  QList<NXGraphicsItem *> selectedItemList = getSelectedNXItems();
  if (selectedItemList.count() == 0) {
    return;
  }
  for (auto item : selectedItemList) {
    removeItem(item);
  }
}

void NXGraphicsScene::clear()
{
  Q_D(NXGraphicsScene);
  d->m_ItemsLink.clear();
  for (auto const &pair : d->m_Items.toStdMap()) {
    delete pair.second;
  }
  d->m_Items.clear();
  update();
}

QList<NXGraphicsItem *> NXGraphicsScene::createAndAddItem(int width, int height, int count)
{
  if (count <= 0) {
    return QList<NXGraphicsItem *>();
  }
  QList<NXGraphicsItem *> createItemList;
  for (int i = 0; i < count; i++) {
    NXGraphicsItem *item = new NXGraphicsItem();
    item->setWidth(width);
    item->setHeight(height);
    createItemList.append(item);
    addItem(item);
  }
  return createItemList;
}

QList<NXGraphicsItem *> NXGraphicsScene::getSelectedNXItems() const
{
  QList<QGraphicsItem *> selectedItemList = selectedItems();
  QList<NXGraphicsItem *> selectedNXItemList;
  for (auto item : selectedItemList) {
    NXGraphicsItem *itemCast = dynamic_cast<NXGraphicsItem *>(item);
    if (itemCast) {
      selectedNXItemList.append(itemCast);
    }
  }
  return selectedNXItemList;
}

QList<NXGraphicsItem *> NXGraphicsScene::getNXItems()
{
  Q_D(NXGraphicsScene);
  return d->m_Items.values();
}

QList<NXGraphicsItem *> NXGraphicsScene::getNXItems(QPoint pos)
{
  return getNXItems(pos.toPointF());
}

QList<NXGraphicsItem *> NXGraphicsScene::getNXItems(QPointF pos)
{
  QList<NXGraphicsItem *> result;
  for (QGraphicsItem *item : items(pos)) {
    if (auto nxItem = dynamic_cast<NXGraphicsItem *>(item))
      result.append(nxItem);
  }
  return result;
}

QList<NXGraphicsItem *> NXGraphicsScene::getNXItems(QRect rect)
{
  return getNXItems(rect.toRectF());
}

QList<NXGraphicsItem *> NXGraphicsScene::getNXItems(QRectF rect)
{
  QList<NXGraphicsItem *> result;
  for (QGraphicsItem *item : items(rect)) {
    if (auto nxItem = dynamic_cast<NXGraphicsItem *>(item))
      result.append(nxItem);
  }
  return result;
}

void NXGraphicsScene::setSceneMode(NXGraphicsSceneType::SceneMode mode)
{
  Q_D(NXGraphicsScene);
  d->m_SceneMode = mode;
  if (mode == NXGraphicsSceneType::SceneMode::DragMove) {
    views().at(0)->setDragMode(QGraphicsView::ScrollHandDrag);
  } else {
    views().at(0)->setDragMode(QGraphicsView::RubberBandDrag);
  }
}

NXGraphicsSceneType::SceneMode NXGraphicsScene::getSceneMode() const
{
  return d_ptr->m_SceneMode;
}

void NXGraphicsScene::selectAllItems()
{
  Q_D(NXGraphicsScene);
  for (auto const &pair : d->m_Items.toStdMap()) {
    NXGraphicsItem *item = pair.second;
    item->setSelected(true);
  }
}

QList<QVariantMap> NXGraphicsScene::getItemLinkList() const
{
  return d_ptr->m_ItemsLink;
}

bool NXGraphicsScene::addItemLink(NXGraphicsItem *item1, NXGraphicsItem *item2, int port1, int port2)
{
  Q_D(NXGraphicsScene);
  if (!item1 || !item2 || (item1 == item2) || port1 < 0 || port2 < 0 || item1->getMaxLinkPortCount() <= port1 ||
      item2->getMaxLinkPortCount() <= port2) {
    return false;
  }
  if (d->m_IsCheckLinkPort) {
    if (!item1->getLinkPortState(port1) && !item2->getLinkPortState(port2)) {
      item1->setLinkPortState(true, port1);
      item2->setLinkPortState(true, port2);
    } else {
      return false;
    }
  }
  QVariantMap linkObject;
  linkObject.insert(item1->getItemUID(), port1);
  linkObject.insert(item2->getItemUID(), port2);
  d->m_ItemsLink.append(linkObject);
  NXGraphicsLineItem *lineItem = new NXGraphicsLineItem(item1, item2, port1, port2);
  QGraphicsScene::addItem(lineItem);
  d->m_LineItemsList.append(lineItem);
  update();
  return true;
}

bool NXGraphicsScene::removeItemLink(NXGraphicsItem *item1)
{
  Q_D(NXGraphicsScene);
  if (!item1) {
    return false;
  }
  if (d->m_IsCheckLinkPort) {
    item1->setLinkPortState(false);
  }
  // 处理与该Item有关的连接
  foreach(auto &link, d->m_ItemsLink)
  {
    if (link.contains(item1->getItemUID())) {
      if (d->m_IsCheckLinkPort) {
        // 解除otherItem端口占用
        QStringList keys = link.keys();
        keys.removeOne(item1->getItemUID());
        NXGraphicsItem *otherItem = d->m_Items.value(keys.at(0));
        otherItem->setLinkPortState(false, link.value(keys.at(0)).toInt());
      }
      d->m_ItemsLink.removeOne(link);
    }
  }
  // 处理连接图元
  foreach(auto lineItem, d->m_LineItemsList)
  {
    if (lineItem->isTargetLink(item1)) {
      d->m_LineItemsList.removeOne(lineItem);
      QGraphicsScene::removeItem(lineItem);
      delete lineItem;
    }
  }
  update();
  return true;
}

bool NXGraphicsScene::removeItemLink(NXGraphicsItem *item1, NXGraphicsItem *item2, int port1, int port2)
{
  Q_D(NXGraphicsScene);
  if (!item1 || !item2) {
    return false;
  }
  bool isLinkExist = false;
  foreach(auto &link, d->m_ItemsLink)
  {
    QVariant portVariant1 = link.value(item1->getItemUID());
    QVariant portVariant2 = link.value(item2->getItemUID());
    if (portVariant1.isValid() && portVariant2.isValid() && portVariant1.toUInt() == port1 &&
        portVariant2.toUInt() == port2) {
      d->m_ItemsLink.removeOne(link);
      // 这里处理连线图元
      isLinkExist = true;
      break;
    }
  }
  foreach(auto lineItem, d->m_LineItemsList)
  {
    if (lineItem->isTargetLink(item1, item2, port1, port2)) {
      d->m_LineItemsList.removeOne(lineItem);
      QGraphicsScene::removeItem(lineItem);
      delete lineItem;
      break;
    }
  }
  if (isLinkExist) {
    if (d->m_IsCheckLinkPort) {
      item1->setLinkPortState(false, port1);
      item2->setLinkPortState(false, port2);
    }
    update();
    return true;
  } else {
    return false;
  }
}

QVector<QVariantMap> NXGraphicsScene::getItemsDataRoute() const
{
  QVector<QVariantMap> dataRouteVector;
  for (auto const &pair : d_ptr->m_Items.toStdMap()) {
    NXGraphicsItem *item = pair.second;
    dataRouteVector.append(item->getDataRoutes());
  }
  return dataRouteVector;
}

void NXGraphicsScene::serialize()
{
  Q_D(NXGraphicsScene);
  QFile file(d->m_SerializePath);
  if (!file.open(QIODevice::WriteOnly)) {
    qDebug() << "serialize Error";
    return;
  }
  QDataStream serialStream(&file);
  serialStream << d;
  file.flush();
  file.close();
}

void NXGraphicsScene::deserialize()
{
  Q_D(NXGraphicsScene);
  QFile file(d->m_SerializePath);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "deserialize Error";
    return;
  }
  QDataStream deserialStream(&file);
  deserialStream >> d;
  file.close();
  update();
}

void NXGraphicsScene::focusOutEvent(QFocusEvent *event)
{
  Q_D(NXGraphicsScene);
  d->m_SceneMode = NXGraphicsSceneType::SceneMode::Default;
  d->_removeLinkLineItem();
  QGraphicsScene::focusOutEvent(event);
}

void NXGraphicsScene::keyPressEvent(QKeyEvent *event)
{
  Q_D(NXGraphicsScene);
  switch (event->key()) {
  case Qt::Key_Control : {
    d->m_SceneMode = NXGraphicsSceneType::SceneMode::MultiSelect;
    break;
  }
  case Qt::Key_Shift : {
    d->m_SceneMode = NXGraphicsSceneType::SceneMode::ItemLink;
    clearSelection();
    break;
  }
  case Qt::Key_Delete : {
    removeSelectedItems();
    break;
  }
  }
  QGraphicsScene::keyPressEvent(event);
}

void NXGraphicsScene::keyReleaseEvent(QKeyEvent *event)
{
  Q_D(NXGraphicsScene);
  d->m_SceneMode = NXGraphicsSceneType::SceneMode::Default;
  d->_removeLinkLineItem();
  QGraphicsScene::keyReleaseEvent(event);
}

void NXGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  Q_D(NXGraphicsScene);

  if (event->button() == Qt::LeftButton) {
    if (d->m_SceneMode == NXGraphicsSceneType::SceneMode::Default) {
      d->m_IsLeftButtonPress = true;
      d->m_LastPos           = event->pos();
    }
  }
  QList<QGraphicsItem *> selectedItemList = selectedItems();
  QGraphicsScene::mousePressEvent(event);
  if (d->m_SceneMode == NXGraphicsSceneType::SceneMode::ItemLink) {
    for (auto item : selectedItemList) {
      item->setSelected(true);
    }
    d->m_LastPos = event->pos();
  }
}

void NXGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  Q_D(NXGraphicsScene);
  QList<NXGraphicsItem *> itemList = getNXItems(event->scenePos());
  if (event->button() == Qt::LeftButton) {
    d->m_IsLeftButtonPress = false;
    switch (d->m_SceneMode) {
    case NXGraphicsSceneType::SceneMode::Default : {
      if (itemList.count() > 0) {
        Q_EMIT mouseLeftClickedItem(itemList[0]);
      }
      break;
    }
    case NXGraphicsSceneType::SceneMode::ItemLink : {
      QList<NXGraphicsItem *> selectedItemList = getSelectedNXItems();
      if (selectedItemList.count() == 1 && !d->m_LinkLineItem) {
        d->m_LinkLineItem = new NXGraphicsLineItem(selectedItemList.at(0)->pos(), selectedItemList.at(0)->pos());
        QGraphicsScene::addItem(d->m_LinkLineItem);
      } else if (selectedItemList.count() == 2) {
        if (d->m_IsCheckLinkPort) {
          Q_EMIT showItemLink();
        } else {
          QVariantMap linkObject;
          for (auto item : selectedItemList) {
            linkObject.insert(item->getItemUID(), 0);
          }
          d->m_ItemsLink.append(linkObject);
          QGraphicsScene::mouseReleaseEvent(event);
          d->_removeLinkLineItem();
          addItemLink(selectedItemList.at(0), selectedItemList.at(1));
          clearSelection();
          return;
        }
      } else {
        d->_removeLinkLineItem();
        update();
      }
      break;
    }
    default : {
      break;
    }
    }
  } else if (event->button() == Qt::RightButton) {
    if (itemList.count() > 0) {
      clearSelection();
      itemList[0]->setSelected(true);
      Q_EMIT mouseRightClickedItem(itemList[0]);
    }
  }
  QGraphicsScene::mouseReleaseEvent(event);
}

void NXGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  Q_D(NXGraphicsScene);
  if (d->m_SceneMode == NXGraphicsSceneType::SceneMode::ItemLink) {
    if (getSelectedNXItems().count() == 1) {
      if (d->m_LinkLineItem) {
        d->m_LinkLineItem->setEndPoint(event->scenePos());
        d->m_LinkLineItem->update();
      }
    }
  } else {
    if (d->m_IsLeftButtonPress) {
      for (auto lineItem : d->m_LineItemsList) {
        lineItem->update();
      }
    }
  }
  QGraphicsScene::mouseMoveEvent(event);
}

void NXGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  QList<NXGraphicsItem *> itemList = getNXItems(event->scenePos());
  if (itemList.count() > 0) {
    Q_EMIT mouseDoubleClickedItem(itemList[0]);
  }
  QGraphicsScene::mouseDoubleClickEvent(event);
}
