#ifndef NXGRAPHICSSCENEPRIVATE_H
#define NXGRAPHICSSCENEPRIVATE_H

#include <QMap>
#include <QObject>
#include <QPointF>

#include "NXGraphicsScene.h"
#include "NXProperty.h"
class NXGraphicsItem;
class NXGraphicsLineItem;

class NXGraphicsScenePrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXGraphicsScene)
  Q_PROPERTY_CREATE_D(QString, SerializePath)
  Q_PROPERTY_CREATE_D(bool, IsCheckLinkPort)

public:
  friend QDataStream &operator<< (QDataStream &stream, NXGraphicsScenePrivate const *data);
  friend QDataStream &operator>> (QDataStream &stream, NXGraphicsScenePrivate *data);
  explicit NXGraphicsScenePrivate(QObject *parent = nullptr);
  ~NXGraphicsScenePrivate();


private:
  bool m_IsLeftButtonPress { false };
  NXGraphicsSceneType::SceneMode m_SceneMode;
  QMap<QString, NXGraphicsItem *> m_Items; // 存储所有item
  qreal m_CurrentZ { 1 };
  QPointF m_LastPos;
  QPointF m_LastLeftPressPos;
  QList<QVariantMap> m_ItemsLink; // item连接状态
  QList<NXGraphicsLineItem *> m_LineItemsList;
  NXGraphicsLineItem *m_LinkLineItem { nullptr };
  QList<NXGraphicsItem *> _serializeItem(int count);
  void _removeLinkLineItem();
  void _deserializeLink();
};

#endif // NXGRAPHICSSCENEPRIVATE_H
