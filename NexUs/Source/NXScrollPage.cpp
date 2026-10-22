#include "NXScrollPage.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "NXBreadcrumbBar.h"
#include "NXNavigationRouter.h"
#include "NXScrollArea.h"
#include "NXScrollBar.h"
#include "private/NXScrollPagePrivate.h"

NXScrollPage::NXScrollPage(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXScrollPagePrivate())
{
  Q_D(NXScrollPage);
  setProperty("NXBaseClassName", "NXScrollPage");
  d->q_ptr          = this;
  d->_breadcrumbBar = new NXBreadcrumbBar(this);
  d->_breadcrumbBar->setTextPixelSize(28);
  connect(d->_breadcrumbBar, &NXBreadcrumbBar::breadcrumbClicked, this, [=](QString breadcrumb, QStringList lastBreadcrumbList) {
    if (d->_centralWidgetMap.contains(breadcrumb)) {
      int widgetIndex = d->_centralWidgetMap.value(breadcrumb);
      d->_switchCentralStackIndex(widgetIndex, d->_navigationTargetIndex);
      d->_navigationTargetIndex = widgetIndex;
      QVariantMap routeData     = QVariantMap();
      routeData.insert(QStringLiteral("NXScrollPageCheckSumKey"), QStringLiteral("BreadcrumbClicked"));
      routeData.insert(QStringLiteral("NXBackBreadcrumbList"), lastBreadcrumbList);
      routeData.insert(QStringLiteral("NXForwardBreadcrumbList"), d->_breadcrumbBar->getBreadcrumbList());
      NXNavigationRouter::getInstance()->navigationRoute(window(), d, QStringLiteral("onNavigationRoute"), routeData);
    }
  });
  d->_pageTitleLayout = new QHBoxLayout();
  d->_pageTitleLayout->setContentsMargins(0, 0, 0, 0);
  d->_pageTitleLayout->addWidget(d->_breadcrumbBar);

  d->_centralStackedWidget = new QStackedWidget(this);
  d->_centralStackedWidget->setContentsMargins(0, 0, 0, 0);

  d->_mainLayout = new QVBoxLayout(this);
  d->_mainLayout->setSpacing(0);
  d->_mainLayout->setContentsMargins(0, 0, 0, 0);
  d->_mainLayout->addLayout(d->_pageTitleLayout);
  d->_topCustomWidget = new QWidget(this);
  d->_topCustomWidget->setVisible(false);
  d->_mainLayout->addWidget(d->_topCustomWidget);
  d->_mainLayout->addWidget(d->_centralStackedWidget);
  d->_bottomCustomWidget = new QWidget(this);
  d->_bottomCustomWidget->setVisible(false);
  d->_mainLayout->addWidget(d->_bottomCustomWidget);
  setContentsMargins(20, 20, 0, 0);
}

NXScrollPage::~NXScrollPage()
{
}

void NXScrollPage::setTopCustomWidget(QWidget *customWidget)
{
  Q_D(NXScrollPage);
  if (!customWidget || customWidget == this) {
    return;
  }
  if (d->_topCustomWidget) {
    d->_mainLayout->removeWidget(d->_topCustomWidget);
  }
  d->_mainLayout->insertWidget(1, customWidget);
  d->_topCustomWidget = customWidget;
}

QWidget * NXScrollPage::getTopCustomWidget() const
{
  Q_D(const NXScrollPage);
  return d->_topCustomWidget;
}

void NXScrollPage::setBottomCustomWidget(QWidget *customWidget)
{
  Q_D(NXScrollPage);
  if (!customWidget || customWidget == this) {
    return;
  }
  if (d->_bottomCustomWidget) {
    d->_mainLayout->removeWidget(d->_bottomCustomWidget);
  }
  d->_mainLayout->insertWidget(3, customWidget);
  d->_bottomCustomWidget = customWidget;
}

QWidget * NXScrollPage::getBottomCustomWidget() const
{
  Q_D(const NXScrollPage);
  return d->_bottomCustomWidget;
}

void NXScrollPage::addCentralWidget(QWidget *centralWidget, bool isWidgetResizeable, bool isVerticalGrabGesture, qreal mousePressEventDelay, Qt::ScrollBarPolicy vScrollBarPolicy, Qt::ScrollBarPolicy hScrollBarPolicy)
{
  Q_D(NXScrollPage);
  if (!centralWidget) {
    return;
  }
  if (centralWidget->windowTitle().isEmpty()) {
    centralWidget->setWindowTitle(QString(QStringLiteral("Page_%1")).arg(d->_centralStackedWidget->count()));
  }
  if (d->_centralStackedWidget->count() == 0) {
    d->_breadcrumbBar->appendBreadcrumb(centralWidget->windowTitle());
  }
  NXScrollArea *scrollArea = new NXScrollArea(this);
  scrollArea->setMouseTracking(true);
  scrollArea->setIsAnimation(Qt::Vertical, true);
  scrollArea->setWidgetResizable(isWidgetResizeable);
  scrollArea->setIsGrabGesture(isVerticalGrabGesture, mousePressEventDelay);
  scrollArea->setIsOverShoot(Qt::Vertical, true);
  scrollArea->setVerticalScrollBarPolicy(vScrollBarPolicy);
  scrollArea->setHorizontalScrollBarPolicy(hScrollBarPolicy);
  NXScrollBar *floatVScrollBar = new NXScrollBar(scrollArea->verticalScrollBar(), scrollArea);
  floatVScrollBar->setIsAnimation(true);

  QWidget *scrollPageContainer = new QWidget(this);
  scrollPageContainer->setObjectName("NXScrollPageContainer");
  scrollPageContainer->setStyleSheet(QStringLiteral("#NXScrollPageContainer{background-color:transparent;}"));
  QVBoxLayout *scrollPageContainerLayout = new QVBoxLayout(scrollPageContainer);
  scrollPageContainerLayout->setContentsMargins(0, 0, 0, 0);
  scrollPageContainerLayout->addWidget(centralWidget);
  scrollArea->setWidget(scrollPageContainer);

  d->_centralWidgetMap.insert(centralWidget->windowTitle(), d->_centralStackedWidget->count());
  d->_centralStackedWidget->addWidget(scrollArea);
}

void NXScrollPage::setPageTitle(const QString &title)
{
  Q_D(NXScrollPage);
  QStringList breadcrumbList = d->_breadcrumbBar->getBreadcrumbList();
  if (!breadcrumbList.isEmpty()) {
    QString oldTitle  = breadcrumbList.first();
    breadcrumbList[0] = title;
    d->_breadcrumbBar->setBreadcrumbList(breadcrumbList);
    if (d->_centralWidgetMap.contains(oldTitle)) {
      int index = d->_centralWidgetMap.take(oldTitle);
      d->_centralWidgetMap.insert(title, index);
    }
  }
}

void NXScrollPage::navigation(int widgetIndex, bool isLogRoute)
{
  Q_D(NXScrollPage);
  if (widgetIndex >= d->_centralStackedWidget->count() || d->_navigationTargetIndex == widgetIndex) {
    return;
  }
  d->_switchCentralStackIndex(widgetIndex, d->_navigationTargetIndex);
  d->_navigationTargetIndex = widgetIndex;
  QString pageTitle         = d->_centralWidgetMap.key(widgetIndex);
  if (isLogRoute) {
    QVariantMap routeData = QVariantMap();
    routeData.insert(QStringLiteral("NXScrollPageCheckSumKey"), QStringLiteral("Navigation"));
    QStringList breadcrumbList = d->_breadcrumbBar->getBreadcrumbList();
    routeData.insert(QStringLiteral("NXBackPageTitle"), breadcrumbList.last());
    routeData.insert(QStringLiteral("NXForwardPageTitle"), pageTitle);
    NXNavigationRouter::getInstance()->navigationRoute(d, QStringLiteral("onNavigationRoute"), routeData);
  }
  d->_breadcrumbBar->appendBreadcrumb(pageTitle);
}

void NXScrollPage::setPageTitleSpacing(int spacing)
{
  Q_D(NXScrollPage);
  d->_pageTitleLayout->takeAt(0);
  d->_pageTitleLayout->addSpacing(spacing);
  d->_pageTitleLayout->addWidget(d->_breadcrumbBar);
}

int NXScrollPage::getPageTitleSpacing() const
{
  return d_ptr->_pageTitleSpacing;
}

void NXScrollPage::setTitleVisible(bool isVisible)
{
  Q_D(NXScrollPage);
  d->_breadcrumbBar->setVisible(isVisible);
}

void NXScrollPage::setPageTitlePixelSize(int pixelSize)
{
  Q_D(NXScrollPage);
  d->_breadcrumbBar->setTextPixelSize(pixelSize);
}

int NXScrollPage::getPageTitlePixelSize() const
{
  Q_D(const NXScrollPage);
  return d->_breadcrumbBar->getTextPixelSize();
}
