#include "NXActionCommanderPrivate.h"

#include "DeveloperComponents/NXActionCommanderModel.h"
#include "DeveloperComponents/NXActionCommanderView.h"
#include "NXActionCommander.h"
#include "NXMessageBar.h"

NXActionCommanderPrivate::NXActionCommanderPrivate(QObject *parent)
    : QObject { parent }
{
}

NXActionCommanderPrivate::~NXActionCommanderPrivate()
{
}

void NXActionCommanderPrivate::_showCommandDesc(const NXActionCommand *command, bool isRedo, bool isExecute) const
{
  if (!command->getIsShowMessage() || !_pIsMessageDisplayEnable) {
    return;
  }
  QString commandTitle = isExecute ? QStringLiteral("执行") : isRedo ? QStringLiteral("重做")
                                                                     : QStringLiteral("撤回");
  switch (command->getMessageMode()) {
  case NXMessageBarType::Success : {
    NXMessageBar::success(NXMessageBarType::TopRight, commandTitle, command->getCommandDesc(), _pMessageDisplayMesc, command->getMessageWidget());
    break;
  }
  case NXMessageBarType::Warning : {
    NXMessageBar::warning(NXMessageBarType::TopRight, commandTitle, command->getCommandDesc(), _pMessageDisplayMesc, command->getMessageWidget());
    break;
  }
  case NXMessageBarType::Information : {
    NXMessageBar::information(NXMessageBarType::TopRight, commandTitle, command->getCommandDesc(), _pMessageDisplayMesc, command->getMessageWidget());
    break;
  }
  case NXMessageBarType::Error : {
    NXMessageBar::error(NXMessageBarType::TopRight, commandTitle, command->getCommandDesc(), _pMessageDisplayMesc, command->getMessageWidget());
    break;
  }
  }
}

bool NXActionCommanderPrivate::_executeCommand(const QString &domainName, NXActionCommand *command)
{
  Q_Q(NXActionCommander);
  auto &commandData = _commandDomainMap[domainName];
  if (commandData.isBusyMode) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("任务管理器繁忙中!"), _pMessageDisplayMesc, command->getMessageWidget());
    return false;
  }
  if (!command) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("命令对象不可用!"), _pMessageDisplayMesc, command->getMessageWidget());
    return false;
  }
  q->setCurrentDomain(domainName);
  command->setParent(q);
  q->clearCommand(domainName);
  command->redo();
  _showCommandDesc(command, true, true);
  return true;
}

bool NXActionCommanderPrivate::_recordCommand(const QString &domainName, NXActionCommand *command, bool isRedo)
{
  Q_Q(NXActionCommander);
  auto &commandData = _commandDomainMap[domainName];
  if (commandData.isBusyMode) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("任务管理器繁忙中!"), _pMessageDisplayMesc, command->getMessageWidget());
    return false;
  }
  if (!command) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("命令对象不可用!"), _pMessageDisplayMesc, command->getMessageWidget());
    return false;
  }
  if (!_isCommanderValid) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("当前不处于可编辑模式，仅支持运行命令!"), _pMessageDisplayMesc, command->getMessageWidget());
    return false;
  }
  q->setCurrentDomain(domainName);
  command->setParent(q);
  if (isRedo) {
    // 加入时Redo一次
    command->redo();
    _showCommandDesc(command, true, true);
    // 记录时执行失败则丢弃命令 后续撤回重做时失败不进行处理
    if (command->getMessageMode() == NXMessageBarType::Error) {
      delete command;
      return false;
    }
  }
  auto &commandList = commandData.commandList;
  auto commandModel = commandData.commanderModel;
  if (commandData.currentIndex <= 0 || commandList.count() == 0) {
    commandData.undoState = NXActionCommanderType::UndoValid;
    commandData.redoState = NXActionCommanderType::RedoInvalid;
    Q_EMIT q->commanderStateChanged(domainName, NXActionCommanderType::UndoValid);
    Q_EMIT q->commanderStateChanged(domainName, NXActionCommanderType::RedoInvalid);
  } else {
    // 超过最大命令数 则移除第一条命令
    if (commandList.count() >= _pMaxCommandCount) {
      auto deleteCommand = commandList[0];
      commandModel->removeCommand(deleteCommand);
      delete deleteCommand;
      commandList.removeFirst();
      commandData.currentIndex -= 1;
    }
  }
  // 当前索引不位于末尾 则清除索引后的数据
  if (commandData.currentIndex != commandList.count() - 1) {
    int deleteStartIndex = commandData.currentIndex + 1;
    int deleteCount      = commandList.count() - commandData.currentIndex - 1;
    int deleteEndIndex   = deleteStartIndex + deleteCount;
    for (int i = deleteStartIndex; i < deleteEndIndex; i++) {
      auto deleteCommand = commandList[i];
      commandModel->removeCommand(deleteCommand);
      delete deleteCommand;
    }
    commandList.remove(deleteStartIndex, deleteCount);
    if (commandData.currentIndex > 0) {
      commandData.redoState = NXActionCommanderType::RedoInvalid;
      Q_EMIT q->commanderStateChanged(domainName, NXActionCommanderType::RedoInvalid);
    }
  }
  commandList.append(command);
  commandData.currentIndex = commandList.count() - 1;
  commandModel->addCommand(command);
  _commanderView->clearSelection();
  _commanderView->selectionModel()->select(commandModel->getCommandModelIndex(command), QItemSelectionModel::Select | QItemSelectionModel::Rows);
  return true;
}

void NXActionCommanderPrivate::_undoCommand(const QString &domainName, bool isShowMessage)
{
  Q_Q(NXActionCommander);
  auto &commandData = _commandDomainMap[domainName];
  if (commandData.undoState == NXActionCommanderType::UndoInvalid) {
    return;
  }
  auto &commandList = commandData.commandList;
  if (commandList.isEmpty()) {
    return;
  }
  if (commandData.currentIndex == 0) {
    commandData.undoState = NXActionCommanderType::UndoInvalid;
    Q_EMIT q->commanderStateChanged(domainName, NXActionCommanderType::UndoInvalid);
  }
  if (commandData.currentIndex == commandList.size() - 1) {
    commandData.redoState = NXActionCommanderType::RedoValid;
    Q_EMIT q->commanderStateChanged(domainName, NXActionCommanderType::RedoValid);
  }
  auto command = commandList[commandData.currentIndex];
  command->undo();
  commandData.currentIndex -= 1;
  auto commandModel = commandData.commanderModel;
  commandModel->updateCommand(command, false);
  _commanderView->clearSelection();
  _commanderView->selectionModel()->select(commandModel->getCommandModelIndex(command), QItemSelectionModel::Select | QItemSelectionModel::Rows);
  _commanderView->viewport()->update();
  if (isShowMessage) {
    _showCommandDesc(command, false);
  }
}

void NXActionCommanderPrivate::_redoCommand(const QString &domainName, bool isShowMessage)
{
  Q_Q(NXActionCommander);
  auto &commandData = _commandDomainMap[domainName];
  auto &commandList = commandData.commandList;
  if (commandData.redoState == NXActionCommanderType::RedoInvalid) {
    return;
  }
  if (commandList.isEmpty()) {
    return;
  }
  if (commandData.currentIndex <= 0) {
    commandData.undoState = NXActionCommanderType::UndoValid;
    Q_EMIT q->commanderStateChanged(domainName, NXActionCommanderType::UndoValid);
  }
  if (commandData.currentIndex == commandList.size() - 2) {
    commandData.redoState = NXActionCommanderType::RedoInvalid;
    Q_EMIT q->commanderStateChanged(domainName, NXActionCommanderType::RedoInvalid);
  }
  if (commandData.currentIndex < commandList.size() - 1) {
    commandData.currentIndex += 1;
  }
  auto command = commandList[commandData.currentIndex];
  command->redo();
  auto commandModel = commandData.commanderModel;
  commandModel->updateCommand(command, true);
  _commanderView->viewport()->update();
  _commanderView->clearSelection();
  _commanderView->selectionModel()->select(commandModel->getCommandModelIndex(command), QItemSelectionModel::Select | QItemSelectionModel::Rows);
  if (isShowMessage) {
    _showCommandDesc(command, true);
  }
}

void NXActionCommanderPrivate::_initDomain(NXActionCommandDomainData &domainData) const
{
  if (domainData.commanderModel) {
    return;
  }
  domainData.commanderModel = new NXActionCommanderModel(_commanderDesign);
}
