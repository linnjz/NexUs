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
  d->m_BreadcrumbBar = new NXBreadcrumbBar(this);
  d->m_BreadcrumbBar->setTextPixelSize(28);
  connect(d->m_BreadcrumbBar, &NXBreadcrumbBar::breadcrumbClicked, this, [=](QString breadcrumb, QStringList lastBreadcrumbList) {
    if (d->m_CentralWidgetMap.contains(breadcrumb)) {
      int widgetIndex = d->m_CentralWidgetMap.value(breadcrumb);
      d->_switchCentralStackIndex(widgetIndex, d->m_NavigationTargetIndex);
      d->m_NavigationTargetIndex = widgetIndex;
      QVariantMap routeData     = QVariantMap();
      routeData.insert(QStringLiteral("NXScrollPageCheckSumKey"), QStringLiteral("BreadcrumbClicked"));
      routeData.insert(QStringLiteral("NXBackBreadcrumbList"), lastBreadcrumbList);
      routeData.insert(QStringLiteral("NXForwardBreadcrumbList"), d->m_BreadcrumbBar->getBreadcrumbList());
      NXNavigationRouter::getInstance()->navigationRoute(window(), d, QStringLiteral("onNavigationRoute"), routeData);
    }
  });
  d->m_PageTitleLayout = new QHBoxLayout();
  d->m_PageTitleLayout->setContentsMargins(0, 0, 0, 0);
  d->m_PageTitleLayout->addWidget(d->m_BreadcrumbBar);

  d->m_CentralStackedWidget = new QStackedWidget(this);
  d->m_CentralStackedWidget->setContentsMargins(0, 0, 0, 0);

  d->m_MainLayout = new QVBoxLayout(this);
  d->m_MainLayout->setSpacing(0);
  d->m_MainLayout->setContentsMargins(0, 0, 0, 0);
  d->m_MainLayout->addLayout(d->m_PageTitleLayout);
  d->m_TopCustomWidget = new QWidget(this);
  d->m_TopCustomWidget->setVisible(false);
  d->m_MainLayout->addWidget(d->m_TopCustomWidget);
  d->m_MainLayout->addWidget(d->m_CentralStackedWidget);
  d->m_BottomCustomWidget = new QWidget(this);
  d->m_BottomCustomWidget->setVisible(false);
  d->m_MainLayout->addWidget(d->m_BottomCustomWidget);
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
  if (d->m_TopCustomWidget) {
    d->m_MainLayout->removeWidget(d->m_TopCustomWidget);
  }
  d->m_MainLayout->insertWidget(1, customWidget);
  d->m_TopCustomWidget = customWidget;
}

QWidget *NXScrollPage::getTopCustomWidget() const
{
  Q_CONST_D(NXScrollPage);
  return d->m_TopCustomWidget;
}

void NXScrollPage::setBottomCustomWidget(QWidget *customWidget)
{
  Q_D(NXScrollPage);
  if (!customWidget || customWidget == this) {
    return;
  }
  if (d->m_BottomCustomWidget) {
    d->m_MainLayout->removeWidget(d->m_BottomCustomWidget);
  }
  d->m_MainLayout->insertWidget(3, customWidget);
  d->m_BottomCustomWidget = customWidget;
}

QWidget *NXScrollPage::getBottomCustomWidget() const
{
  Q_CONST_D(NXScrollPage);
  return d->m_BottomCustomWidget;
}

void NXScrollPage::addCentralWidget(QWidget *centralWidget, bool isWidgetResizeable, bool isVerticalGrabGesture, qreal mousePressEventDelay, Qt::ScrollBarPolicy vScrollBarPolicy, Qt::ScrollBarPolicy hScrollBarPolicy)
{
  Q_D(NXScrollPage);
  if (!centralWidget) {
    return;
  }
  if (centralWidget->windowTitle().isEmpty()) {
    centralWidget->setWindowTitle(QString(QStringLiteral("Page_%1")).arg(d->m_CentralStackedWidget->count()));
  }
  if (d->m_CentralStackedWidget->count() == 0) {
    d->m_BreadcrumbBar->appendBreadcrumb(centralWidget->windowTitle());
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

  d->m_CentralWidgetMap.insert(centralWidget->windowTitle(), d->m_CentralStackedWidget->count());
  d->m_CentralStackedWidget->addWidget(scrollArea);
}

void NXScrollPage::setPageTitle(QString const &title)
{
  Q_D(NXScrollPage);
  QStringList breadcrumbList = d->m_BreadcrumbBar->getBreadcrumbList();
  if (!breadcrumbList.isEmpty()) {
    QString oldTitle  = breadcrumbList.first();
    breadcrumbList[0] = title;
    d->m_BreadcrumbBar->setBreadcrumbList(breadcrumbList);
    if (d->m_CentralWidgetMap.contains(oldTitle)) {
      int index = d->m_CentralWidgetMap.take(oldTitle);
      d->m_CentralWidgetMap.insert(title, index);
    }
  }
}

void NXScrollPage::navigation(int widgetIndex, bool isLogRoute)
{
  Q_D(NXScrollPage);
  if (widgetIndex >= d->m_CentralStackedWidget->count() || d->m_NavigationTargetIndex == widgetIndex) {
    return;
  }
  d->_switchCentralStackIndex(widgetIndex, d->m_NavigationTargetIndex);
  d->m_NavigationTargetIndex = widgetIndex;
  QString pageTitle         = d->m_CentralWidgetMap.key(widgetIndex);
  if (isLogRoute) {
    QVariantMap routeData = QVariantMap();
    routeData.insert(QStringLiteral("NXScrollPageCheckSumKey"), QStringLiteral("Navigation"));
    QStringList breadcrumbList = d->m_BreadcrumbBar->getBreadcrumbList();
    routeData.insert(QStringLiteral("NXBackPageTitle"), breadcrumbList.last());
    routeData.insert(QStringLiteral("NXForwardPageTitle"), pageTitle);
    NXNavigationRouter::getInstance()->navigationRoute(d, QStringLiteral("onNavigationRoute"), routeData);
  }
  d->m_BreadcrumbBar->appendBreadcrumb(pageTitle);
}

void NXScrollPage::setPageTitleSpacing(int spacing)
{
  Q_D(NXScrollPage);
  d->m_PageTitleLayout->takeAt(0);
  d->m_PageTitleLayout->addSpacing(spacing);
  d->m_PageTitleLayout->addWidget(d->m_BreadcrumbBar);
}

int NXScrollPage::getPageTitleSpacing() const
{
  return d_ptr->m_PageTitleSpacing;
}

void NXScrollPage::setTitleVisible(bool isVisible)
{
  Q_D(NXScrollPage);
  d->m_BreadcrumbBar->setVisible(isVisible);
}

void NXScrollPage::setPageTitlePixelSize(int pixelSize)
{
  Q_D(NXScrollPage);
  d->m_BreadcrumbBar->setTextPixelSize(pixelSize);
}

int NXScrollPage::getPageTitlePixelSize() const
{
  Q_CONST_D(NXScrollPage);
  return d->m_BreadcrumbBar->getTextPixelSize();
}
