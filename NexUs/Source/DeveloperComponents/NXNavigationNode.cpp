#include "NXNavigationNode.h"

#include <QUuid>

NXNavigationNode::NXNavigationNode(const QString &nodeTitle, NXNavigationNode *parent)
    : QObject(nullptr)
{
  _pDepth           = 0;
  _pKeyPoints       = 0;
  _nodeKey          = QUuid::createUuid()
                          .toString()
                          .remove(QStringLiteral("{"))
                          .remove(QStringLiteral("}"))
                          .remove(QStringLiteral("-"));
  _pNodeTitle       = nodeTitle;
  _pIsRootNode      = false;
  _pIsFooterNode    = false;
  _pIsHasFooterPage = false;
  _pParentNode      = parent;
  _pIsExpanderNode  = false;
  _pIsVisible       = false;
  _pIsCategoryNode  = false;
  _pAwesome         = NXIconType::None;
}

NXNavigationNode::~NXNavigationNode()
{
  for (const auto childNode : _pChildrenNodes) {
    childNode->deleteLater();
  }
}

QString NXNavigationNode::getNodeKey() const
{
  return _nodeKey;
}

void NXNavigationNode::setIsExpanded(bool isExpanded)
{
  _isExpanded = isExpanded;
  setChildVisible(isExpanded);
}

bool NXNavigationNode::getIsExpanded() const
{
  return _isExpanded;
}

void NXNavigationNode::setChildVisible(bool isVisible)
{
  if (isVisible) {
    for (auto node : _pChildrenNodes) {
      node->setIsVisible(isVisible);
      if (node->getIsExpanderNode() && !node->getIsExpanded()) {
        continue;
      }
      node->setChildVisible(isVisible);
    }
  } else {
    for (auto node : _pChildrenNodes) {
      node->setChildVisible(isVisible);
      node->setIsVisible(isVisible);
    }
  }
}

bool NXNavigationNode::getIsHasChild() const
{
  if (_pChildrenNodes.count() > 0) {
    return true;
  }
  return false;
}

bool NXNavigationNode::getIsHasPageChild() const
{
  if (_pChildrenNodes.count() == 0) {
    return false;
  }
  for (auto childNode : _pChildrenNodes) {
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
  if (_pIsExpanderNode) {
    _pChildrenNodes.append(childNode);
  }
}

void NXNavigationNode::removeChildNode(NXNavigationNode *childNode)
{
  if (_pIsExpanderNode) {
    _pChildrenNodes.removeOne(childNode);
  }
}

void NXNavigationNode::insertChildNode(int row, NXNavigationNode *childNode)
{
  if (row < 0 || row > _pChildrenNodes.count())
    return;
  _pChildrenNodes.insert(row, childNode);
  childNode->setParentNode(this);
  childNode->setParent(this);
}

bool NXNavigationNode::getIsChildHasKeyPoints() const
{
  for (auto childNnode : _pChildrenNodes) {
    if (childNnode->getKeyPoints()) {
      return true;
    }
    if (childNnode->getIsChildHasKeyPoints()) {
      return true;
    }
  }
  return false;
}

NXNavigationNode * NXNavigationNode::getOriginalNode()
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
  if (_pChildrenNodes.count() > 0) {
    if (_pChildrenNodes.contains(node)) {
      return true;
    }
    for (auto childNnode : _pChildrenNodes) {
      if (childNnode->getIsChildNode(node)) {
        return true;
      }
    }
  }
  return false;
}

int NXNavigationNode::getRow() const
{
  if (_pParentNode) {
    return _pParentNode->getChildrenNodes().indexOf(const_cast<NXNavigationNode *>(this));
  }
  return 0;
}

int NXNavigationNode::getRowExceptCategoryNodes() const
{
  if (_pParentNode) {
    return _pParentNode->getExceptCategoryNodes().indexOf(const_cast<NXNavigationNode *>(this));
  }
  return 0;
}

QList<NXNavigationNode *> NXNavigationNode::getExceptCategoryNodes()
{
  QList<NXNavigationNode *> exceptCategoryNodeList;
  for (auto node : _pChildrenNodes) {
    if (!node->getIsCategoryNode()) {
      exceptCategoryNodeList.append(node);
    }
  }
  return exceptCategoryNodeList;
}
