#include "NXActionCommanderNode.h"

#include <QDebug>
#include <QUuid>

NXActionCommanderNode::NXActionCommanderNode(NXActionCommanderNode *parent)
    : QObject(nullptr)
{
  _pIsRootNode    = false;
  _pIsCommandNode = false;
  _pParentNode    = parent;
  _pIsCurrentNode = false;
  _pNodeIndex     = 1;
  _pMessageMode   = NXMessageBarType::Success;
}

NXActionCommanderNode::~NXActionCommanderNode()
{
  // 不使用QT的继承链删除 手动释放 一般情况下此时_pChildNodes已经为空
  for (const auto childNode : _pChildNodes) {
    delete childNode;
  }
}

void NXActionCommanderNode::appendChildNode(NXActionCommanderNode *childNode)
{
  _pChildNodes.append(childNode);
}

void NXActionCommanderNode::removeChildNode(NXActionCommanderNode *childNode)
{
  _pChildNodes.removeOne(childNode);
}

int NXActionCommanderNode::getRow() const
{
  if (_pParentNode) {
    return _pParentNode->getChildNodes().indexOf(const_cast<NXActionCommanderNode *>(this));
  }
  return 0;
}
