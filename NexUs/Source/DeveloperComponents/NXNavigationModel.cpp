#include "NXNavigationModel.h"

#include <QIcon>
#include <QJsonObject>

#include "NXNavigationNode.h"

NXNavigationModel::NXNavigationModel(QObject *parent)
    : QAbstractItemModel { parent }
    , m_SelectedNode { nullptr }
    , m_SelectedExpandedNode { nullptr }
    , m_RootNode { new NXNavigationNode(QStringLiteral("root")) }
{
  m_RootNode->setIsRootNode(true);
  m_RootNode->setIsExpanderNode(true);
  m_RootNode->setIsExpanded(true);
}

NXNavigationModel::~NXNavigationModel()
{
  m_RootNode->deleteLater();
}

void NXNavigationModel::setIsMaximalMode(bool isMaximal)
{
  auto rootNodes = m_RootNode->getChildrenNodes();
  for (int i = 0; i < rootNodes.count(); i++) {
    auto childNode = rootNodes[i];
    if (childNode->getIsCategoryNode()) {
      if (!isMaximal) {
        beginRemoveRows({}, i, i);
        m_IsMaximalMode = isMaximal;
        endRemoveRows();
      } else {
        beginInsertRows({}, i, i);
        m_IsMaximalMode = isMaximal;
        endInsertRows();
      }
    }
  }
}

bool NXNavigationModel::getIsMaximalMode()
{
  return m_IsMaximalMode;
}

QModelIndex NXNavigationModel::parent(QModelIndex const &child) const
{
  if (!child.isValid()) {
    return {};
  }
  NXNavigationNode *childNode  = static_cast<NXNavigationNode *>(child.internalPointer());
  NXNavigationNode *parentNode = childNode->getParentNode();
  if (parentNode == m_RootNode) {
    return {};
  }
  if (parentNode == nullptr) {
    return {};
  }
  return createIndex(parentNode->getRow(), 0, parentNode);
}

QModelIndex NXNavigationModel::index(int row, int column, QModelIndex const &parent) const
{
  if (!hasIndex(row, column, parent)) {
    return {};
  }
  NXNavigationNode *parentNode;
  if (!parent.isValid()) {
    parentNode = m_RootNode;
  } else {
    parentNode = static_cast<NXNavigationNode *>(parent.internalPointer());
  }
  NXNavigationNode *childNode = nullptr;
  if (parentNode->getChildrenNodes().count() > row) {
    if (parentNode == m_RootNode && !m_IsMaximalMode) {
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

int NXNavigationModel::rowCount(QModelIndex const &parent) const
{
  NXNavigationNode *parentNode;
  if (parent.column() > 0) {
    return 0;
  }
  if (!parent.isValid()) {
    parentNode = m_RootNode;
  } else {
    parentNode = static_cast<NXNavigationNode *>(parent.internalPointer());
  }
  if (parentNode == m_RootNode && !m_IsMaximalMode) {
    return m_RootNode->getExceptCategoryNodes().count();
  }
  return parentNode->getChildrenNodes().count();
};

int NXNavigationModel::columnCount(QModelIndex const &parent) const
{
  Q_UNUSED(parent)
  return 1;
}

QVariant NXNavigationModel::data(QModelIndex const &index, int role) const
{
  Q_UNUSED(index)
  Q_UNUSED(role)
  return QVariant();
}

Qt::ItemFlags NXNavigationModel::flags(QModelIndex const &index) const
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

QString NXNavigationModel::addExpanderNode(QString const &expanderTitle, NXIconType::IconName awesome)
{
  NXNavigationNode *node = new NXNavigationNode(expanderTitle, m_RootNode);
  node->setDepth(1);
  node->setIsVisible(true);
  node->setIsExpanderNode(true);
  node->setAwesome(awesome);
  beginInsertRows(QModelIndex(), m_RootNode->getChildrenNodes().count(), m_RootNode->getChildrenNodes().count());
  m_RootNode->appendChildNode(node);
  m_NodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  return node->getNodeKey();
}

NXNodeResultExpected NXNavigationModel::addExpanderNode(QString const &expanderTitle, QString const &targetExpanderKey, NXIconType::IconName awesome)
{
  if (!m_NodesMap.contains(targetExpanderKey)) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeInvalid };
  }
  NXNavigationNode *parentNode = m_NodesMap.value(targetExpanderKey);
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
  m_NodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  return node->getNodeKey();
}

QString NXNavigationModel::addPageNode(QString const &pageTitle, NXIconType::IconName awesome)
{
  NXNavigationNode *node = new NXNavigationNode(pageTitle, m_RootNode);
  node->setAwesome(awesome);
  node->setDepth(1);
  node->setIsVisible(true);
  beginInsertRows(QModelIndex(), m_RootNode->getChildrenNodes().count(), m_RootNode->getChildrenNodes().count());
  m_RootNode->appendChildNode(node);
  m_NodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  if (!m_SelectedNode) {
    m_SelectedNode = node;
  }
  return node->getNodeKey();
}

NXNodeResultExpected NXNavigationModel::addPageNode(QString const &pageTitle, QString const &targetExpanderKey, NXIconType::IconName awesome)
{
  if (!m_NodesMap.contains(targetExpanderKey)) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeInvalid };
  }
  NXNavigationNode *parentNode = m_NodesMap.value(targetExpanderKey);
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
  m_NodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  if (!m_SelectedNode) {
    m_SelectedNode = node;
  }
  return node->getNodeKey();
}

QString NXNavigationModel::addPageNode(QString const &pageTitle, int keyPoints, NXIconType::IconName awesome)
{
  NXNavigationNode *node = new NXNavigationNode(pageTitle, m_RootNode);
  node->setAwesome(awesome);
  node->setDepth(1);
  node->setIsVisible(true);
  node->setKeyPoints(keyPoints);
  beginInsertRows(QModelIndex(), m_RootNode->getChildrenNodes().count(), m_RootNode->getChildrenNodes().count());
  m_RootNode->appendChildNode(node);
  m_NodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  if (!m_SelectedNode) {
    m_SelectedNode = node;
  }
  return node->getNodeKey();
}

NXNodeResultExpected NXNavigationModel::addPageNode(QString const &pageTitle, QString const &targetExpanderKey, int keyPoints, NXIconType::IconName awesome)
{
  if (!m_NodesMap.contains(targetExpanderKey)) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeInvalid };
  }
  NXNavigationNode *parentNode = m_NodesMap.value(targetExpanderKey);
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
  m_NodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  if (!m_SelectedNode) {
    m_SelectedNode = node;
  }
  return node->getNodeKey();
}

QString NXNavigationModel::addCategoryNode(QString const &categoryTitle)
{
  NXNavigationNode *node = new NXNavigationNode(categoryTitle, m_RootNode);
  node->setDepth(1);
  node->setIsVisible(true);
  node->setIsCategoryNode(true);
  beginInsertRows(QModelIndex(), m_RootNode->getChildrenNodes().count(), m_RootNode->getChildrenNodes().count());
  m_RootNode->appendChildNode(node);
  m_NodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  return node->getNodeKey();
}

NXNodeResultExpected NXNavigationModel::addCategoryNode(QString const &categoryTitle, QString const &targetExpanderKey)
{
  if (!m_NodesMap.contains(targetExpanderKey)) {
    return NXNodeResultUnexpected { NXNavigationType::NodeResult::TargetNodeInvalid };
  }
  NXNavigationNode *parentNode = m_NodesMap.value(targetExpanderKey);
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
  m_NodesMap.insert(node->getNodeKey(), node);
  endInsertRows();
  return node->getNodeKey();
}

QStringList NXNavigationModel::removeNavigationNode(QString const &nodeKey)
{
  QList<QString> removeKeyList;
  if (!m_NodesMap.contains(nodeKey)) {
    return removeKeyList;
  }
  NXNavigationNode *node       = m_NodesMap.value(nodeKey);
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
  m_NodesMap.remove(node->getNodeKey());
  int removeRow = parentNode->getChildrenNodes().indexOf(node);
  beginRemoveRows(parentNode->getModelIndex(), removeRow, removeRow);
  parentNode->removeChildNode(node);
  endRemoveRows();
  node->deleteLater();
  return removeKeyList;
}

NXNavigationNode const *NXNavigationModel::getRootNode() const
{
  return m_RootNode;
}

NXNavigationNode *NXNavigationModel::getNavigationNode(QString const &nodeKey) const
{
  if (m_NodesMap.contains(nodeKey)) {
    return m_NodesMap.value(nodeKey);
  }
  return nullptr;
}

QList<NXNavigationNode *> NXNavigationModel::getRootExpanderNodes() const
{
  QList<NXNavigationNode *> expandedNodeList;
  for (auto node : m_RootNode->getChildrenNodes()) {
    if (node->getIsExpanderNode()) {
      expandedNodeList.append(node);
    }
  }
  return expandedNodeList;
}

QList<NXNavigationNode *> NXNavigationModel::getRootExpandedNodes() const
{
  QList<NXNavigationNode *> expandedNodeList;
  for (auto node : m_RootNode->getChildrenNodes()) {
    if (node->getIsExpanderNode() && node->getIsExpanded()) {
      expandedNodeList.append(node);
    }
  }
  return expandedNodeList;
}

QList<NXNavigationNode *> NXNavigationModel::getRootCategoryNodes() const
{
  QList<NXNavigationNode *> categoryNodeList;
  for (auto node : m_RootNode->getChildrenNodes()) {
    if (node->getIsCategoryNode()) {
      categoryNodeList.append(node);
    }
  }
  return categoryNodeList;
}
