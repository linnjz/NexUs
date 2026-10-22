#include "NXWindowStackChangeCommand.h"

#include "NXCentralStackedWidget.h"
#include "NXWindow.h"

NXWindowStackChangeCommand::NXWindowStackChangeCommand(QObject *parent)
    : NXActionCommand(parent)
{
}

NXWindowStackChangeCommand::~NXWindowStackChangeCommand()
{
}

void NXWindowStackChangeCommand::undo()
{
  _pWindowPrivate->_centerStackedWidget->doWindowStackSwitch(_pWindowPrivate->_pStackSwitchMode, _pUndoStackIndex, false);
  Q_EMIT _pWindowPrivate->q_ptr->pCurrentStackIndexChanged();
  setCommandDesc(QStringLiteral("撤销堆栈页面切换操作"));
}

void NXWindowStackChangeCommand::redo()
{
  _pWindowPrivate->_centerStackedWidget->doWindowStackSwitch(_pWindowPrivate->_pStackSwitchMode, _pRedoStackIndex, false);
  Q_EMIT _pWindowPrivate->q_ptr->pCurrentStackIndexChanged();
  setCommandDesc(QStringLiteral("执行堆栈页面切换操作"));
}
