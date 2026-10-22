#include "NXNavigationModel.h"

#include <QIcon>
#include <QJsonObject>

#include "NXNavigationNode.h"

NXNavigationModel::NXNavigationModel(QObject *parent)
    : QAbstractItemModel { parent }
{
  _rootNode = new NXNavigationNode(QStringLiteral("root"));
  _rootNode->setIsRootNode(true);
  _rootNode->setIsExpanderNode(true);
  _rootNode->setIsExpanded(true);
  _pSelectedNode         = nullptr;
  _pSelectedExpandedNode = nullptr;
}

NXNavigationModel::~NXNavigationModel()
{
  _rootNode->deleteLater();
}

void NXNavigationModel::setIsMaximalMode(bool isMaximal)
{
  auto rootNodes = _rootNode->getChildrenNodes();
  for (int i = 0; i < rootNodes.count(); i++) {
    auto childNode = rootNodes[i];
    if (childNode->getIsCategoryNode()) {
      if (!isMaximal) {
        beginRemoveRows({}, i, i);
        _isMaximalMode = isMaximal;
        endRemoveRows();
      } else {
        beginInsertRows({}, i, i);
        _isMaximalMode = isMaximal;
        endInsertRows();
      }
    }
  }
}

bool NXNavigationModel::getIsMaximalMode()
{
  return _isMaximalMode;
}

QModelIndex NXNavigationModel::parent(const QModelIndex &child) const
{
  if (!child.isValid()) {
    return {};
  }
  NXNavigationNode *childNode  = static_cast<NXNavigationNode *>(child.internalPointer());
  NXNavigationNode *parentNode = childNode->getParentNode();
  if (parentNode == _rootNode) {
    return {};
  }
  if (parentNode == nullptr) {
    return {};
  }
  return createIndex(parentNode->getRow(), 0, parentNode);
}

QModelIndex NXNavigationModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent)) {
    return {};
  }
  NXNavigationNode *parentNode;
  if (!parent.isValid()) {
    parentNode = _rootNode;
  } else {
    parentNode = static_cast<NXNavigationNode *>(parent.internalPointer());
  }
  NXNavigationNode *childNode = nullptr;
  if (parentNode->getChildrenNodes().count() > row) {
    if (parentNode == _rootNode && !_isMaximalMode) {
      childNode = parentNode->getExceptCategoryNodes().at(row);
    } else {
      childNode = parentNode->getChildrenNodes().at(row);
    }
  }
  if (childNode) {
    childNode->setModelIndex(createIndex(row, column, childNode));
    return childNode->getModelIndex();
  }
  return {};
}

int NXNavigationModel::rowCount(const QModelIndex &parent) const
{
  NXNavigationNode *parentNode;
  if (parent.column() > 0) {
    return 0;
  }
  if (!parent.isValid()) {
    parentNode = _rootNode;
  } else {
    parentNode = static_cast<NXNavigationNode *>(parent.internalPointer());
  }
  if (parentNode == _rootNode && !_isMaximalMode) {
    return _rootNode->getExceptCategoryNodes().count();
  }
  return parentNode->getChildrenNodes().count();
};

int NXNavigationModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)
  return 1;
}

QVariant NXNavigationModel::data(const QModelIndex &index, int role) const
{
  Q_UNUSED(index)
  Q_UNUSED(role)
  return QVariant();
}

Qt::ItemFlags NXNavigationModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return Qt::NoItemFlags;
  }
  auto flags = Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
  if (index.column() != 0) {
    flags |= Qt::ItemIsEditable;
  }
  return flags;
}

QString NXNavigationModel::addExpanderNode(const QString &expanderTitle, NXIconType::IconName awesome)
{
  NXNavigationNode *node = new NXNavigationNode(expanderTitle, _rootNode);
  node->setDepth(1);
  node->setIsVisible(true);
  node->setIsExpanderNode(true);
  node->setAwesome(awesome);
  beginInsertRows(QModelIndex(), _rootNode->getChildrenNodes().count(), _rootNode->getChildrenNodes().count());
  _rootNode->appendChildNode(node);
  _nodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  return node->getNodeKey();
}

NXNodeResultExpected NXNavigationModel::addExpanderNode(const QString &expanderTitle, const QString &targetExpanderKey, NXIconType::IconName awesome)
{
  if (!_nodesMap.contains(targetExpanderKey)) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeInvalid };
  }
  NXNavigationNode *parentNode = _nodesMap.value(targetExpanderKey);
  if (!parentNode->getIsExpanderNode()) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeTypeError };
  }
  if (parentNode->getDepth() > 10) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeDepthLimit };
  }
  NXNavigationNode *node = new NXNavigationNode(expanderTitle, parentNode);
  node->setDepth(parentNode->getDepth() + 1);
  node->setIsExpanderNode(true);
  node->setAwesome(awesome);
  if (parentNode->getIsVisible() && parentNode->getIsExpanded()) {
    node->setIsVisible(true);
  }
  beginInsertRows(parentNode->getModelIndex(), parentNode->getChildrenNodes().count(), parentNode->getChildrenNodes().count());
  parentNode->appendChildNode(node);
  _nodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  return node->getNodeKey();
}

QString NXNavigationModel::addPageNode(const QString &pageTitle, NXIconType::IconName awesome)
{
  NXNavigationNode *node = new NXNavigationNode(pageTitle, _rootNode);
  node->setAwesome(awesome);
  node->setDepth(1);
  node->setIsVisible(true);
  beginInsertRows(QModelIndex(), _rootNode->getChildrenNodes().count(), _rootNode->getChildrenNodes().count());
  _rootNode->appendChildNode(node);
  _nodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  if (!_pSelectedNode) {
    _pSelectedNode = node;
  }
  return node->getNodeKey();
}

NXNodeResultExpected NXNavigationModel::addPageNode(const QString &pageTitle, const QString &targetExpanderKey, NXIconType::IconName awesome)
{
  if (!_nodesMap.contains(targetExpanderKey)) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeInvalid };
  }
  NXNavigationNode *parentNode = _nodesMap.value(targetExpanderKey);
  if (!parentNode->getIsExpanderNode()) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeTypeError };
  }
  if (parentNode->getDepth() > 10) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeDepthLimit };
  }
  NXNavigationNode *node = new NXNavigationNode(pageTitle, parentNode);
  node->setDepth(parentNode->getDepth() + 1);
  node->setAwesome(awesome);
  if (parentNode->getIsVisible() && parentNode->getIsExpanded()) {
    node->setIsVisible(true);
  }
  beginInsertRows(parentNode->getModelIndex(), parentNode->getChildrenNodes().count(), parentNode->getChildrenNodes().count());
  parentNode->appendChildNode(node);
  _nodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  if (!_pSelectedNode) {
    _pSelectedNode = node;
  }
  return node->getNodeKey();
}

QString NXNavigationModel::addPageNode(const QString &pageTitle, int keyPoints, NXIconType::IconName awesome)
{
  NXNavigationNode *node = new NXNavigationNode(pageTitle, _rootNode);
  node->setAwesome(awesome);
  node->setDepth(1);
  node->setIsVisible(true);
  node->setKeyPoints(keyPoints);
  beginInsertRows(QModelIndex(), _rootNode->getChildrenNodes().count(), _rootNode->getChildrenNodes().count());
  _rootNode->appendChildNode(node);
  _nodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  if (!_pSelectedNode) {
    _pSelectedNode = node;
  }
  return node->getNodeKey();
}

NXNodeResultExpected NXNavigationModel::addPageNode(const QString &pageTitle, const QString &targetExpanderKey, int keyPoints, NXIconType::IconName awesome)
{
  if (!_nodesMap.contains(targetExpanderKey)) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeInvalid };
  }
  NXNavigationNode *parentNode = _nodesMap.value(targetExpanderKey);
  if (!parentNode->getIsExpanderNode()) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeTypeError };
  }
  if (parentNode->getDepth() > 10) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeDepthLimit };
  }
  NXNavigationNode *node = new NXNavigationNode(pageTitle, parentNode);
  node->setDepth(parentNode->getDepth() + 1);
  node->setAwesome(awesome);
  node->setKeyPoints(keyPoints);
  if (parentNode->getIsVisible() && parentNode->getIsExpanded()) {
    node->setIsVisible(true);
  }
  beginInsertRows(parentNode->getModelIndex(), parentNode->getChildrenNodes().count(), parentNode->getChildrenNodes().count());
  parentNode->appendChildNode(node);
  _nodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  if (!_pSelectedNode) {
    _pSelectedNode = node;
  }
  return node->getNodeKey();
}

QString NXNavigationModel::addCategoryNode(const QString &categoryTitle)
{
  NXNavigationNode *node = new NXNavigationNode(categoryTitle, _rootNode);
  node->setDepth(1);
  node->setIsVisible(true);
  node->setIsCategoryNode(true);
  beginInsertRows(QModelIndex(), _rootNode->getChildrenNodes().count(), _rootNode->getChildrenNodes().count());
  _rootNode->appendChildNode(node);
  _nodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  return node->getNodeKey();
}

NXNodeResultExpected NXNavigationModel::addCategoryNode(const QString &categoryTitle, const QString &targetExpanderKey)
{
  if (!_nodesMap.contains(targetExpanderKey)) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeInvalid };
  }
  NXNavigationNode *parentNode = _nodesMap.value(targetExpanderKey);
  if (!parentNode->getIsExpanderNode()) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeTypeError };
  }
  if (parentNode->getDepth() > 10) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeDepthLimit };
  }
  NXNavigationNode *node = new NXNavigationNode(categoryTitle, parentNode);
  node->setDepth(parentNode->getDepth() + 1);
  node->setIsCategoryNode(true);
  if (parentNode->getIsVisible() && parentNode->getIsExpanded()) {
    node->setIsVisible(true);
  }
  beginInsertRows(parentNode->getModelIndex(), parentNode->getChildrenNodes().count(), parentNode->getChildrenNodes().count());
  parentNode->appendChildNode(node);
  _nodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  return node->getNodeKey();
}

QStringList NXNavigationModel::removeNavigationNode(const QString &nodeKey)
{
  QList<QString> removeKeyList;
  if (!_nodesMap.contains(nodeKey)) {
    return removeKeyList;
  }
  NXNavigationNode *node       = _nodesMap.value(nodeKey);
  NXNavigationNode *parentNode = node->getParentNode();
  if (node->getIsExpanderNode()) {
    QList<NXNavigationNode *> childNodeList = node->getChildrenNodes();
    for (int i = 0; i < childNodeList.count(); i++) {
      NXNavigationNode *childNode       = childNodeList[i];
      QList<QString> childRemoveKeyList = removeNavigationNode(childNode->getNodeKey());
      removeKeyList.append(childRemoveKeyList);
    }
  } else {
    removeKeyList.append(node->getNodeKey());
  }
  _nodesMap.remove(node->getNodeKey());
  int removeRow = parentNode->getChildrenNodes().indexOf(node);
  beginRemoveRows(parentNode->getModelIndex(), removeRow, removeRow);
  parentNode->removeChildNode(node);
  endRemoveRows();
  node->deleteLater();
  return removeKeyList;
}

const NXNavigationNode * NXNavigationModel::getRootNode() const
{
  return _rootNode;
}

NXNavigationNode * NXNavigationModel::getNavigationNode(const QString &nodeKey) const
{
  if (_nodesMap.contains(nodeKey)) {
    return _nodesMap.value(nodeKey);
  }
  return nullptr;
}

QList<NXNavigationNode *> NXNavigationModel::getRootExpanderNodes() const
{
  QList<NXNavigationNode *> expandedNodeList;
  for (auto node : _rootNode->getChildrenNodes()) {
    if (node->getIsExpanderNode()) {
      expandedNodeList.append(node);
    }
  }
  return expandedNodeList;
}

QList<NXNavigationNode *> NXNavigationModel::getRootExpandedNodes() const
{
  QList<NXNavigationNode *> expandedNodeList;
  for (auto node : _rootNode->getChildrenNodes()) {
    if (node->getIsExpanderNode() && node->getIsExpanded()) {
      expandedNodeList.append(node);
    }
  }
  return expandedNodeList;
}

QList<NXNavigationNode *> NXNavigationModel::getRootCategoryNodes() const
{
  QList<NXNavigationNode *> categoryNodeList;
  for (auto node : _rootNode->getChildrenNodes()) {
    if (node->getIsCategoryNode()) {
      categoryNodeList.append(node);
    }
  }
  return categoryNodeList;
}
