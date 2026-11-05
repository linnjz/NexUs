#include "NXActionCommanderModel.h"

#include <QDebug>
#include <QTime>

#include "DeveloperComponents/NXActionCommanderNode.h"
#include "NXActionCommander.h"

NXActionCommanderModel::NXActionCommanderModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_RootNode { new NXActionCommanderNode(nullptr) }
{
  m_RootNode->setIsRootNode(true);
}

NXActionCommanderModel::~NXActionCommanderModel()
{
  delete m_RootNode;
}

QModelIndex NXActionCommanderModel::parent(QModelIndex const &child) const
{
  if (!child.isValid()) {
    return {};
  }
  auto const childNode { static_cast<NXActionCommanderNode *>(child.internalPointer()) };
  NXActionCommanderNode *parentNode { childNode->getParentNode() };
  if (parentNode == m_RootNode) {
    return {};
  }
  if (parentNode == nullptr) {
    return {};
  }
  return createIndex(parentNode->getRow(), 0, parentNode);
}

QModelIndex NXActionCommanderModel::index(int row, int column, QModelIndex const &parent) const
{
  if (!hasIndex(row, column, parent)) {
    return {};
  }
  NXActionCommanderNode *parentNode;
  if (!parent.isValid()) {
    parentNode = m_RootNode;
  } else {
    parentNode = static_cast<NXActionCommanderNode *>(parent.internalPointer());
  }
  NXActionCommanderNode *childNode = nullptr;
  if (parentNode->getChildNodes().count() > row) {
    childNode = parentNode->getChildNodes()[row];
  }
  if (childNode) {
    childNode->setModelIndex(createIndex(row, column, childNode));
    return childNode->getModelIndex();
  }
  return {};
}

int NXActionCommanderModel::rowCount(QModelIndex const &parent) const
{
  NXActionCommanderNode *parentNode;
  if (parent.column() > 0) {
    return 0;
  }
  if (!parent.isValid()) {
    parentNode = m_RootNode;
  } else {
    parentNode = static_cast<NXActionCommanderNode *>(parent.internalPointer());
  }
  if (!parentNode) {
    return 0;
  }
  return parentNode->getChildNodes().count();
}

int NXActionCommanderModel::columnCount(QModelIndex const &parent) const
{
  Q_UNUSED(parent)
  return 1;
}

QVariant NXActionCommanderModel::data(QModelIndex const &index, int role) const
{
  if (index.column() == 0) {
    switch (role) {
    case Qt::DisplayRole : {
      if (auto const node = static_cast<NXActionCommanderNode *>(index.internalPointer())) {
        // 空出图标位置
        if (node->getIsCommandNode()) {
          return QStringLiteral("   ") + node->getNodeName();
        }
        return node->getNodeName();
      }
      return {};
    }
    case Qt::UserRole : {
      if (auto const node = static_cast<NXActionCommanderNode *>(index.internalPointer())) {
        return node->getMessageMode();
      }
      return {};
    }
    default : {
      break;
    }
    }
  }
  return {};
}

void NXActionCommanderModel::addCommand(NXActionCommand *command)
{
  if (!command || m_CommanderNodeMap.contains(command)) {
    return;
  }
  beginInsertRows(m_RootNode->getModelIndex(), m_CommanderNodeMap.count(), m_CommanderNodeMap.count());
  auto commandNode = new NXActionCommanderNode(m_RootNode);

  commandNode->setMessageMode(command->getMessageMode());
  commandNode->setIsCommandNode(true);
  int nodeIndex = 1;
  if (!m_RootNode->getChildNodes().isEmpty()) {
    nodeIndex = m_RootNode->getChildNodes().last()->getNodeIndex() + 1;
  }
  commandNode->setNodeIndex(nodeIndex);
  commandNode->setNodeIndexText(QStringLiteral("%1").arg(nodeIndex, 3, 10, QChar('0')));
  commandNode->setNodeCreateTime(QTime::currentTime().toString("hh:mm:ss"));
  // 主要行
  commandNode->setNodeName(QStringLiteral("%1 [%2] %3")
                               .arg(commandNode->getNodeIndexText())
                               .arg(commandNode->getNodeCreateTime())
                               .arg(command->getCommandDesc()));
  m_RootNode->appendChildNode(commandNode);
  endInsertRows();
  // 次要行
  auto commandTypeNode = new NXActionCommanderNode(commandNode);
  commandNode->appendChildNode(commandTypeNode);
  commandTypeNode->setNodeName(QStringLiteral("命令类型: %1").arg(command->metaObject()->className()));
  auto commandStateNode = new NXActionCommanderNode(commandNode);
  commandNode->appendChildNode(commandStateNode);
  commandStateNode->setNodeName(QStringLiteral("命令状态: 已执行"));
  auto commandResultNode = new NXActionCommanderNode(commandNode);
  commandNode->appendChildNode(commandResultNode);
  switch (commandNode->getMessageMode()) {
  case NXMessageBarType::Information :
  case NXMessageBarType::Success     : {
    commandResultNode->setNodeName(QStringLiteral("执行结果: 成功"));
    break;
  }
  case NXMessageBarType::Warning : {
    commandResultNode->setNodeName(QStringLiteral("执行结果: 警告"));
    break;
  }
  case NXMessageBarType::Error : {
    commandResultNode->setNodeName(QStringLiteral("执行结果: 错误"));
    break;
  }
  }
  m_CommanderNodeMap.insert(command, commandNode);
  _updateNodeModelIndex();
}

void NXActionCommanderModel::removeCommand(NXActionCommand *command)
{
  if (!command || !m_CommanderNodeMap.contains(command)) {
    return;
  }
  auto commandNode = m_CommanderNodeMap[command];
  m_CommanderNodeMap.remove(command);
  int removeRow = m_RootNode->getChildNodes().indexOf(commandNode);
  beginRemoveRows(m_RootNode->getModelIndex(), removeRow, removeRow);
  m_RootNode->removeChildNode(commandNode);
  endRemoveRows();
  delete commandNode;
  _updateNodeModelIndex();
}

void NXActionCommanderModel::updateCommand(NXActionCommand *command, bool isRedo)
{
  if (!command || !m_CommanderNodeMap.contains(command)) {
    return;
  }
  auto commandNode = m_CommanderNodeMap[command];
  commandNode->setMessageMode(command->getMessageMode());
  commandNode->setNodeName(QStringLiteral("%1 [%2] %3")
                               .arg(commandNode->getNodeIndexText())
                               .arg(commandNode->getNodeCreateTime())
                               .arg(command->getCommandDesc()));
  // 次要行
  auto commandTypeNode = commandNode->getChildNodes()[0];
  commandTypeNode->setNodeName(QStringLiteral("命令类型: %1").arg(command->metaObject()->className()));
  auto commandStateNode = commandNode->getChildNodes()[1];
  commandStateNode->setNodeName(
      QStringLiteral("命令状态: %1").arg(isRedo ? QStringLiteral("已执行") : QStringLiteral("已撤销")));
  auto commandResultNode = commandNode->getChildNodes()[2];
  switch (commandNode->getMessageMode()) {
  case NXMessageBarType::Information :
  case NXMessageBarType::Success     : {
    commandResultNode->setNodeName(QStringLiteral("执行结果: 成功"));
    break;
  }
  case NXMessageBarType::Warning : {
    commandResultNode->setNodeName(QStringLiteral("执行结果: 警告"));
    break;
  }
  case NXMessageBarType::Error : {
    commandResultNode->setNodeName(QStringLiteral("执行结果: 错误"));
    break;
  }
  }
}

void NXActionCommanderModel::clearCommand()
{
  beginResetModel();
  m_RootNode->setChildNodes({});
  for (auto const node : m_CommanderNodeMap) {
    delete node;
  }
  m_CommanderNodeMap.clear();
  endResetModel();
  _updateNodeModelIndex();
}

QModelIndex const &NXActionCommanderModel::getCommandModelIndex(NXActionCommand *command)
{
  static QModelIndex invalidModelIndex;
  if (!m_CommanderNodeMap.contains(command)) {
    return invalidModelIndex;
  }
  return m_CommanderNodeMap[command]->getModelIndex();
}

void NXActionCommanderModel::_updateNodeModelIndex(QModelIndex const &parentIndex)
{
  int indexRowCount = rowCount(parentIndex);
  for (int row = 0; row < indexRowCount; ++row) {
    QModelIndex modelIndex = index(row, 0, parentIndex);
    auto const childNode   = static_cast<NXActionCommanderNode *>(modelIndex.internalPointer());
    childNode->setModelIndex(modelIndex);
    if (hasChildren(modelIndex)) {
      _updateNodeModelIndex(modelIndex);
    }
  }
}
