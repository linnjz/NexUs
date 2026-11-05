#include <QApplication>
#include "NXWindow.h"

#include <QDockWidget>
#include <QHBoxLayout>
#include <QMovie>
#include <QPainter>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QScreen>
#include <QStyleOption>
#include <QToolBar>
#include <QtMath>

#include "NXCentralStackedWidget.h"
#include "DeveloperComponents/NXWindowStyle.h"
#include "NXApplication.h"
#include "NXEventBus.h"
#include "NXMenu.h"
#include "NXNavigationBar.h"
#include "NXNavigationRouter.h"
#include "NXTheme.h"
#include "private/NXAppBarPrivate.h"
#include "private/NXNavigationBarPrivate.h"
#include "private/NXWindowPrivate.h"
Q_PROPERTY_CREATE_CPP(NXWindow, QPS_PAEMIT(int), ThemeChangeTime)
Q_PROPERTY_CREATE_CPP(NXWindow, QPS_PAEMIT(NXWindowType::StackSwitchMode), StackSwitchMode)
Q_TAKEOVER_NATIVEEVENT_CPP(NXWindow, d_func()->m_AppBar);

NXWindow::NXWindow(QWidget *parent)
    : QMainWindow { parent }
    , d_ptr(new NXWindowPrivate())
{
  Q_D(NXWindow);
  d->q_ptr = this;
#ifdef Q_OS_MACOS
  setAttribute(Qt::WA_Hover);
#endif
  d->m_StackSwitchMode = NXWindowType::StackSwitchMode::Popup;
  setProperty("NXBaseClassName", "NXWindow");
  resize(1020, 680); // 默认宽高

  d->m_ThemeChangeTime          = 700;
  d->m_NavigationBarDisplayMode = NXNavigationType::NavigationDisplayMode::Auto;

  // 自定义AppBar
  d->m_AppBar = new NXAppBar(this);
  d->m_AppBar->setWindowButtonFlag(NXAppBarType::NavigationButtonHint);
  connect(d->m_AppBar, &NXAppBar::routeBackButtonClicked, this, [this]() {
    NXNavigationRouter::getInstance()->navigationRouteBack(this);
  });
  connect(d->m_AppBar, &NXAppBar::routeForwardButtonClicked, this, [this]() {
    NXNavigationRouter::getInstance()->navigationRouteForward(this);
  });
  connect(d->m_AppBar, &NXAppBar::closeButtonClicked, this, &NXWindow::closeButtonClicked);
  // 导航栏
  d->m_NavigationBar = new NXNavigationBar(this);
  // 返回按钮状态变更（仅响应本窗口的路由事件）
  connect(NXNavigationRouter::getInstance(), &NXNavigationRouter::windowRouterStateChanged, d, [d, this](QObject *context, NXNavigationRouterType::RouteMode routeMode) {
    if (context == this) {
      d->onNavigationRouterStateChanged(routeMode);
    }
  });

  // 转发用户卡片点击信号
  connect(d->m_NavigationBar, &NXNavigationBar::userInfoCardClicked, this, &NXWindow::userInfoCardClicked);
  // 跳转处理
  connect(d->m_NavigationBar, &NXNavigationBar::navigationNodeClicked, d, &NXWindowPrivate::onNavigationNodeClicked);
  // 新增窗口
  connect(d->m_NavigationBar, &NXNavigationBar::navigationNodeAdded, d, &NXWindowPrivate::onNavigationNodeAdded);
  // 移除窗口
  connect(d->m_NavigationBar, &NXNavigationBar::navigationNodeRemoved, d, &NXWindowPrivate::onNavigationNodeRemoved);
  // 在新窗口打开
  connect(d->m_NavigationBar, &NXNavigationBar::pageOpenInNewWindow, this, &NXWindow::pageOpenInNewWindow);

  // 导航中心堆栈窗口
  d->m_NavigationCenterStackedWidget = new NXCentralStackedWidget(this);
  d->m_NavigationCenterStackedWidget->setContentsMargins(0, 0, 0, 0);
#ifdef Q_OS_MACOS
  d->m_NavigationCenterStackedWidget->installEventFilter(this);
#endif
  QWidget *navigationCentralWidget = new QWidget(this);
  navigationCentralWidget->setObjectName("NXWindowNavigationCentralWidget");
  navigationCentralWidget->setStyleSheet(
      QStringLiteral("#NXWindowNavigationCentralWidget{background-color:transparent;}"));
  navigationCentralWidget->installEventFilter(this);
  d->m_CenterLayout = new QHBoxLayout(navigationCentralWidget);
  d->m_CenterLayout->setSpacing(5);
  d->m_CenterLayout->addWidget(d->m_NavigationBar);
  d->m_CenterLayout->addWidget(d->m_NavigationCenterStackedWidget);
  d->m_CenterLayout->setContentsMargins(d->m_ContentsMargins, 0, 0, 0);

  // 事件总线
  d->m_FocusEvent = new NXEvent(QStringLiteral("WMWindowClicked"), QStringLiteral("onWMWindowClickedEvent"), d);
  d->m_FocusEvent->registerAndInit();

  // 导航栏操作
  connect(d->m_AppBar, &NXAppBar::navigationButtonClicked, d, &NXWindowPrivate::onNavigationButtonClicked);

  // 主题变更动画
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXWindowPrivate::onThemeModeChanged);
  connect(d->m_AppBar, &NXAppBar::themeChangeButtonClicked, d, &NXWindowPrivate::onThemeReadyChange);
  d->m_IsInitFinished = true;

  // 中心堆栈窗口
  d->m_CenterStackedWidget = new NXCentralStackedWidget(this);
  d->m_CenterStackedWidget->setIsTransparent(true);
  d->m_CenterStackedWidget->getContainerStackedWidget()->addWidget(navigationCentralWidget);
  setCentralWidget(d->m_CenterStackedWidget);
  setObjectName("NXWindow");
  setStyleSheet(QStringLiteral("#NXWindow{background-color:transparent;}"));
  setStyle(new NXWindowStyle(style()));

  nxApp->syncWindowDisplayMode(this);
  d->m_WindowDisplayMode = nxApp->getWindowDisplayMode();
  connect(nxApp, &NXApplication::signalWindowDisplayModeChanged, d, &NXWindowPrivate::onWindowDisplayModeChanged);

  d->m_WindowPaintMode = NXWindowType::PaintMode::Normal;
  d->m_LightWindowPix   = new QPixmap();
  d->m_DarkWindowPix    = new QPixmap();

  d->m_WindowPaintMovie = new QMovie(this);
  connect(d->m_WindowPaintMovie, &QMovie::frameChanged, this, [=]() {
    update();
  });
}

NXWindow::~NXWindow()
{
  Q_D(NXWindow);
  nxApp->syncWindowDisplayMode(this, false);
  //delete this->style();
  delete d->m_LightWindowPix;
  delete d->m_DarkWindowPix;
}

void NXWindow::setIsStayTop(bool isStayTop)
{
  Q_D(NXWindow);
  d->m_AppBar->setIsStayTop(isStayTop);
  Q_EMIT signalIsStayTopChanged();
}

bool NXWindow::getIsStayTop() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  return d_ptr->m_AppBar->getIsStayTop();
}

void NXWindow::setIsFixedSize(bool isFixedSize)
{
  Q_D(NXWindow);
  d->m_AppBar->setIsFixedSize(isFixedSize);
  Q_EMIT signalIsFixedSizeChanged();
}

bool NXWindow::getIsFixedSize() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  return d_ptr->m_AppBar->getIsFixedSize();
}

void NXWindow::setIsDefaultClosed(bool isDefaultClosed)
{
  Q_D(NXWindow);
  d->m_AppBar->setIsDefaultClosed(isDefaultClosed);
  Q_EMIT signalIsDefaultClosedChanged();
}

bool NXWindow::getIsDefaultClosed() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  return d_ptr->m_AppBar->getIsDefaultClosed();
}

void NXWindow::setAppBarHeight(int appBarHeight)
{
  Q_D(NXWindow);
  d->m_AppBar->setAppBarHeight(appBarHeight);
  Q_EMIT signalAppBarHeightChanged();
}

int NXWindow::getAppBarHeight() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXWindow);
  return d->m_AppBar->getAppBarHeight();
}

void NXWindow::setRibbonHeight(int ribbonHeight)
{
  Q_D(NXWindow);
  d->m_AppBar->setRibbonHeight(ribbonHeight);
  Q_EMIT signalRibbonHeightChanged();
}

int NXWindow::getRibbonHeight() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXWindow);
  return d->m_AppBar->getRibbonHeight();
}

void NXWindow::setCustomWidget(NXAppBarType::CustomArea customArea, QWidget *widget, QObject *hitTestObject, QString const &hitTestFunctionName)
{
  Q_D(NXWindow);
  d->m_AppBar->setCustomWidget(customArea, widget, hitTestObject, hitTestFunctionName);
  Q_EMIT customWidgetChanged();
}

QWidget *NXWindow::getCustomWidget(NXAppBarType::CustomArea customArea) const
{
  Q_CONST_D(NXWindow);
  return d->m_AppBar->getCustomWidget(customArea);
}

void NXWindow::setCentralCustomWidget(QWidget *customWidget)
{
  Q_D(NXWindow);
  d->m_NavigationCenterStackedWidget->setCustomWidget(customWidget);
  Q_EMIT centralCustomWidgetChanged();
}

QWidget *NXWindow::getCentralCustomWidget() const
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationCenterStackedWidget->getCustomWidget();
}

void NXWindow::setCustomMenu(QMenu *customMenu)
{
  Q_D(NXWindow);
  d->m_AppBar->setCustomMenu(customMenu);
  Q_EMIT customMenuChanged();
}

QMenu *NXWindow::getCustomMenu() const
{
  Q_CONST_D(NXWindow);
  return d->m_AppBar->getCustomMenu();
}

void NXWindow::setIsCentralStackedWidgetTransparent(bool isTransparent)
{
  Q_D(NXWindow);
  d->m_NavigationCenterStackedWidget->setIsTransparent(isTransparent);
}

bool NXWindow::getIsCentralStackedWidgetTransparent() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationCenterStackedWidget->getIsTransparent();
}

void NXWindow::setIsAllowPageOpenInNewWindow(bool isAllowPageOpenInNewWindow)
{
  Q_D(NXWindow);
  d->m_NavigationBar->setIsAllowPageOpenInNewWindow(isAllowPageOpenInNewWindow);
  Q_EMIT signalIsAllowPageOpenInNewWindowChanged();
}

bool NXWindow::getIsAllowPageOpenInNewWindow() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBar->getIsAllowPageOpenInNewWindow();
}

void NXWindow::setNavigationBarDisplayMode(NXNavigationType::NavigationDisplayMode displayMode)
{
  Q_D(NXWindow);
  d->m_NavigationBarDisplayMode       = displayMode;
  d->m_CurrentNavigationBarDisplayMode = displayMode;
  if (d->m_IsNavigationBarFloat) {
    d->m_IsNavigationDisplayModeChanged = true;
    d->m_IsNavigationBarFloat           = false;
    d->m_IsNavigationBarExpanded        = false;
    d->m_NavigationBar->setIsTransparent(true);
    d->_resetWindowLayout(false);
  }
  switch (displayMode) {
  case NXNavigationType::Auto : {
    d->_doNavigationDisplayModeChange();
    break;
  }
  case NXNavigationType::Minimal : {
    d->m_NavigationBar->setDisplayMode(NXNavigationType::Minimal, isVisible());
    break;
  }
  case NXNavigationType::Compact : {
    d->m_NavigationBar->setDisplayMode(NXNavigationType::Compact, isVisible());
    break;
  }
  case NXNavigationType::Maximal : {
    d->m_NavigationBar->setDisplayMode(NXNavigationType::Maximal, isVisible());
    break;
  }
  }
  Q_EMIT signalNavigationBarDisplayModeChanged();
}

NXNavigationType::NavigationDisplayMode NXWindow::getNavigationBarDisplayMode() const noexcept(std::is_nothrow_copy_constructible_v<NXNavigationType::NavigationDisplayMode>)
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBarDisplayMode;
}

void NXWindow::setNavigationBarWidth(int navigationBarWidth)
{
  Q_D(NXWindow);
  d->m_NavigationBar->setNavigationBarWidth(navigationBarWidth);
  Q_EMIT signalNavigationBarWidthChanged();
}

int NXWindow::getNavigationBarWidth() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBar->getNavigationBarWidth();
}

void NXWindow::setCurrentStackIndex(int currentStackIndex)
{
  Q_D(NXWindow);
  if (currentStackIndex >= d->m_CenterStackedWidget->getContainerStackedWidget()->count() || currentStackIndex < 0 ||
      currentStackIndex == d->m_CentralStackTargetIndex) {
    return;
  }
  d->m_CentralStackTargetIndex = currentStackIndex;
  QVariantMap routeData;
  int currentCenterStackedWidgetIndex = d->m_CenterStackedWidget->getContainerStackedWidget()->currentIndex();
  routeData.insert(QStringLiteral("NXBackCentralStackIndex"), currentCenterStackedWidgetIndex);
  routeData.insert(QStringLiteral("NXForwardCentralStackIndex"), currentStackIndex);
  NXNavigationRouter::getInstance()->navigationRoute(this, d, QStringLiteral("onNavigationRoute"), routeData);
  d->m_CenterStackedWidget->doWindowStackSwitch(d->m_StackSwitchMode, currentStackIndex, false);
  Q_EMIT signalCurrentStackIndexChanged();
}

int NXWindow::getCurrentStackIndex() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXWindow);
  return d->m_CenterStackedWidget->getContainerStackedWidget()->currentIndex();
}

void NXWindow::setWindowPaintMode(NXWindowType::PaintMode windowPaintMode)
{
  Q_D(NXWindow);
  if (d->m_WindowPaintMovie->state() == QMovie::Running) {
    d->m_WindowPaintMovie->stop();
  }
  if (windowPaintMode == NXWindowType::PaintMode::Movie) {
    d->m_WindowPaintMovie->setFileName(d->m_ThemeMode == NXThemeType::Light ? d->m_LightWindowMoviePath : d->m_DarkWindowMoviePath);
    d->m_WindowPaintMovie->start();
  }
  d->m_WindowPaintMode = windowPaintMode;
  update();
  Q_EMIT signalWindowPaintModeChanged();
}

NXWindowType::PaintMode NXWindow::getWindowPaintMode() const noexcept(std::is_nothrow_copy_constructible_v<NXWindowType::PaintMode>)
{
  Q_CONST_D(NXWindow);
  return d->m_WindowPaintMode;
}

void NXWindow::moveToCenter()
{
  if (isMaximized() || isFullScreen()) {
    return;
  }
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
  auto geometry = screen()->availableGeometry();
#else
  auto geometry = qApp->screenAt(this->geometry().center())->geometry();
#endif
  setGeometry((geometry.left() + geometry.right() - width()) / 2, (geometry.top() + geometry.bottom() - height()) / 2, width(), height());
}

void NXWindow::setIsNavigationBarEnable(bool isVisible)
{
  Q_D(NXWindow);
  d->m_IsNavigationEnable = isVisible;
  d->m_NavigationBar->setVisible(isVisible);
  d->m_CenterLayout->setContentsMargins(isVisible ? d->m_ContentsMargins : 0, 0, 0, 0);
  d->m_NavigationCenterStackedWidget->setIsHasRadius(isVisible);
}

bool NXWindow::getIsNavigationBarEnable() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  return d_ptr->m_IsNavigationEnable;
}

void NXWindow::setUserInfoCardVisible(bool isVisible)
{
  Q_D(NXWindow);
  d->m_NavigationBar->setUserInfoCardVisible(isVisible);
}

void NXWindow::setUserInfoCardPixmap(QPixmap const &pix)
{
  Q_D(NXWindow);
  d->m_NavigationBar->setUserInfoCardPixmap(pix);
}

void NXWindow::setUserInfoCardTitle(QString const &title)
{
  Q_D(NXWindow);
  d->m_NavigationBar->setUserInfoCardTitle(title);
}

void NXWindow::setUserInfoCardSubTitle(QString const &subTitle)
{
  Q_D(NXWindow);
  d->m_NavigationBar->setUserInfoCardSubTitle(subTitle);
}

QString NXWindow::addExpanderNode(QString const &expanderTitle, NXIconType::IconName awesome) const
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBar->addExpanderNode(expanderTitle, awesome);
}

NXNodeResultExpected NXWindow::addExpanderNode(QString const &expanderTitle, QString const &targetExpanderKey, NXIconType::IconName awesome) const
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBar->addExpanderNode(expanderTitle, targetExpanderKey, awesome);
}

NXNodeResultExpected NXWindow::addPageNode(QString const &pageTitle, QWidget *page, NXIconType::IconName awesome)
{
  Q_D(NXWindow);
  auto returnType = d->m_NavigationBar->addPageNode(pageTitle, page, awesome);
  if (returnType.has_value()) {
    d->m_PageMetaMap.insert(page->property("NXPageKey").toString(), page->metaObject());
  }
  return returnType;
}

NXNodeResultExpected NXWindow::addPageNode(QString const &pageTitle, QWidget *page, int keyPoints, NXIconType::IconName awesome)
{
  Q_D(NXWindow);
  auto returnType = d->m_NavigationBar->addPageNode(pageTitle, page, keyPoints, awesome);
  if (returnType.has_value()) {
    d->m_PageMetaMap.insert(page->property("NXPageKey").toString(), page->metaObject());
  }
  return returnType;
}

NXNodeResultExpected NXWindow::addPageNode(QString const &pageTitle, QWidget *page, QString const &targetExpanderKey, NXIconType::IconName awesome)
{
  Q_D(NXWindow);
  auto returnType = d->m_NavigationBar->addPageNode(pageTitle, page, targetExpanderKey, awesome);
  if (returnType.has_value()) {
    d->m_PageMetaMap.insert(page->property("NXPageKey").toString(), page->metaObject());
  }
  return returnType;
}

NXNodeResultExpected NXWindow::addPageNode(QString const &pageTitle, QWidget *page, QString const &targetExpanderKey, int keyPoints, NXIconType::IconName awesome)
{
  Q_D(NXWindow);
  auto returnType = d->m_NavigationBar->addPageNode(pageTitle, page, targetExpanderKey, keyPoints, awesome);
  if (returnType.has_value()) {
    d->m_PageMetaMap.insert(page->property("NXPageKey").toString(), page->metaObject());
  }
  return returnType;
}

NXNodeResultExpected NXWindow::addFooterNode(QString const &footerTitle, int keyPoints, NXIconType::IconName awesome) const
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBar->addFooterNode(footerTitle, nullptr, keyPoints, awesome);
}

NXNodeResultExpected NXWindow::addFooterNode(QString const &footerTitle, QWidget *page, int keyPoints, NXIconType::IconName awesome)
{
  Q_D(NXWindow);
  auto returnType = d->m_NavigationBar->addFooterNode(footerTitle, page, keyPoints, awesome);
  if (page && returnType.has_value()) {
    d->m_PageMetaMap.insert(page->property("NXPageKey").toString(), page->metaObject());
  }
  return returnType;
}

QString NXWindow::addCategoryNode(QString const &categoryTitle)
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBar->addCategoryNode(categoryTitle);
}

NXNodeResultExpected NXWindow::addCategoryNode(QString const &categoryTitle, QString const &targetExpanderKey)
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBar->addCategoryNode(categoryTitle, targetExpanderKey);
}

void NXWindow::addCentralWidget(QWidget *centralWidget)
{
  Q_D(NXWindow);
  if (!centralWidget) {
    return;
  }
  d->m_CenterStackedWidget->getContainerStackedWidget()->addWidget(centralWidget);
}

QWidget *NXWindow::getCentralWidget(int index) const
{
  Q_CONST_D(NXWindow);
  if (index >= d->m_CenterStackedWidget->getContainerStackedWidget()->count() || index < 1) {
    return nullptr;
  }
  return d->m_CenterStackedWidget->getContainerStackedWidget()->widget(index);
}

bool NXWindow::getNavigationNodeIsExpanded(QString const &expanderKey) const
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBar->getNodeIsExpanded(expanderKey);
}

void NXWindow::expandNavigationNode(QString const &expanderKey)
{
  Q_D(NXWindow);
  d->m_NavigationBar->expandNode(expanderKey);
}

void NXWindow::collapseNavigationNode(QString const &expanderKey)
{
  Q_D(NXWindow);
  d->m_NavigationBar->collapseNode(expanderKey);
}

void NXWindow::removeNavigationNode(QString const &nodeKey) const
{
  Q_CONST_D(NXWindow);
  d->m_NavigationBar->removeNode(nodeKey);
}

int NXWindow::getPageOpenInNewWindowCount(QString const &nodeKey) const
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBar->getPageOpenInNewWindowCount(nodeKey);
}

void NXWindow::backtrackNavigationNode(QString const &nodeKey)
{
  Q_D(NXWindow);
  QMetaObject const *meta = d->m_PageMetaMap.value(nodeKey);
  if (!meta) {
    return;
  }
  QWidget *widget = dynamic_cast<QWidget *>(meta->newInstance());
  if (widget) {
    auto originWidget = d->m_RouteMap[nodeKey];
    int currentIndex  = d->m_NavigationCenterStackedWidget->getContainerStackedWidget()->currentIndex();
    int originIndex   = d->m_NavigationCenterStackedWidget->getContainerStackedWidget()->indexOf(originWidget);
    widget->setProperty("NXPageKey", nodeKey);
    d->m_RouteMap[nodeKey] = widget;
    d->m_NavigationCenterStackedWidget->getContainerStackedWidget()->insertWidget(originIndex, widget);
    d->m_NavigationCenterStackedWidget->getContainerStackedWidget()->setCurrentIndex(currentIndex);
    d->m_NavigationCenterStackedWidget->getContainerStackedWidget()->removeWidget(originWidget);
    originWidget->deleteLater();
  }
}

void NXWindow::setNodeKeyPoints(QString const &nodeKey, int keyPoints)
{
  Q_D(NXWindow);
  d->m_NavigationBar->setNodeKeyPoints(nodeKey, keyPoints);
}

int NXWindow::getNodeKeyPoints(QString const &nodeKey) const
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBar->getNodeKeyPoints(nodeKey);
}

void NXWindow::setNavigationNodeTitle(QString const &nodeKey, QString const &nodeTitle)
{
  Q_D(NXWindow);
  d->m_NavigationBar->setNodeTitle(nodeKey, nodeTitle);
}

QString NXWindow::getNavigationNodeTitle(QString const &nodeKey) const
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBar->getNodeTitle(nodeKey);
}

void NXWindow::navigation(QString const &pageKey)
{
  Q_D(NXWindow);
  d->m_NavigationBar->navigation(pageKey);
}

int NXWindow::getCurrentNavigationIndex() const
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationCenterStackedWidget->getContainerStackedWidget()->currentIndex();
}

QString NXWindow::getCurrentNavigationPageKey() const
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationCenterStackedWidget->getContainerStackedWidget()
      ->currentWidget()
      ->property("NXPageKey")
      .toString();
}

QList<NXSuggestBox::SuggestData> NXWindow::getNavigationSuggestDataList() const
{
  Q_CONST_D(NXWindow);
  return d->m_NavigationBar->getSuggestDataList();
}

void NXWindow::setWindowButtonFlag(NXAppBarType::ButtonType buttonFlag, bool isEnable)
{
  Q_D(NXWindow);
  d->m_AppBar->setWindowButtonFlag(buttonFlag, isEnable);
}

void NXWindow::setWindowButtonFlags(NXAppBarType::ButtonFlags buttonFlags)
{
  Q_D(NXWindow);
  d->m_AppBar->setWindowButtonFlags(buttonFlags);
}

NXAppBarType::ButtonFlags NXWindow::getWindowButtonFlags() const
{
  return d_ptr->m_AppBar->getWindowButtonFlags();
}

void NXWindow::setWindowMoviePath(NXThemeType::ThemeMode themeMode, QString const &moviePath)
{
  Q_D(NXWindow);
  if (themeMode == NXThemeType::ThemeMode::Light) {
    d->m_LightWindowMoviePath = moviePath;
  } else {
    d->m_DarkWindowMoviePath = moviePath;
  }
  if (d->m_ThemeMode == themeMode && d->m_WindowPaintMode == NXWindowType::PaintMode::Movie) {
    if (d->m_WindowPaintMovie->state() == QMovie::Running) {
      d->m_WindowPaintMovie->stop();
    }
    d->m_WindowPaintMovie->setFileName(moviePath);
    d->m_WindowPaintMovie->start();
  }
}

QString NXWindow::getWindowMoviePath(NXThemeType::ThemeMode themeMode) const
{
  Q_CONST_D(NXWindow);
  return themeMode == NXThemeType::Light ? d->m_LightWindowMoviePath : d->m_DarkWindowMoviePath;
}

void NXWindow::setWindowMovieRate(qreal rate)
{
  Q_D(NXWindow);
  d->m_WindowPaintMovie->setSpeed(rate * 100);
}

qreal NXWindow::getWindowMovieRate() const
{
  Q_CONST_D(NXWindow);
  return d->m_WindowPaintMovie->speed() / 100.0;
}

void NXWindow::tabifyDockWidget(QDockWidget *targetDockWidget, QDockWidget *dockWidget)
{
  QMainWindow::tabifyDockWidget(targetDockWidget, dockWidget);
}

void NXWindow::tabifyDockWidget(Qt::DockWidgetArea area, QString const &targetDockTitle, QDockWidget *dockWidget)
{
  if (!dockWidget) {
    return;
  }
  auto dockWidgetList = findChildren<QDockWidget *>();
  for (auto const otherDock : dockWidgetList) {
    if (otherDock == dockWidget) {
      continue;
    }
    if (dockWidgetArea(otherDock) == area && otherDock->windowTitle() == targetDockTitle) {
      tabifyDockWidget(otherDock, dockWidget);
      return;
    }
  }
  // 未找到 按Area添加
  addDockWidget(area, dockWidget);
}

void NXWindow::setWindowPixmap(NXThemeType::ThemeMode themeMode, QPixmap const &pixmap)
{
  Q_D(NXWindow);
  if (themeMode == NXThemeType::ThemeMode::Light) {
    *d->m_LightWindowPix = pixmap;
  } else {
    *d->m_DarkWindowPix = pixmap;
  }
  update();
}

QPixmap NXWindow::getWindowPixmap(NXThemeType::ThemeMode themeMode) const
{
  Q_CONST_D(NXWindow);
  return themeMode == NXThemeType::Light ? *d->m_LightWindowPix : *d->m_DarkWindowPix;
}

bool NXWindow::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXWindow);
  switch (event->type()) {
  case QEvent::Resize : {
    d->_doNavigationDisplayModeChange();
    break;
  }
#ifdef Q_OS_MACOS
  case QEvent::MouseButtonPress :
  case QEvent::MouseButtonRelease : {
    if (event->type() == QEvent::MouseButtonPress) {
      if (!NXApplication::containsCursorToItem(d->m_NavigationBar)) {
        if (d->m_IsNavigationBarExpanded) {
          QPropertyAnimation *navigationMoveAnimation = new QPropertyAnimation(d->m_NavigationBar, "pos");
          connect(navigationMoveAnimation, &QPropertyAnimation::valueChanged, d, [=]() {
            if (d->m_IsNavigationDisplayModeChanged) {
              d->m_IsNavigationBarFloat = false;
              d->_resetWindowLayout(false);
              navigationMoveAnimation->deleteLater();
            }
          });
          connect(navigationMoveAnimation, &QPropertyAnimation::finished, d, [=]() {
            if (!d->m_IsNavigationDisplayModeChanged) {
              d->m_NavigationBar->setDisplayMode(NXNavigationType::Minimal, false);
              d->_resetWindowLayout(false);
            }
            d->m_IsNavigationBarFloat = false;
          });
          navigationMoveAnimation->setEasingCurve(QEasingCurve::OutCubic);
          navigationMoveAnimation->setDuration(225);
          navigationMoveAnimation->setStartValue(d->m_NavigationBar->pos());
          navigationMoveAnimation->setEndValue(QPoint(-d->m_NavigationBar->width(), 0));
          navigationMoveAnimation->start(QAbstractAnimation::DeleteWhenStopped);
          d->m_IsNavigationBarExpanded = false;
        }
      }
    }
    break;
  }
#endif
  default : {
    break;
  }
  }
  return QMainWindow::eventFilter(watched, event);
}

QMenu *NXWindow::createPopupMenu()
{
  NXMenu *menu                     = nullptr;
  QList<QDockWidget *> dockwidgets = findChildren<QDockWidget *>();
  if (dockwidgets.size()) {
    menu = new NXMenu(this);
    for (int i = 0; i < dockwidgets.size(); ++i) {
      QDockWidget *dockWidget = dockwidgets.at(i);
      if (dockWidget->parentWidget() == this) {
        menu->addAction(dockwidgets.at(i)->toggleViewAction());
      }
    }
    menu->addSeparator();
  }

  QList<QToolBar *> toolbars = findChildren<QToolBar *>();
  if (toolbars.size()) {
    if (!menu) {
      menu = new NXMenu(this);
    }
    for (int i = 0; i < toolbars.size(); ++i) {
      QToolBar *toolBar = toolbars.at(i);
      if (toolBar->parentWidget() == this) {
        menu->addAction(toolbars.at(i)->toggleViewAction());
      }
    }
  }
  if (menu) {
    menu->setMenuItemHeight(28);
  }
  return menu;
}

void NXWindow::paintEvent(QPaintEvent *event)
{
  Q_D(NXWindow);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  switch (d->m_WindowDisplayMode) {
  case NXApplicationType::Normal : {
    switch (d->m_WindowPaintMode) {
    case NXWindowType::Normal : {
      painter.setPen(Qt::NoPen);
      painter.setBrush(NXThemeColor(d->m_ThemeMode, WindowBase));
      painter.drawRect(rect());
      break;
    }
    case NXWindowType::Pixmap : {
      QPixmap *pix            = d->m_ThemeMode == NXThemeType::Light ? d->m_LightWindowPix : d->m_DarkWindowPix;
      qreal windowAspectRatio = (qreal) rect().width() / rect().height();
      qreal pixAspectRatio    = (qreal) pix->width() / pix->height();
      int targetPixWidth, targetPixHeight;
      if (windowAspectRatio < pixAspectRatio) {
        targetPixWidth  = qRound(pix->width() * windowAspectRatio / pixAspectRatio);
        targetPixHeight = pix->height();
      } else {
        targetPixWidth  = pix->width();
        targetPixHeight = qRound(pix->height() * pixAspectRatio / windowAspectRatio);
      }
      painter.drawPixmap(rect(), *pix, QRect((pix->width() - targetPixWidth) / 2, (pix->height() - targetPixHeight) / 2, targetPixWidth, targetPixHeight));
      break;
    }
    case NXWindowType::Movie : {
      QPixmap pix             = d->m_WindowPaintMovie->currentPixmap();
      qreal windowAspectRatio = (qreal) rect().width() / rect().height();
      qreal pixAspectRatio    = (qreal) pix.width() / pix.height();
      int targetPixWidth, targetPixHeight;
      if (windowAspectRatio < pixAspectRatio) {
        targetPixWidth  = qRound(pix.width() * windowAspectRatio / pixAspectRatio);
        targetPixHeight = pix.height();
      } else {
        targetPixWidth  = pix.width();
        targetPixHeight = qRound(pix.height() * pixAspectRatio / windowAspectRatio);
      }
      painter.drawPixmap(rect(), pix, QRect((pix.width() - targetPixWidth) / 2, (pix.height() - targetPixHeight) / 2, targetPixWidth, targetPixHeight));
      break;
    }
    default : {
      break;
    }
    }
  }
  default : {
    break;
  }
  }
  painter.restore();
}

#ifdef Q_OS_MACOS
void NXWindow::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXWindow);
  // 检查是否点击在导航栏外部
  if (!NXApplication::containsCursorToItem(d->m_NavigationBar)) {
    if (d->m_IsNavigationBarExpanded) {
      QPropertyAnimation *navigationMoveAnimation = new QPropertyAnimation(d->m_NavigationBar, "pos");
      connect(navigationMoveAnimation, &QPropertyAnimation::valueChanged, d, [=]() {
        if (d->m_IsNavigationDisplayModeChanged) {
          d->m_IsNavigationBarFloat = false;
          d->_resetWindowLayout(false);
          navigationMoveAnimation->deleteLater();
        }
      });
      connect(navigationMoveAnimation, &QPropertyAnimation::finished, d, [=]() {
        if (!d->m_IsNavigationDisplayModeChanged) {
          d->m_NavigationBar->setDisplayMode(NXNavigationType::Minimal, false);
          d->_resetWindowLayout(false);
        }
        d->m_IsNavigationBarFloat = false;
      });
      navigationMoveAnimation->setEasingCurve(QEasingCurve::OutCubic);
      navigationMoveAnimation->setDuration(225);
      navigationMoveAnimation->setStartValue(d->m_NavigationBar->pos());
      navigationMoveAnimation->setEndValue(QPoint(-d->m_NavigationBar->width(), 0));
      navigationMoveAnimation->start(QAbstractAnimation::DeleteWhenStopped);
      d->m_IsNavigationBarExpanded = false;
    }
  }
  QMainWindow::mousePressEvent(event);
}
#endif

QWidget *NXWindow::centralWidget() const
{
  return QMainWindow::centralWidget();
}

void NXWindow::setCentralWidget(QWidget *widget)
{
  QMainWindow::setCentralWidget(widget);
}
