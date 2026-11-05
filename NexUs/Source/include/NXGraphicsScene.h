#ifndef NXGRAPHICSSCENE_H
#define NXGRAPHICSSCENE_H

#include <QDataStream>
#include <QEvent>
#include <QGraphicsScene>
#include <QJsonObject>
#include <QObject>
#include <QPainter>

#include "NXDef.h"
#include "NXProperty.h"

class NXGraphicsItem;
class NXGraphicsScenePrivate;

class NX_EXPORT NXGraphicsScene : public QGraphicsScene
{
  Q_OBJECT
  Q_Q_CREATE(NXGraphicsScene)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), SerializePath)
  Q_PROPERTY_CREATE_H(bool, IsCheckLinkPort)

public:
  explicit NXGraphicsScene(QObject *parent = nullptr);
  ~NXGraphicsScene();
  void addItem(NXGraphicsItem *item);
  void removeItem(NXGraphicsItem *item);
  void removeSelectedItems();
  void clear();

  QList<NXGraphicsItem *> createAndAddItem(int width, int height, int count = 1);
  QList<NXGraphicsItem *> getSelectedNXItems() const;
  QList<NXGraphicsItem *> getNXItems();
  QList<NXGraphicsItem *> getNXItems(QPoint pos);
  QList<NXGraphicsItem *> getNXItems(QPointF pos);
  QList<NXGraphicsItem *> getNXItems(QRect rect);
  QList<NXGraphicsItem *> getNXItems(QRectF rect);

  void setSceneMode(NXGraphicsSceneType::SceneMode mode);
  NXGraphicsSceneType::SceneMode getSceneMode() const;

  void selectAllItems();

  QList<QVariantMap> getItemLinkList() const;
  bool addItemLink(NXGraphicsItem *item1, NXGraphicsItem *item2, int port1 = 0, int port2 = 0);
  bool removeItemLink(NXGraphicsItem *item1);
  bool removeItemLink(NXGraphicsItem *item1, NXGraphicsItem *item2, int port1 = 0, int port2 = 0);

  QVector<QVariantMap> getItemsDataRoute() const;

  // 序列化 反序列化
  void serialize();
  void deserialize();
  Q_SIGNAL void showItemLink();
  Q_SIGNAL void mouseLeftClickedItem(NXGraphicsItem *item);
  Q_SIGNAL void mouseRightClickedItem(NXGraphicsItem *item);
  Q_SIGNAL void mouseDoubleClickedItem(NXGraphicsItem *item);

protected:
  void focusOutEvent(QFocusEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // NXGRAPHICSSCENE_H
