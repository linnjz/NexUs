#include "NXNavigationRouteCommand.h"

#include "NXNavigationBar.h"

NXNavigationRouteCommand::NXNavigationRouteCommand(QObject *parent)
    : NXActionCommand(parent)
{
}

NXNavigationRouteCommand::~NXNavigationRouteCommand()
{
}

void NXNavigationRouteCommand::undo()
{
  m_NavigationBar->navigation(m_UndoPageKey, false, false);
  setCommandDesc(QStringLiteral("撤销路由跳转操作"));
}

void NXNavigationRouteCommand::redo()
{
  m_NavigationBar->navigation(m_RedoPageKey, false, false);
  setCommandDesc(QStringLiteral("执行路由跳转操作"));
}
