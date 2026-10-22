#include "NXTabWidget.h"

#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>

#include "NXTabBar.h"
#include "private/NXTabWidgetPrivate.h"
Q_PROPERTY_CREATE_CPP(NXTabWidget, bool, IsTabTransparent)
Q_PROPERTY_CREATE_CPP(NXTabWidget, bool, IsContainerAcceptDrops)
Q_PROPERTY_CREATE_CPP(NXTabWidget, QSize, FloatWidgetSize)

NXTabWidget::NXTabWidget(QWidget *parent)
    : QTabWidget(parent)
    , d_ptr(new NXTabWidgetPrivate())
{
  Q_D(NXTabWidget);
  d->q_ptr                    = this;
  d->_pIsContainerAcceptDrops = false;
  d->_pIsTabTransparent       = false;
  d->_pFloatWidgetSize        = QSize(700, 500);
  setObjectName("NXTabWidget");
  setAcceptDrops(true);
  d->_tabBar = new NXTabBar(this);
  setTabBar(d->_tabBar);
  connect(d->_tabBar, &NXTabBar::tabDragCreate, d, &NXTabWidgetPrivate::onTabDragCreate);
  connect(d->_tabBar, &NXTabBar::tabDragEnter, d, &NXTabWidgetPrivate::onTabDragEnter);
  connect(d->_tabBar, &NXTabBar::tabDragLeave, d, &NXTabWidgetPrivate::onTabDragLeave);
  connect(d->_tabBar, &NXTabBar::tabDragDrop, d, &NXTabWidgetPrivate::onTabDragDrop);
  connect(d->_tabBar, &NXTabBar::tabCloseRequested, d, &NXTabWidgetPrivate::onTabCloseRequested);
  connect(d->_tabBar, &NXTabBar::currentChanged, this, [=](int index) {
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
  d->_tabBar->locateCurrentTab();
}

void NXTabWidget::setTabBarStyle(NXTabBarType::TabBarStyle style)
{
  Q_D(NXTabWidget);
  if (d->_tabBar) {
    d->_tabBar->setTabBarStyle(style);
  }
  if (d->_customTabBar) {
    d->_customTabBar->setTabBarStyle(style);
  }
}

NXTabBarType::TabBarStyle NXTabWidget::getTabBarStyle() const
{
  Q_D(const NXTabWidget);
  return d->_tabBar->getTabBarStyle();
}

void NXTabWidget::setTabCornerRadius(int radius)
{
  Q_D(NXTabWidget);
  if (d->_tabBar) {
    d->_tabBar->setTabCornerRadius(radius);
  }
  if (d->_customTabBar) {
    d->_customTabBar->setTabCornerRadius(radius);
  }
}

int NXTabWidget::getTabCornerRadius() const
{
  Q_D(const NXTabWidget);
  return d->_tabBar->getTabCornerRadius();
}

void NXTabWidget::setIsSelectedIndicatorVisible(bool isVisible)
{
  Q_D(NXTabWidget);
  if (d->_tabBar) {
    d->_tabBar->setIsSelectedIndicatorVisible(isVisible);
  }
  if (d->_customTabBar) {
    d->_customTabBar->setIsSelectedIndicatorVisible(isVisible);
  }
}

bool NXTabWidget::getIsSelectedIndicatorVisible() const
{
  Q_D(const NXTabWidget);
  return d->_tabBar->getIsSelectedIndicatorVisible();
}

void NXTabWidget::setTabSize(QSize tabSize)
{
  Q_D(NXTabWidget);
  d->_tabBar->setTabSize(tabSize);
}

QSize NXTabWidget::getTabSize() const
{
  Q_D(const NXTabWidget);
  return d->_tabBar->getTabSize();
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
  if (!d->_pIsTabTransparent) {
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
  if (d->_pIsContainerAcceptDrops && event->mimeData()->property("NXTabWidgetObject").value<NXTabWidget *>() != this) {
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
