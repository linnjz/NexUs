#include "NXScrollPageRouteCommand.h"

#include "NXBreadcrumbBar.h"
#include "NXScrollPage.h"
#include "private/NXScrollPagePrivate.h"

NXScrollPageRouteCommand::NXScrollPageRouteCommand(QObject *parent)
    : NXActionCommand(parent)
    , m_IsBreadcrumbClicked { false }
{
}

NXScrollPageRouteCommand::~NXScrollPageRouteCommand()
{
}

void NXScrollPageRouteCommand::undo()
{
  if (m_IsBreadcrumbClicked) {
    int widgetIndex = m_ScrollPagePrivate->m_CentralWidgetMap.value(m_UndoBreadcrumbList.last());
    m_ScrollPagePrivate->_switchCentralStackIndex(widgetIndex, m_ScrollPagePrivate->m_NavigationTargetIndex);
    m_ScrollPagePrivate->m_NavigationTargetIndex = widgetIndex;
    m_ScrollPagePrivate->m_BreadcrumbBar->setBreadcrumbList(m_UndoBreadcrumbList);
  } else {
    m_ScrollPagePrivate->q_ptr->navigation(m_UndoPageIndex, false);
  }
  setCommandDesc(QStringLiteral("撤销页面切换操作"));
}

void NXScrollPageRouteCommand::redo()
{
  if (m_IsBreadcrumbClicked) {
    int widgetIndex = m_ScrollPagePrivate->m_CentralWidgetMap.value(m_RedoBreadcrumbList.last());
    m_ScrollPagePrivate->_switchCentralStackIndex(widgetIndex, m_ScrollPagePrivate->m_NavigationTargetIndex);
    m_ScrollPagePrivate->m_NavigationTargetIndex = widgetIndex;
    m_ScrollPagePrivate->m_BreadcrumbBar->setBreadcrumbList(m_RedoBreadcrumbList);
  } else {
    m_ScrollPagePrivate->q_ptr->navigation(m_RedoPageIndex, false);
  }
  setCommandDesc(QStringLiteral("执行页面切换操作"));
}
