#ifndef NXACTIONCOMMANDERNODE_H
#define NXACTIONCOMMANDERNODE_H

#include <QModelIndex>
#include <QObject>
#include "NXDef.h"

class NXActionCommanderNode : public QObject
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QPS_SCREF(QString), NodeName)
  Q_PRIVATE_CREATE(QPS_SCREF(QString), NodeIndexText)
  Q_PRIVATE_CREATE(QPS_SCREF(QString), NodeCreateTime)
  Q_PRIVATE_CREATE(QPS_SCREF(QModelIndex), ModelIndex)
  Q_PRIVATE_CREATE(QPS_SCREF(QList<NXActionCommanderNode *>), ChildNodes)
  Q_PRIVATE_CREATE(NXActionCommanderNode *, ParentNode)
  Q_PRIVATE_CREATE(NXMessageBarType::MessageMode, MessageMode)
  Q_PRIVATE_CREATE(int, NodeIndex)
  Q_PRIVATE_CREATE(bool, IsRootNode)
  Q_PRIVATE_CREATE(bool, IsCommandNode)
  Q_PRIVATE_CREATE(bool, IsCurrentNode)

public:
  explicit NXActionCommanderNode(NXActionCommanderNode *parent = nullptr);
  ~NXActionCommanderNode() override;

  void appendChildNode(NXActionCommanderNode *childNode);
  void removeChildNode(NXActionCommanderNode *childNode);

  int getRow() const;
};

#endif //NXACTIONCOMMANDERNODE_H
