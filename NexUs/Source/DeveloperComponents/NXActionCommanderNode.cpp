#include "NXActionCommanderNode.h"

#include <QDebug>
#include <QUuid>

NXActionCommanderNode::NXActionCommanderNode(NXActionCommanderNode *parent)
    : QObject(nullptr)
    , m_NodeName {}
    , m_NodeIndexText {}
    , m_NodeCreateTime {}
    , m_ModelIndex {}
    , m_ChildNodes {}
    , m_ParentNode { parent }
    , m_MessageMode { NXMessageBarType::Success }
    , m_NodeIndex { 1 }
    , m_IsRootNode { false }
    , m_IsCommandNode { false }
    , m_IsCurrentNode { false }
{
}

NXActionCommanderNode::~NXActionCommanderNode()
{
  // 不使用QT的继承链删除 手动释放 一般情况下此时m_ChildNodes已经为空
  for (auto const childNode : m_ChildNodes) {
    delete childNode;
  }
}

void NXActionCommanderNode::appendChildNode(NXActionCommanderNode *childNode)
{
  m_ChildNodes.append(childNode);
}

void NXActionCommanderNode::removeChildNode(NXActionCommanderNode *childNode)
{
  m_ChildNodes.removeOne(childNode);
}

int NXActionCommanderNode::getRow() const
{
  if (m_ParentNode) {
    return m_ParentNode->getChildNodes().indexOf(const_cast<NXActionCommanderNode *>(this));
  }
  return 0;
}
