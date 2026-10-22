#include "NXActionCommanderModel.h"

#include <QDebug>
#include <QTime>

#include "DeveloperComponents/NXActionCommanderNode.h"
#include "NXActionCommander.h"

NXActionCommanderModel::NXActionCommanderModel(QObject *parent)
    : QAbstractItemModel(parent)
{
  _rootNode = new NXActionCommanderNode(nullptr);
  _rootNode->setIsRootNode(true);
}

NXActionCommanderModel::~NXActionCommanderModel()
{
  delete _rootNode;
}

QModelIndex NXActionCommanderModel::parent(const QModelIndex &child) const
{
  if (!child.isValid()) {
    return {};
  }
  const auto childNode              = static_cast<NXActionCommanderNode *>(child.internalPointer());
  NXActionCommanderNode *parentNode = childNode->getParentNode();
  if (parentNode == _rootNode) {
    return {};
  }
  if (parentNode == nullptr) {
    return {};
  }
  return createIndex(parentNode->getRow(), 0, parentNode);
}

QModelIndex NXActionCommanderModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent)) {
    return {};
  }
  NXActionCommanderNode *parentNode;
  if (!parent.isValid()) {
    parentNode = _rootNode;
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

int NXActionCommanderModel::rowCount(const QModelIndex &parent) const
{
  NXActionCommanderNode *parentNode;
  if (parent.column() > 0) {
    return 0;
  }
  if (!parent.isValid()) {
    parentNode = _rootNode;
  } else {
    parentNode = static_cast<NXActionCommanderNode *>(parent.internalPointer());
  }
  if (!parentNode) {
    return 0;
  }
  return parentNode->getChildNodes().count();
}

int NXActionCommanderModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)
  return 1;
}

QVariant NXActionCommanderModel::data(const QModelIndex &index, int role) const
{
  if (index.column() == 0) {
    switch (role) {
    case Qt::DisplayRole : {
      if (const auto node = static_cast<NXActionCommanderNode *>(index.internalPointer())) {
        // 空出图标位置
        if (node->getIsCommandNode()) {
          return QStringLiteral("   ") + node->getNodeName();
        }
        return node->getNodeName();
      }
      return {};
    }
    case Qt::UserRole : {
      if (const auto node = static_cast<NXActionCommanderNode *>(index.internalPointer())) {
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
  if (!command || _commanderNodeMap.contains(command)) {
    return;
  }
  beginInsertRows(_rootNode->getModelIndex(), _commanderNodeMap.count(), _commanderNodeMap.count());
  auto commandNode = new NXActionCommanderNode(_rootNode);

  commandNode->setMessageMode(command->getMessageMode());
  commandNode->setIsCommandNode(true);
  int nodeIndex = 1;
  if (!_rootNode->getChildNodes().isEmpty()) {
    nodeIndex = _rootNode->getChildNodes().last()->getNodeIndex() + 1;
  }
  commandNode->setNodeIndex(nodeIndex);
  commandNode->setNodeIndexText(QStringLiteral("%1").arg(nodeIndex, 3, 10, QChar('0')));
  commandNode->setNodeCreateTime(QTime::currentTime().toString("hh:mm:ss"));
  // 主要行
  commandNode->setNodeName(QStringLiteral("%1 [%2] %3")
                               .arg(commandNode->getNodeIndexText())
                               .arg(commandNode->getNodeCreateTime())
                               .arg(command->getCommandDesc()));
  _rootNode->appendChildNode(commandNode);
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
  _commanderNodeMap.insert(command, commandNode);
  _updateNodeModelIndex();
}

void NXActionCommanderModel::removeCommand(NXActionCommand *command)
{
  if (!command || !_commanderNodeMap.contains(command)) {
    return;
  }
  auto commandNode = _commanderNodeMap[command];
  _commanderNodeMap.remove(command);
  int removeRow = _rootNode->getChildNodes().indexOf(commandNode);
  beginRemoveRows(_rootNode->getModelIndex(), removeRow, removeRow);
  _rootNode->removeChildNode(commandNode);
  endRemoveRows();
  delete commandNode;
  _updateNodeModelIndex();
}

void NXActionCommanderModel::updateCommand(NXActionCommand *command, bool isRedo)
{
  if (!command || !_commanderNodeMap.contains(command)) {
    return;
  }
  auto commandNode = _commanderNodeMap[command];
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
  _rootNode->setChildNodes({});
  for (const auto node : _commanderNodeMap) {
    delete node;
  }
  _commanderNodeMap.clear();
  endResetModel();
  _updateNodeModelIndex();
}

const QModelIndex &NXActionCommanderModel::getCommandModelIndex(NXActionCommand *command)
{
  static QModelIndex invalidModelIndex;
  if (!_commanderNodeMap.contains(command)) {
    return invalidModelIndex;
  }
  return _commanderNodeMap[command]->getModelIndex();
}

void NXActionCommanderModel::_updateNodeModelIndex(const QModelIndex &parentIndex)
{
  int indexRowCount = rowCount(parentIndex);
  for (int row = 0; row < indexRowCount; ++row) {
    QModelIndex modelIndex = index(row, 0, parentIndex);
    const auto childNode   = static_cast<NXActionCommanderNode *>(modelIndex.internalPointer());
    childNode->setModelIndex(modelIndex);
    if (hasChildren(modelIndex)) {
      _updateNodeModelIndex(modelIndex);
    }
  }
}
