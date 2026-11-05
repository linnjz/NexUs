#include "NXNavigationNode.h"

#include <QUuid>

NXNavigationNode::NXNavigationNode(QString const &nodeTitle, NXNavigationNode *parent)
    : QObject { nullptr }
    , m_NodeTitle { nodeTitle }
    , m_ParentNode { parent }
    , m_Awesome { NXIconType::None }
    , m_KeyPoints { 0 }
    , m_Depth { 0 }
    , m_IsRootNode { false }
    , m_IsFooterNode { false }
    , m_IsHasFooterPage { false }
    , m_IsExpanderNode { false }
    , m_IsCategoryNode { false }
    , m_IsVisible { false }
    , m_NodeKey { QUuid::createUuid().toRfc4122().toHex() }
{
}

NXNavigationNode::~NXNavigationNode()
{
  for (auto const childNode : m_ChildrenNodes) {
    childNode->deleteLater();
  }
}

QString NXNavigationNode::getNodeKey() const
{
  return m_NodeKey;
}

void NXNavigationNode::setIsExpanded(bool isExpanded)
{
  m_IsExpanded = isExpanded;
  setChildVisible(isExpanded);
}

bool NXNavigationNode::getIsExpanded() const
{
  return m_IsExpanded;
}

void NXNavigationNode::setChildVisible(bool isVisible)
{
  if (isVisible) {
    for (auto node : m_ChildrenNodes) {
      node->setIsVisible(isVisible);
      if (node->getIsExpanderNode() && !node->getIsExpanded()) {
        continue;
      }
      node->setChildVisible(isVisible);
    }
  } else {
    for (auto node : m_ChildrenNodes) {
      node->setChildVisible(isVisible);
      node->setIsVisible(isVisible);
    }
  }
}

bool NXNavigationNode::getIsHasChild() const
{
  if (m_ChildrenNodes.count() > 0) {
    return true;
  }
  return false;
}

bool NXNavigationNode::getIsHasPageChild() const
{
  if (m_ChildrenNodes.count() == 0) {
    return false;
  }
  for (auto childNode : m_ChildrenNodes) {
    if (!childNode->getIsExpanderNode()) {
      return true;
    }
    if (childNode->getIsHasPageChild()) {
      return true;
    }
  }
  return false;
}

void NXNavigationNode::appendChildNode(NXNavigationNode *childNode)
{
  if (m_IsExpanderNode) {
    m_ChildrenNodes.append(childNode);
  }
}

void NXNavigationNode::removeChildNode(NXNavigationNode *childNode)
{
  if (m_IsExpanderNode) {
    m_ChildrenNodes.removeOne(childNode);
  }
}

void NXNavigationNode::insertChildNode(int row, NXNavigationNode *childNode)
{
  if (row < 0 || row > m_ChildrenNodes.count())
    return;
  m_ChildrenNodes.insert(row, childNode);
  childNode->setParentNode(this);
  childNode->setParent(this);
}

bool NXNavigationNode::getIsChildHasKeyPoints() const
{
  for (auto childNnode : m_ChildrenNodes) {
    if (childNnode->getKeyPoints()) {
      return true;
    }
    if (childNnode->getIsChildHasKeyPoints()) {
      return true;
    }
  }
  return false;
}

NXNavigationNode *NXNavigationNode::getOriginalNode()
{
  if (this->getParentNode()->getIsRootNode()) {
    return this;
  } else {
    NXNavigationNode *originalNode = this->getParentNode();
    while (!originalNode->getIsRootNode() && !originalNode->getParentNode()->getIsRootNode()) {
      originalNode = originalNode->getParentNode();
    }
    return originalNode;
  }
}

bool NXNavigationNode::getIsChildNode(NXNavigationNode *node)
{
  if (m_ChildrenNodes.count() > 0) {
    if (m_ChildrenNodes.contains(node)) {
      return true;
    }
    for (auto childNnode : m_ChildrenNodes) {
      if (childNnode->getIsChildNode(node)) {
        return true;
      }
    }
  }
  return false;
}

int NXNavigationNode::getRow() const
{
  if (m_ParentNode) {
    return m_ParentNode->getChildrenNodes().indexOf(const_cast<NXNavigationNode *>(this));
  }
  return 0;
}

int NXNavigationNode::getRowExceptCategoryNodes() const
{
  if (m_ParentNode) {
    return m_ParentNode->getExceptCategoryNodes().indexOf(const_cast<NXNavigationNode *>(this));
  }
  return 0;
}

QList<NXNavigationNode *> NXNavigationNode::getExceptCategoryNodes()
{
  QList<NXNavigationNode *> exceptCategoryNodeList;
  for (auto node : m_ChildrenNodes) {
    if (!node->getIsCategoryNode()) {
      exceptCategoryNodeList.append(node);
    }
  }
  return exceptCategoryNodeList;
}
