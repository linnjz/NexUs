#include "mainwindow.h"

#include <QGraphicsView>
#include <QHBoxLayout>
#include <QMouseEvent>
#include "NXCheckBox.h"
#include "NXContentDialog.h"
#include "NXDockWidget.h"
#include "NXEventBus.h"
#include "NXLog.h"
#include "NXMenu.h"
#include "NXMenuBar.h"
#include "NXNavigationRouter.h"
#include "NXProgressBar.h"
#include "NXProgressRing.h"
#include "NXPushButton.h"
#include "NXRibbonBar.h"
#include "NXRibbonGroup.h"
#include "NXRibbonTabBar.h"
#include "NXSlider.h"
#include "NXStatusBar.h"
#include "NXSuggestBox.h"
#include "NXText.h"
#include "NXTheme.h"
#include "NXToolBar.h"
#include "NXToolButton.h"
#include "T_About.h"
#include "T_BaseComponents.h"
#include "T_Card.h"
#include "T_Graphics.h"
#include "T_ListView.h"
#include "T_Setting.h"
#include "T_TableView.h"
#include "T_TableWidget.h"
#include "T_TreeView.h"
#ifdef Q_OS_WIN
#  include <QTimer>
#  include "NXApplication.h"
#endif
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
#  include "ExamplePage/T_NXScreen.h"
#endif

#include "ExamplePage/T_CodeEditor.h"
#include "ExamplePage/T_Home.h"
#include "ExamplePage/T_Icon.h"
#include "ExamplePage/T_LogWidget.h"
#include "ExamplePage/T_Navigation.h"
#include "ExamplePage/T_NewComponents.h"
#include "ExamplePage/T_NewComponents2.h"
#include "ExamplePage/T_Popup.h"
#include "ExamplePage/T_Router.h"
#include "ExamplePage/T_UpdateWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : NXWindow(parent)
{
  initWindow();

  //额外布局
  initEdgeLayout();

  initRibbon();

  //中心窗口
  initContent();

  // 拦截默认关闭事件
  _closeDialog    = new NXContentDialog(this);
  auto *cancelBtn = _closeDialog->addButton(u8"取消");
  auto *minBtn    = _closeDialog->addButton(u8"最小化");
  auto *exitBtn   = _closeDialog->addButton(u8"退出");

  exitBtn->setLightDefaultColor(NXThemeColor(NXThemeType::Light, PrimaryNormal));
  exitBtn->setLightHoverColor(NXThemeColor(NXThemeType::Light, PrimaryHover));
  exitBtn->setLightPressColor(NXThemeColor(NXThemeType::Light, PrimaryPress));
  exitBtn->setLightTextColor(Qt::white);
  exitBtn->setDarkDefaultColor(NXThemeColor(NXThemeType::Dark, PrimaryNormal));
  exitBtn->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, PrimaryHover));
  exitBtn->setDarkPressColor(NXThemeColor(NXThemeType::Dark, PrimaryPress));
  exitBtn->setDarkTextColor(Qt::black);

  connect(_closeDialog, &NXContentDialog::buttonClicked, this, [=](NXPushButton *btn) {
    if (btn == exitBtn) {
      close();
    } else if (btn == minBtn) {
      showMinimized();
    }
  });
  this->setIsDefaultClosed(false);
  connect(this, &MainWindow::closeButtonClicked, this, [=]() {
    _closeDialog->exec();
  });

  //移动到中心
  //moveToCenter();

  //  如果你的windows版本低于Win11 调用原生Mica、Mica-Alt、Acrylic 会导致窗口绘制失效  Dwm_Blur仍可使用
  //    nxTheme->setThemeMode(NXThemeType::Dark);
  //    QTimer::singleShot(1, this, [=]() {
  //        nxApp->setWindowDisplayMode(NXApplicationType::Mica);
  //    });

  //    QTimer::singleShot(1, this, [=]() {
  //        showFullScreen();
  //    });
}

MainWindow::~MainWindow()
{
  delete this->_aboutPage;
}

void MainWindow::initWindow()
{
  setFocusPolicy(Qt::StrongFocus);
  // setIsCentralStackedWidgetTransparent(true);
  setWindowIcon(QIcon(QStringLiteral(":/Resource/Image/Cirno.jpg")));
  resize(1200, 740);
  // nxTheme->setThemeMode(NXThemeType::Dark);
  // setIsNavigationBarEnable(false);
  // setNavigationBarDisplayMode(NXNavigationType::Compact);
  // setWindowButtonFlag(NXAppBarType::MinimizeButtonHint, false);
  setUserInfoCardPixmap(QPixmap(QStringLiteral(":/Resource/Image/Cirno.jpg")));
  setUserInfoCardTitle(QStringLiteral("NX Tool"));
  setUserInfoCardSubTitle(QStringLiteral("魔改版"));
  setWindowTitle(QStringLiteral("NXWidgetTool"));
  // setIsStayTop(true);
  // setUserInfoCardVisible(false);
  // setNavigationBarWidth(260);
  NXText *centralStack = new NXText(QStringLiteral("这是一个主窗口堆栈页面"), this);
  centralStack->setFocusPolicy(Qt::StrongFocus);
  centralStack->setTextPixelSize(32);
  centralStack->setAlignment(Qt::AlignCenter);
  addCentralWidget(centralStack);

  // 窗口绘制模式
  setWindowPixmap(NXThemeType::Light, QPixmap(QStringLiteral(":/Resource/Image/WindowBase/Miku.png")));
  setWindowPixmap(NXThemeType::Dark, QPixmap(QStringLiteral(":/Resource/Image/WindowBase/WorldTree.jpg")));
  setWindowMoviePath(NXThemeType::Light, QStringLiteral(":/Resource/Image/WindowBase/Miku.gif"));
  setWindowMoviePath(NXThemeType::Dark, QStringLiteral(":/Resource/Image/WindowBase/WorldTree.gif"));
  //setWindowPaintMode(NXWindowType::PaintMode::Pixmap);

  // 自定义AppBar菜单
  NXMenu *appBarMenu = new NXMenu(this);
  appBarMenu->setMenuItemHeight(27);
  connect(appBarMenu->addAction(QStringLiteral("跳转到一级主要堆栈")), &QAction::triggered, this, [=]() {
    setCurrentStackIndex(0);
  });
  connect(appBarMenu->addAction(QStringLiteral("跳转到二级主要堆栈")), &QAction::triggered, this, [=]() {
    setCurrentStackIndex(1);
  });
  connect(appBarMenu->addAction(QStringLiteral("更改页面切换特效(Scale)")), &QAction::triggered, this, [=]() {
    setStackSwitchMode(NXWindowType::StackSwitchMode::Scale);
  });
  connect(appBarMenu->addNXIconAction(NXIconType::GearComplex, QStringLiteral("自定义主窗口设置")), &QAction::triggered, this, [=]() {
    navigation(_settingKey);
  });
  appBarMenu->addSeparator();
  connect(appBarMenu->addNXIconAction(NXIconType::MoonStars, QStringLiteral("更改项目主题")), &QAction::triggered, this, [=]() {
    nxTheme->setThemeMode(nxTheme->getThemeMode() == NXThemeType::Light ? NXThemeType::Dark : NXThemeType::Light);
  });
  connect(appBarMenu->addAction(QStringLiteral("使用原生菜单")), &QAction::triggered, this, [=]() {
    setCustomMenu(nullptr);
  });
  setCustomMenu(appBarMenu);

  // 堆栈独立自定义窗口
  QWidget *centralCustomWidget           = new QWidget(this);
  QHBoxLayout *centralCustomWidgetLayout = new QHBoxLayout(centralCustomWidget);
  centralCustomWidgetLayout->setContentsMargins(13, 15, 9, 6);
  NXToolButton *leftButton = new NXToolButton(this);
  leftButton->setNXIcon(NXIconType::AngleLeft);
  leftButton->setEnabled(false);
  connect(leftButton, &NXToolButton::clicked, this, [=]() {
    NXNavigationRouter::getInstance()->navigationRouteBack();
  });
  NXToolButton *rightButton = new NXToolButton(this);
  rightButton->setNXIcon(NXIconType::AngleRight);
  rightButton->setEnabled(false);
  connect(rightButton, &NXToolButton::clicked, this, [=]() {
    NXNavigationRouter::getInstance()->navigationRouteForward();
  });
  connect(NXNavigationRouter::getInstance(), &NXNavigationRouter::navigationRouterStateChanged, this, [=](NXNavigationRouterType::RouteMode routeMode) {
    switch (routeMode) {
    case NXNavigationRouterType::BackValid : {
      leftButton->setEnabled(true);
      break;
    }
    case NXNavigationRouterType::BackInvalid : {
      leftButton->setEnabled(false);
      break;
    }
    case NXNavigationRouterType::ForwardValid : {
      rightButton->setEnabled(true);
      break;
    }
    case NXNavigationRouterType::ForwardInvalid : {
      rightButton->setEnabled(false);
      break;
    }
    }
  });
  _windowSuggestBox = new NXSuggestBox(this);
  _windowSuggestBox->setFixedHeight(32);
  _windowSuggestBox->setPlaceholderText(QStringLiteral("搜索关键字"));
  connect(_windowSuggestBox, &NXSuggestBox::suggestionClicked, this, [=](NXSuggestBox::SuggestData const &suggestData) {
    navigation(suggestData.getSuggestData().value(QStringLiteral("NXPageKey")).toString());
  });

  NXText *progressBusyRingText = new NXText(QStringLiteral("系统运行中"), this);
  progressBusyRingText->setIsWrapAnywhere(false);
  progressBusyRingText->setTextPixelSize(15);

  NXProgressRing *progressBusyRing = new NXProgressRing(this);
  progressBusyRing->setBusyingWidth(4);
  progressBusyRing->setFixedSize(28, 28);
  progressBusyRing->setIsBusying(true);

  centralCustomWidgetLayout->addWidget(leftButton);
  centralCustomWidgetLayout->addWidget(rightButton);
  centralCustomWidgetLayout->addWidget(_windowSuggestBox);
  centralCustomWidgetLayout->addStretch();
  centralCustomWidgetLayout->addWidget(progressBusyRingText);
  centralCustomWidgetLayout->addWidget(progressBusyRing);
  setCentralCustomWidget(centralCustomWidget);
}

void MainWindow::initEdgeLayout()
{
  //菜单栏
  NXMenuBar *menuBar = new NXMenuBar(this);
  menuBar->setFixedHeight(30);
  QWidget *customWidget = new QWidget(this);
  customWidget->setFixedWidth(500);
  QVBoxLayout *customLayout = new QVBoxLayout(customWidget);
  customLayout->setContentsMargins(0, 0, 0, 0);
  customLayout->addWidget(menuBar);
  customLayout->addStretch();
  // this->setMenuBar(menuBar);
  this->setCustomWidget(NXAppBarType::MiddleArea, customWidget);
  _menuBarWrapper = customWidget;

  menuBar->addNXIconAction(NXIconType::AtomSimple, QStringLiteral("动作菜单"));
  NXMenu *iconMenu = menuBar->addMenu(NXIconType::Aperture, QStringLiteral("图标菜单"));
  iconMenu->setMenuItemHeight(27);
  iconMenu->addNXIconAction(NXIconType::BoxCheck, QStringLiteral("排序方式"), QKeySequence::SelectAll);
  iconMenu->addNXIconAction(NXIconType::Copy, QStringLiteral("复制"));
  iconMenu->addNXIconAction(NXIconType::MagnifyingGlassPlus, QStringLiteral("显示设置"));
  iconMenu->addSeparator();
  iconMenu->addNXIconAction(NXIconType::ArrowRotateRight, QStringLiteral("刷新"));
  iconMenu->addNXIconAction(NXIconType::ArrowRotateLeft, QStringLiteral("撤销"));
  menuBar->addSeparator();
  NXMenu *shortCutMenu = new NXMenu(QStringLiteral("快捷菜单(&A)"), this);
  shortCutMenu->setMenuItemHeight(27);
  shortCutMenu->addNXIconAction(NXIconType::BoxCheck, QStringLiteral("排序方式"), QKeySequence::Find);
  shortCutMenu->addNXIconAction(NXIconType::Copy, QStringLiteral("复制"));
  shortCutMenu->addNXIconAction(NXIconType::MagnifyingGlassPlus, QStringLiteral("显示设置"));
  shortCutMenu->addSeparator();
  shortCutMenu->addNXIconAction(NXIconType::ArrowRotateRight, QStringLiteral("刷新"));
  shortCutMenu->addNXIconAction(NXIconType::ArrowRotateLeft, QStringLiteral("撤销"));
  menuBar->addMenu(shortCutMenu);

  menuBar->addMenu(QStringLiteral("样例菜单(&B)"))
      ->addNXIconAction(NXIconType::ArrowRotateRight, QStringLiteral("样例选项"));
  menuBar->addMenu(QStringLiteral("样例菜单(&C)"))
      ->addNXIconAction(NXIconType::ArrowRotateRight, QStringLiteral("样例选项"));
  menuBar->addMenu(QStringLiteral("样例菜单(&E)"))
      ->addNXIconAction(NXIconType::ArrowRotateRight, QStringLiteral("样例选项"));
  menuBar->addMenu(QStringLiteral("样例菜单(&F)"))
      ->addNXIconAction(NXIconType::ArrowRotateRight, QStringLiteral("样例选项"));
  menuBar->addMenu(QStringLiteral("样例菜单(&G)"))
      ->addNXIconAction(NXIconType::ArrowRotateRight, QStringLiteral("样例选项"));

  //工具栏
  NXToolBar *toolBar = new NXToolBar(QStringLiteral("工具栏"), this);
  toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  toolBar->setToolBarSpacing(3);
  toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
  toolBar->setIconSize(QSize(25, 25));
  // toolBar->setFloatable(false);
  // toolBar->setMovable(false);
  NXToolButton *toolButton1 = new NXToolButton(this);
  toolButton1->setNXIcon(NXIconType::BadgeCheck);
  toolBar->addWidget(toolButton1);
  NXToolButton *toolButton2 = new NXToolButton(this);
  toolButton2->setNXIcon(NXIconType::ChartUser);
  toolBar->addWidget(toolButton2);
  toolBar->addSeparator();
  NXToolButton *toolButton3 = new NXToolButton(this);
  toolButton3->setNXIcon(NXIconType::Bluetooth);
  toolButton3->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  toolButton3->setText(QStringLiteral("Bluetooth"));
  toolBar->addWidget(toolButton3);
  NXToolButton *toolButton4 = new NXToolButton(this);
  toolButton4->setNXIcon(NXIconType::BringFront);
  toolBar->addWidget(toolButton4);
  toolBar->addSeparator();
  NXToolButton *toolButton5 = new NXToolButton(this);
  toolButton5->setNXIcon(NXIconType::ChartSimple);
  toolBar->addWidget(toolButton5);
  NXToolButton *toolButton6 = new NXToolButton(this);
  toolButton6->setNXIcon(NXIconType::FaceClouds);
  toolBar->addWidget(toolButton6);
  NXToolButton *toolButton8 = new NXToolButton(this);
  toolButton8->setNXIcon(NXIconType::Aperture);
  toolBar->addWidget(toolButton8);
  NXToolButton *toolButton9 = new NXToolButton(this);
  toolButton9->setNXIcon(NXIconType::ChartMixed);
  toolBar->addWidget(toolButton9);
  NXToolButton *toolButton10 = new NXToolButton(this);
  toolButton10->setNXIcon(NXIconType::Coins);
  toolBar->addWidget(toolButton10);
  NXToolButton *toolButton11 = new NXToolButton(this);
  toolButton11->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  toolButton11->setNXIcon(NXIconType::AlarmPlus);
  toolButton11->setText(QStringLiteral("AlarmPlus"));
  toolBar->addWidget(toolButton11);
  NXToolButton *toolButton12 = new NXToolButton(this);
  toolButton12->setNXIcon(NXIconType::Crown);
  toolBar->addWidget(toolButton12);
  QAction *test = new QAction(this);
  test->setMenu(new QMenu(this));

  toolBar->addSeparator();
  NXCheckBox *ribbonToggle = new NXCheckBox(QStringLiteral("启用 Ribbon"), this);
  ribbonToggle->setChecked(false);
  connect(ribbonToggle, &QCheckBox::toggled, this, [=](bool checked) {
    setRibbonMode(checked);
  });
  toolBar->addWidget(ribbonToggle);

  NXProgressBar *progressBar = new NXProgressBar(this);
  progressBar->setMinimum(0);
  progressBar->setMaximum(0);
  progressBar->setFixedWidth(350);
  toolBar->addWidget(progressBar);

  this->addToolBar(Qt::TopToolBarArea, toolBar);

  //停靠窗口
  NXDockWidget *logDockWidget = new NXDockWidget(QStringLiteral("日志信息"), this);
  logDockWidget->setWidget(new T_LogWidget(this));
  this->addDockWidget(Qt::RightDockWidgetArea, logDockWidget);
  resizeDocks({ logDockWidget }, { 200 }, Qt::Horizontal);

  NXDockWidget *updateDockWidget = new NXDockWidget(QStringLiteral("更新内容"), this);
  updateDockWidget->setWidget(new T_UpdateWidget(this));
  this->addDockWidget(Qt::RightDockWidgetArea, updateDockWidget);
  resizeDocks({ updateDockWidget }, { 200 }, Qt::Horizontal);

  //状态栏
  NXStatusBar *statusBar = new NXStatusBar(this);
  NXText *statusText     = new NXText(QStringLiteral("初始化成功！"), this);
  statusText->setTextPixelSize(14);
  statusBar->addWidget(statusText);
  this->setStatusBar(statusBar);
}

void MainWindow::initContent()
{
  _homePage = new T_Home(this);
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
  _screenPage = new T_NXScreen(this);
#endif
  _iconPage           = new T_Icon(this);
  _baseComponentsPage = new T_BaseComponents(this);
  _graphicsPage       = new T_Graphics(this);
  _navigationPage     = new T_Navigation(this);
  _popupPage          = new T_Popup(this);
  _cardPage           = new T_Card(this);
  _listViewPage       = new T_ListView(this);
  _tableViewPage      = new T_TableView(this);
  _tableWidgetPage    = new T_TableWidget(this);
  _treeViewPage       = new T_TreeView(this);
  _newComponentsPage  = new T_NewComponents(this);
  _newComponents2Page = new T_NewComponents2(this);
  _codeEditorPage     = new T_CodeEditor(this);
  _routerPage         = new T_Router(this);
  _settingPage        = new T_Setting(this);

  addPageNode(QStringLiteral("HOME"), _homePage, NXIconType::House);
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
#  ifdef Q_OS_WIN
  _dxgiKey = addExpanderNode(QStringLiteral("NXDxgi"), NXIconType::TvMusic);
#  else
  _dxgiKey = addExpanderNode(QStringLiteral("NXScreenCapture"), NXIconType::TvMusic);
#  endif
  QString dxgiCategoryKey;
#  ifdef Q_OS_WIN
  dxgiCategoryKey = *addCategoryNode(QStringLiteral("Windows-DXGI"), _dxgiKey);
#  else
  dxgiCategoryKey = *addCategoryNode(QStringLiteral("macOS-ScreenCapture"), _dxgiKey);
#  endif
  addPageNode(QStringLiteral("NXScreen"), _screenPage, _dxgiKey, 3, NXIconType::ObjectGroup);
#endif
  QString controlCategoryKey = addCategoryNode(QStringLiteral("Controls"));
  // navigation(screenWidget->property("NXPageKey").toString());
  addPageNode(QStringLiteral("NXBaseComponents"), _baseComponentsPage, NXIconType::CabinetFiling);

  _viewKey                = addExpanderNode(QStringLiteral("NXView"), NXIconType::CameraViewfinder);
  QString viewCategoryKey = *addCategoryNode(QStringLiteral("View Content"), _viewKey);
  addPageNode(QStringLiteral("NXListView"), _listViewPage, _viewKey, 9, NXIconType::List);
  addPageNode(QStringLiteral("NXTableView"), _tableViewPage, _viewKey, NXIconType::Table);
  addPageNode(QStringLiteral("NXTableWidget"), _tableWidgetPage, _viewKey, NXIconType::TableCells);
  addPageNode(QStringLiteral("NXTreeView"), _treeViewPage, _viewKey, NXIconType::ListTree);
  expandNavigationNode(_viewKey);

  addPageNode(QStringLiteral("NXGraphics"), _graphicsPage, 9, NXIconType::Paintbrush);
  addPageNode(QStringLiteral("NXCard"), _cardPage, NXIconType::Cards);
  QString customKey = addCategoryNode(QStringLiteral("Custom"));
  addPageNode(QStringLiteral("NXNavigation"), _navigationPage, NXIconType::LocationArrow);
  addPageNode(QStringLiteral("NXPopup"), _popupPage, NXIconType::Envelope);
  addPageNode(QStringLiteral("NXNewComponents"), _newComponentsPage, NXIconType::Sparkles);
  addPageNode(QStringLiteral("NXNewComponents2"), _newComponents2Page, NXIconType::StarChristmas);
  addPageNode(QStringLiteral("NXCodeEditor"), _codeEditorPage, NXIconType::Code);
  addPageNode(QStringLiteral("NXRouter"), _routerPage, NXIconType::SignsPost);
  addPageNode(QStringLiteral("NXIcon"), _iconPage, 99, NXIconType::FontCase);
  QString testKey_1 = addExpanderNode(QStringLiteral("TEST_EXPAND_NODE1"), NXIconType::Acorn);
  QString testKey_2 = *addExpanderNode(QStringLiteral("TEST_EXPAND_NODE2"), testKey_1, NXIconType::Acorn);
  addPageNode(QStringLiteral("TEST_NODE3"), new QWidget(this), testKey_2, NXIconType::Acorn);
  for (int i = 0; i < 10; i++) {
    addExpanderNode(QString(QStringLiteral("TEST_EXPAND_NODE%1")).arg(i + 4), testKey_2, NXIconType::Acorn);
  }
  addExpanderNode(QStringLiteral("TEST_EXPAND_NODE14"), testKey_1, NXIconType::Acorn);
  addExpanderNode(QStringLiteral("TEST_EXPAND_NODE5"), testKey_1, NXIconType::Acorn);
  addExpanderNode(QStringLiteral("TEST_EXPAND_NODE16"), testKey_1, NXIconType::Acorn);

  _aboutKey  = *addFooterNode(QStringLiteral("About"), nullptr, 0, NXIconType::User);
  _aboutPage = new T_About();

  _aboutPage->hide();
  connect(this, &NXWindow::navigationNodeClicked, this, [=](NXNavigationType::NavigationNodeType nodeType, QString nodeKey) {
    if (_aboutKey == nodeKey) {
      _aboutPage->moveToCenter();
      _aboutPage->show();
    }
  });
  _settingKey = *addFooterNode(QStringLiteral("Setting"), _settingPage, 0, NXIconType::GearComplex);
  connect(this, &MainWindow::userInfoCardClicked, this, [=]() {
    this->navigation(_homePage->property("NXPageKey").toString());
  });
#ifdef Q_OS_WIN
  connect(_homePage, &T_Home::screenNavigation, this, [=]() {
    this->navigation(_screenPage->property("NXPageKey").toString());
  });
#endif
  connect(_homePage, &T_Home::baseComponentNavigation, this, [=]() {
    this->navigation(_baseComponentsPage->property("NXPageKey").toString());
  });
  connect(_homePage, &T_Home::sceneNavigation, this, [=]() {
    this->navigation(_graphicsPage->property("NXPageKey").toString());
  });
  connect(_homePage, &T_Home::iconNavigation, this, [=]() {
    this->navigation(_iconPage->property("NXPageKey").toString());
  });
  connect(_homePage, &T_Home::cardNavigation, this, [=]() {
    this->navigation(_cardPage->property("NXPageKey").toString());
  });

  _windowSuggestBox->addSuggestion(getNavigationSuggestDataList());
  qDebug() << "已注册的事件列表" << NXEventBus::getInstance()->getRegisteredEventsName();
}

void MainWindow::initRibbon()
{
  NXRibbonTabBar *ribbonTabBar = new NXRibbonTabBar(this);
  QWidget *tabBarWrapper       = new QWidget(this);
  tabBarWrapper->setFixedWidth(560);
  QHBoxLayout *tabBarLayout = new QHBoxLayout(tabBarWrapper);
  tabBarLayout->setContentsMargins(0, 0, 0, 0);
  tabBarLayout->setSpacing(0);
  tabBarLayout->addWidget(ribbonTabBar);
  tabBarLayout->addStretch();
  tabBarWrapper->hide();
  _ribbonTabBarWrapper = tabBarWrapper;

  NXRibbonBar *ribbonBar = new NXRibbonBar(this);
  ribbonBar->bindTabBar(ribbonTabBar);
  ribbonBar->setAnimationDuration(500);
  _ribbonBar = ribbonBar;

  QWidget *homePage        = ribbonBar->addTab(QStringLiteral("开始"));
  NXRibbonGroup *clipGroup = ribbonBar->addGroup(homePage, QStringLiteral("剪贴板"));
  clipGroup->addToolButton(NXIconType::Paste, QStringLiteral("粘贴"));
  clipGroup->addToolButton(NXIconType::Scissors, QStringLiteral("剪切"));
  clipGroup->addToolButton(NXIconType::Copy, QStringLiteral("复制"));

  NXRibbonGroup *fontGroup = ribbonBar->addGroup(homePage, QStringLiteral("字体"));
  fontGroup->addToolButton(NXIconType::Bold, QStringLiteral("加粗"));
  fontGroup->addToolButton(NXIconType::Italic, QStringLiteral("斜体"));
  fontGroup->addToolButton(NXIconType::Underline, QStringLiteral("下划线"));
  fontGroup->addToolButton(NXIconType::Highlighter, QStringLiteral("高亮"));

  NXRibbonGroup *paragraphGroup = ribbonBar->addGroup(homePage, QStringLiteral("段落"));
  paragraphGroup->addToolButton(NXIconType::AlignLeft, QStringLiteral("左对齐"));
  paragraphGroup->addToolButton(NXIconType::AlignCenter, QStringLiteral("居中"));
  paragraphGroup->addToolButton(NXIconType::AlignRight, QStringLiteral("右对齐"));
  paragraphGroup->addToolButton(NXIconType::ListUl, QStringLiteral("项目符号"));

  NXRibbonGroup *editGroup = ribbonBar->addGroup(homePage, QStringLiteral("编辑"));
  editGroup->addToolButton(NXIconType::PenToSquare, QStringLiteral("修改"));
  editGroup->addToolButton(NXIconType::Eraser, QStringLiteral("清除"));

  QWidget *insertPage       = ribbonBar->addTab(QStringLiteral("插入"));
  NXRibbonGroup *tableGroup = ribbonBar->addGroup(insertPage, QStringLiteral("表格"));
  tableGroup->addToolButton(NXIconType::Table, QStringLiteral("表格"));

  NXRibbonGroup *illustrationGroup = ribbonBar->addGroup(insertPage, QStringLiteral("插图"));
  illustrationGroup->addToolButton(NXIconType::Image, QStringLiteral("图片"));
  illustrationGroup->addToolButton(NXIconType::Camera, QStringLiteral("拍照"));
  illustrationGroup->addToolButton(NXIconType::Palette, QStringLiteral("形状"));

  NXRibbonGroup *linkGroup = ribbonBar->addGroup(insertPage, QStringLiteral("链接"));
  linkGroup->addToolButton(NXIconType::Link, QStringLiteral("超链接"));
  linkGroup->addToolButton(NXIconType::Bookmark, QStringLiteral("书签"));

  NXRibbonGroup *mediaGroup = ribbonBar->addGroup(insertPage, QStringLiteral("媒体"));
  mediaGroup->addToolButton(NXIconType::Music, QStringLiteral("音频"));
  mediaGroup->addToolButton(NXIconType::Video, QStringLiteral("视频"));

  QWidget *designPage       = ribbonBar->addTab(QStringLiteral("设计"));
  NXRibbonGroup *themeGroup = ribbonBar->addGroup(designPage, QStringLiteral("主题"));
  themeGroup->addToolButton(NXIconType::Sun, QStringLiteral("明亮"));
  themeGroup->addToolButton(NXIconType::Moon, QStringLiteral("暗色"));

  NXRibbonGroup *typoGroup = ribbonBar->addGroup(designPage, QStringLiteral("排版"));
  typoGroup->addToolButton(NXIconType::TextSize, QStringLiteral("字号"));
  typoGroup->addToolButton(NXIconType::FontCase, QStringLiteral("字体"));
  typoGroup->addToolButton(NXIconType::Ruler, QStringLiteral("页边距"));

  QWidget *reviewPage       = ribbonBar->addTab(QStringLiteral("审阅"));
  NXRibbonGroup *proofGroup = ribbonBar->addGroup(reviewPage, QStringLiteral("校对"));
  proofGroup->addToolButton(NXIconType::CircleCheck, QStringLiteral("拼写"));
  proofGroup->addToolButton(NXIconType::CircleQuestion, QStringLiteral("字数"));

  NXRibbonGroup *commentGroup = ribbonBar->addGroup(reviewPage, QStringLiteral("评论"));
  commentGroup->addToolButton(NXIconType::Comment, QStringLiteral("新建"));
  commentGroup->addToolButton(NXIconType::Comments, QStringLiteral("查看"));
  commentGroup->addToolButton(NXIconType::Reply, QStringLiteral("回复"));

  QWidget *viewPage        = ribbonBar->addTab(QStringLiteral("视图"));
  NXRibbonGroup *zoomGroup = ribbonBar->addGroup(viewPage, QStringLiteral("缩放"));
  zoomGroup->addToolButton(NXIconType::Plus, QStringLiteral("放大"));
  zoomGroup->addToolButton(NXIconType::Minus, QStringLiteral("缩小"));

  NXRibbonGroup *displayGroup = ribbonBar->addGroup(viewPage, QStringLiteral("显示"));
  displayGroup->addToolButton(NXIconType::Eye, QStringLiteral("网格"));
  displayGroup->addToolButton(NXIconType::EyeSlash, QStringLiteral("标尺"));

  NXRibbonGroup *prefsGroup      = ribbonBar->addGroup(viewPage, QStringLiteral("显示设置"));
  QWidget *checkColumn           = new QWidget(this);
  QVBoxLayout *checkColumnLayout = new QVBoxLayout(checkColumn);
  checkColumnLayout->setContentsMargins(4, 6, 4, 6);
  checkColumnLayout->setSpacing(8);
  checkColumnLayout->addWidget(new NXCheckBox(QStringLiteral("网格线"), this));
  checkColumnLayout->addWidget(new NXCheckBox(QStringLiteral("标尺"), this));
  checkColumnLayout->addWidget(new NXCheckBox(QStringLiteral("导航栏"), this));
  prefsGroup->addWidget(checkColumn);

  QWidget *zoomColumn           = new QWidget(this);
  QVBoxLayout *zoomColumnLayout = new QVBoxLayout(zoomColumn);
  zoomColumnLayout->setContentsMargins(4, 4, 4, 4);
  zoomColumnLayout->setSpacing(2);
  NXText *zoomLabel = new NXText(QStringLiteral("缩放级别"), this);
  zoomLabel->setTextPixelSize(11);
  NXSlider *zoomSlider = new NXSlider(Qt::Horizontal, this);
  zoomSlider->setRange(50, 200);
  zoomSlider->setValue(100);
  zoomSlider->setFixedWidth(140);
  NXText *zoomValue = new NXText(QStringLiteral("100%"), this);
  zoomValue->setTextPixelSize(11);
  zoomValue->setAlignment(Qt::AlignCenter);
  connect(zoomSlider, &QSlider::valueChanged, zoomValue, [=](int v) {
    zoomValue->setText(QString(QStringLiteral("%1%")).arg(v));
  });
  zoomColumnLayout->addWidget(zoomLabel);
  zoomColumnLayout->addWidget(zoomSlider);
  zoomColumnLayout->addWidget(zoomValue);
  prefsGroup->addWidget(zoomColumn);

  NXToolBar *ribbonToolBar = new NXToolBar(QStringLiteral("Ribbon"), this);
  ribbonToolBar->setObjectName("RibbonToolBar");
  ribbonToolBar->setMovable(false);
  ribbonToolBar->setFloatable(false);
  ribbonToolBar->setToolBarSpacing(0);
  ribbonToolBar->layout()->setContentsMargins(0, 0, 0, 0);
  ribbonToolBar->addWidget(ribbonBar);

  QToolBar *existingTopBar = nullptr;
  for (QObject *child : this->children()) {
    QToolBar *bar = qobject_cast<QToolBar *>(child);
    if (bar && bar != ribbonToolBar && this->toolBarArea(bar) == Qt::TopToolBarArea) {
      existingTopBar = bar;
      break;
    }
  }
  if (existingTopBar) {
    insertToolBar(existingTopBar, ribbonToolBar);
    insertToolBarBreak(existingTopBar);
  } else {
    addToolBar(Qt::TopToolBarArea, ribbonToolBar);
  }
  ribbonToolBar->hide();
  _ribbonToolBar = ribbonToolBar;

  ribbonToolBar->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ribbonToolBar, &QWidget::customContextMenuRequested, this, [=](QPoint const &pos) {
    ribbonBar->showPinContextMenu(ribbonToolBar->mapToGlobal(pos));
  });
}

void MainWindow::setRibbonMode(bool enabled)
{
  _isRibbonMode = enabled;
  if (enabled) {
    if (_menuBarWrapper) {
      _menuBarWrapper->hide();
    }
    if (_ribbonTabBarWrapper) {
      _ribbonTabBarWrapper->show();
      this->setCustomWidget(NXAppBarType::MiddleArea, _ribbonTabBarWrapper);
    }
    if (_ribbonToolBar) {
      _ribbonToolBar->show();
    }
  } else {
    if (_ribbonTabBarWrapper) {
      _ribbonTabBarWrapper->hide();
    }
    if (_ribbonToolBar) {
      _ribbonToolBar->hide();
    }
    if (_menuBarWrapper) {
      _menuBarWrapper->show();
      this->setCustomWidget(NXAppBarType::MiddleArea, _menuBarWrapper);
    }
  }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
  if (getCurrentNavigationIndex() != 2) {
    switch (event->button()) {
    case Qt::BackButton : {
      this->setCurrentStackIndex(0);
      break;
    }
    case Qt::ForwardButton : {
      this->setCurrentStackIndex(1);
      break;
    }
    default : {
      break;
    }
    }
  }
  NXWindow::mouseReleaseEvent(event);
}
