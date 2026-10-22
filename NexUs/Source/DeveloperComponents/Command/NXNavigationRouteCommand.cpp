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
  _pNavigationBar->navigation(_pUndoPageKey, false, false);
  setCommandDesc(QStringLiteral("撤销路由跳转操作"));
}

void NXNavigationRouteCommand::redo()
{
  _pNavigationBar->navigation(_pRedoPageKey, false, false);
  setCommandDesc(QStringLiteral("执行路由跳转操作"));
}
