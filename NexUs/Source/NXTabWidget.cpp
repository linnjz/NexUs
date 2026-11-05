#include "NXTabWidget.h"

#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>

#include "NXTabBar.h"
#include "private/NXTabWidgetPrivate.h"
Q_PROPERTY_CREATE_CPP(NXTabWidget, QPS_PAEMIT(bool), IsTabTransparent)
Q_PROPERTY_CREATE_CPP(NXTabWidget, QPS_PAEMIT(bool), IsContainerAcceptDrops)
Q_PROPERTY_CREATE_CPP(NXTabWidget, QPS_PAEMIT(QSize), FloatWidgetSize)

NXTabWidget::NXTabWidget(QWidget *parent)
    : QTabWidget(parent)
    , d_ptr(new NXTabWidgetPrivate())
{
  Q_D(NXTabWidget);
  d->q_ptr                    = this;
  d->m_IsContainerAcceptDrops = false;
  d->m_IsTabTransparent       = false;
  d->m_FloatWidgetSize        = QSize(700, 500);
  setObjectName("NXTabWidget");
  setAcceptDrops(true);
  d->m_TabBar = new NXTabBar(this);
  setTabBar(d->m_TabBar);
  connect(d->m_TabBar, &NXTabBar::tabDragCreate, d, &NXTabWidgetPrivate::onTabDragCreate);
  connect(d->m_TabBar, &NXTabBar::tabDragEnter, d, &NXTabWidgetPrivate::onTabDragEnter);
  connect(d->m_TabBar, &NXTabBar::tabDragLeave, d, &NXTabWidgetPrivate::onTabDragLeave);
  connect(d->m_TabBar, &NXTabBar::tabDragDrop, d, &NXTabWidgetPrivate::onTabDragDrop);
  connect(d->m_TabBar, &NXTabBar::tabCloseRequested, d, &NXTabWidgetPrivate::onTabCloseRequested);
  connect(d->m_TabBar, &NXTabBar::currentChanged, this, [=](int index) {
    if (index < 0) {
      return;
    }
    Q_EMIT currentWidgetChanged(widget(index));
  });
}

NXTabWidget::~NXTabWidget()
{
  Q_D(NXTabWidget);
  d->_clearAllTabWidgetList();
}

void NXTabWidget::locateCurrentTab()
{
  Q_D(NXTabWidget);
  d->m_TabBar->locateCurrentTab();
}

void NXTabWidget::setTabBarStyle(NXTabBarType::TabBarStyle style)
{
  Q_D(NXTabWidget);
  if (d->m_TabBar) {
    d->m_TabBar->setTabBarStyle(style);
  }
  if (d->m_CustomTabBar) {
    d->m_CustomTabBar->setTabBarStyle(style);
  }
}

NXTabBarType::TabBarStyle NXTabWidget::getTabBarStyle() const noexcept(std::is_nothrow_copy_constructible_v<NXTabBarType::TabBarStyle>)
{
  Q_CONST_D(NXTabWidget);
  return d->m_TabBar->getTabBarStyle();
}

void NXTabWidget::setTabCornerRadius(int radius)
{
  Q_D(NXTabWidget);
  if (d->m_TabBar) {
    d->m_TabBar->setTabCornerRadius(radius);
  }
  if (d->m_CustomTabBar) {
    d->m_CustomTabBar->setTabCornerRadius(radius);
  }
}

int NXTabWidget::getTabCornerRadius() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXTabWidget);
  return d->m_TabBar->getTabCornerRadius();
}

void NXTabWidget::setIsSelectedIndicatorVisible(bool isVisible)
{
  Q_D(NXTabWidget);
  if (d->m_TabBar) {
    d->m_TabBar->setIsSelectedIndicatorVisible(isVisible);
  }
  if (d->m_CustomTabBar) {
    d->m_CustomTabBar->setIsSelectedIndicatorVisible(isVisible);
  }
}

bool NXTabWidget::getIsSelectedIndicatorVisible() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXTabWidget);
  return d->m_TabBar->getIsSelectedIndicatorVisible();
}

void NXTabWidget::setTabSize(QSize tabSize)
{
  Q_D(NXTabWidget);
  d->m_TabBar->setTabSize(tabSize);
}

QSize NXTabWidget::getTabSize() const noexcept(std::is_nothrow_copy_constructible_v<QSize>)
{
  Q_CONST_D(NXTabWidget);
  return d->m_TabBar->getTabSize();
}

void NXTabWidget::setTabPosition(TabPosition position)
{
  if (position == QTabWidget::North || position == QTabWidget::South) {
    QTabWidget::setTabPosition(position);
  }
}

void NXTabWidget::paintEvent(QPaintEvent *event)
{
  Q_D(NXTabWidget);
  if (!d->m_IsTabTransparent) {
    QTabWidget::paintEvent(event);
  }
}

void NXTabWidget::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->property("DragType").toString() == QStringLiteral("NXTabBarDrag")) {
    event->acceptProposedAction();
  }
  QTabWidget::dragEnterEvent(event);
}

void NXTabWidget::dropEvent(QDropEvent *event)
{
  Q_D(NXTabWidget);
  if (d->m_IsContainerAcceptDrops && event->mimeData()->property("NXTabWidgetObject").value<NXTabWidget *>() != this) {
    QMimeData *data = const_cast<QMimeData *>(event->mimeData());
    data->setProperty("TabDropIndex", count());
    d->onTabDragDrop(data);
    event->accept();
  }
  QTabWidget::dropEvent(event);
}

void NXTabWidget::tabInserted(int index)
{
  Q_D(NXTabWidget);
  QWidget *tabWidget = widget(index);
  if (tabWidget && !tabWidget->property("IsMetaWidget").toBool() &&
      !tabWidget->property("ElaOriginTabWidget").isValid()) {
    d->_addTabWidgetToList(tabWidget);
  }
  QTabWidget::tabInserted(index);
}
