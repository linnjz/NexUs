#include "NXScrollPageRouteCommand.h"

#include "NXBreadcrumbBar.h"
#include "NXScrollPage.h"
#include "private/NXScrollPagePrivate.h"

NXScrollPageRouteCommand::NXScrollPageRouteCommand(QObject *parent)
    : NXActionCommand(parent)
{
  _pIsBreadcrumbClicked = false;
}

NXScrollPageRouteCommand::~NXScrollPageRouteCommand()
{
}

void NXScrollPageRouteCommand::undo()
{
  if (_pIsBreadcrumbClicked) {
    int widgetIndex = _pScrollPagePrivate->_centralWidgetMap.value(_pUndoBreadcrumbList.last());
    _pScrollPagePrivate->_switchCentralStackIndex(widgetIndex, _pScrollPagePrivate->_navigationTargetIndex);
    _pScrollPagePrivate->_navigationTargetIndex = widgetIndex;
    _pScrollPagePrivate->_breadcrumbBar->setBreadcrumbList(_pUndoBreadcrumbList);
  } else {
    _pScrollPagePrivate->q_ptr->navigation(_pUndoPageIndex, false);
  }
  setCommandDesc(QStringLiteral("撤销页面切换操作"));
}

void NXScrollPageRouteCommand::redo()
{
  if (_pIsBreadcrumbClicked) {
    int widgetIndex = _pScrollPagePrivate->_centralWidgetMap.value(_pRedoBreadcrumbList.last());
    _pScrollPagePrivate->_switchCentralStackIndex(widgetIndex, _pScrollPagePrivate->_navigationTargetIndex);
    _pScrollPagePrivate->_navigationTargetIndex = widgetIndex;
    _pScrollPagePrivate->_breadcrumbBar->setBreadcrumbList(_pRedoBreadcrumbList);
  } else {
    _pScrollPagePrivate->q_ptr->navigation(_pRedoPageIndex, false);
  }
  setCommandDesc(QStringLiteral("执行页面切换操作"));
}
