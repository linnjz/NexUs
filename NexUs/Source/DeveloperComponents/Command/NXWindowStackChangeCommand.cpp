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
  m_WindowPrivate->m_CenterStackedWidget->doWindowStackSwitch(m_WindowPrivate->m_StackSwitchMode, m_UndoStackIndex, false);
  Q_EMIT m_WindowPrivate->q_ptr->signalCurrentStackIndexChanged();
  setCommandDesc(QStringLiteral("撤销堆栈页面切换操作"));
}

void NXWindowStackChangeCommand::redo()
{
  m_WindowPrivate->m_CenterStackedWidget->doWindowStackSwitch(m_WindowPrivate->m_StackSwitchMode, m_RedoStackIndex, false);
  Q_EMIT m_WindowPrivate->q_ptr->signalCurrentStackIndexChanged();
  setCommandDesc(QStringLiteral("执行堆栈页面切换操作"));
}
