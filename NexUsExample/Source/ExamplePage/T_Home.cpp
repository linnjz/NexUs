#include "T_Home.h"

#include <QDebug>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>

#include "NXAcrylicUrlCard.h"
#include "NXFlowLayout.h"
#include "NXImageCard.h"
#include "NXMenu.h"
#include "NXMessageBar.h"
#include "NXNavigationRouter.h"
#include "NXPopularCard.h"
#include "NXScrollArea.h"
#include "NXText.h"
#include "NXToolTip.h"

T_Home::T_Home(QWidget *parent)
    : T_BasePage(parent)
{
  // 预览窗口标题
  setWindowTitle(QStringLiteral("Home"));

  setTitleVisible(false);
  setContentsMargins(2, 2, 0, 0);
  // 标题卡片区域
  NXText *desText = new NXText(QStringLiteral("FluentUI For QWidget"), this);
  desText->setTextPixelSize(18);
  NXText *titleText = new NXText(QStringLiteral("NexUs"), this);
  titleText->setTextPixelSize(35);

  QVBoxLayout *titleLayout = new QVBoxLayout();
  titleLayout->setContentsMargins(30, 10, 0, 0);
  titleLayout->addWidget(desText);
  titleLayout->addWidget(titleText);

  NXImageCard *backgroundCard = new NXImageCard(this);
  backgroundCard->setBorderRadius(10);
  backgroundCard->setFixedHeight(340);
  backgroundCard->setCardImage(QImage(QStringLiteral(":/Resource/Image/Home_Background.png")));

  NXAcrylicUrlCard *urlCard1 = new NXAcrylicUrlCard(this);
  urlCard1->setCardPixmapSize(QSize(62, 62));
  urlCard1->setFixedSize(195, 225);
  urlCard1->setTitlePixelSize(17);
  urlCard1->setTitleSpacing(25);
  urlCard1->setSubTitleSpacing(13);
  urlCard1->setUrl(QStringLiteral("https://github.com/RainbowCandyX/NexUs"));
  urlCard1->setCardPixmap(QPixmap(QStringLiteral(":/Resource/Image/github.png")));
  urlCard1->setTitle(QStringLiteral("NXTool Github"));
  urlCard1->setSubTitle(QStringLiteral("Use NexUs To Create A Cool Project"));
  NXToolTip *urlCard1ToolTip = new NXToolTip(urlCard1);
  urlCard1ToolTip->setToolTip(QStringLiteral("https://github.com/RainbowCandyX/NexUs"));
  NXAcrylicUrlCard *urlCard2 = new NXAcrylicUrlCard(this);
  urlCard2->setCardPixmapSize(QSize(62, 62));
  urlCard2->setFixedSize(195, 225);
  urlCard2->setTitlePixelSize(17);
  urlCard2->setTitleSpacing(25);
  urlCard2->setSubTitleSpacing(13);
  urlCard2->setUrl(QStringLiteral("https://space.bilibili.com/21256707"));
  urlCard2->setCardPixmap(QPixmap(QStringLiteral(":/Resource/Image/Moon.jpg")));
  urlCard2->setTitle(QStringLiteral("NXWidgetTool"));
  urlCard2->setSubTitle(QStringLiteral("NXWidgetTool"));

  NXScrollArea *cardScrollArea = new NXScrollArea(this);
  cardScrollArea->setWidgetResizable(true);
  cardScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  cardScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  cardScrollArea->setIsGrabGesture(true, 0);
  cardScrollArea->setIsOverShoot(Qt::Horizontal, true);
  QWidget *cardScrollAreaWidget = new QWidget(this);
  cardScrollAreaWidget->setStyleSheet(QStringLiteral("background-color:transparent;"));
  cardScrollArea->setWidget(cardScrollAreaWidget);
  QHBoxLayout *urlCardLayout = new QHBoxLayout();
  urlCardLayout->setSpacing(15);
  urlCardLayout->setContentsMargins(30, 0, 0, 6);
  urlCardLayout->addWidget(urlCard1);
  urlCardLayout->addWidget(urlCard2);
  urlCardLayout->addStretch();
  QVBoxLayout *cardScrollAreaWidgetLayout = new QVBoxLayout(cardScrollAreaWidget);
  cardScrollAreaWidgetLayout->setContentsMargins(0, 0, 0, 0);
  cardScrollAreaWidgetLayout->addStretch();
  cardScrollAreaWidgetLayout->addLayout(urlCardLayout);

  QVBoxLayout *backgroundLayout = new QVBoxLayout(backgroundCard);
  backgroundLayout->setContentsMargins(0, 0, 0, 0);
  backgroundLayout->addLayout(titleLayout);
  backgroundLayout->addWidget(cardScrollArea);

  // 推荐卡片
  NXText *flowText = new NXText(QStringLiteral("热门免费应用"), this);
  flowText->setTextPixelSize(20);
  QHBoxLayout *flowTextLayout = new QHBoxLayout();
  flowTextLayout->setContentsMargins(33, 0, 0, 0);
  flowTextLayout->addWidget(flowText);
  // NXFlowLayout
  NXPopularCard *homeCard = new NXPopularCard(this);
  connect(homeCard, &NXPopularCard::popularCardButtonClicked, this, [=]() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://github.com/RainbowCandyX/NexUs")));
  });
  homeCard->setCardPixmap(QPixmap(QStringLiteral(":/Resource/Image/Cirno.jpg")));
  homeCard->setTitle(QStringLiteral("NXWidgetTool"));
  homeCard->setSubTitle(QStringLiteral("5.0⭐ 实用程序与工具"));
  homeCard->setInteractiveTips(QStringLiteral("免费下载"));
  homeCard->setDetailedText(QStringLiteral(
      "NexUs致力于为QWidget用户提供一站式的外观和实用功能解决方案, 只需数十MB内存和极少CPU占用以支持高效而美观的界面开发"));
  homeCard->setCardFloatPixmap(QPixmap(QStringLiteral(":/Resource/Image/IARC/IARC_7+.svg.png")));

  NXPopularCard *homeCard1 = new NXPopularCard(this);
  connect(homeCard1, &NXPopularCard::popularCardButtonClicked, this, [=]() {
    Q_EMIT screenNavigation();
  });
  homeCard1->setTitle(QStringLiteral("NXScreen"));
  homeCard1->setSubTitle(QStringLiteral("5.0⭐ 实用程序与工具"));
  homeCard1->setCardPixmap(QPixmap(QStringLiteral(":/Resource/Image/control/AutomationProperties.png")));
  homeCard1->setInteractiveTips(QStringLiteral("免费使用"));
  homeCard1->setDetailedText(
      QStringLiteral("使用NXDxgiManager获取屏幕的实时数据，以QImage的形式处理数据，支持切换采集设备和输出设备。"));
  homeCard1->setCardFloatPixmap(QPixmap(QStringLiteral(":/Resource/Image/IARC/IARC_7+.svg.png")));

  NXPopularCard *homeCard2 = new NXPopularCard(this);
  connect(homeCard2, &NXPopularCard::popularCardButtonClicked, this, [=]() {
    Q_EMIT sceneNavigation();
  });
  homeCard2->setTitle(QStringLiteral("NXScene"));
  homeCard2->setSubTitle(QStringLiteral("5.0⭐ 实用程序与工具"));
  homeCard2->setCardPixmap(QPixmap(QStringLiteral(":/Resource/Image/control/Canvas.png")));
  homeCard2->setInteractiveTips(QStringLiteral("免费使用"));
  homeCard2->setDetailedText(
      QStringLiteral("使用NXScene封装的高集成度API进行快速拓扑绘图开发，对基于连接的网络拓扑特化处理。"));
  homeCard2->setCardFloatPixmap(QPixmap(QStringLiteral(":/Resource/Image/IARC/IARC_7+.svg.png")));

  NXPopularCard *homeCard3 = new NXPopularCard(this);
  connect(homeCard3, &NXPopularCard::popularCardButtonClicked, this, [=]() {
    Q_EMIT baseComponentNavigation();
  });
  homeCard3->setTitle(QStringLiteral("NXBaseComponent"));
  homeCard3->setSubTitle(QStringLiteral("5.0⭐ 实用程序与工具"));
  homeCard3->setCardPixmap(QPixmap(QStringLiteral(":/Resource/Image/control/StandardUICommand.png")));
  homeCard3->setInteractiveTips(QStringLiteral("免费使用"));
  homeCard3->setDetailedText(
      QStringLiteral("添加NXBaseComponent页面中的基础组件到你的项目中以进行快捷开发，使用方便，结构整洁，API规范"));
  homeCard3->setCardFloatPixmap(QPixmap(QStringLiteral(":/Resource/Image/IARC/IARC_7+.svg.png")));

  NXPopularCard *homeCard4 = new NXPopularCard(this);
  connect(homeCard4, &NXPopularCard::popularCardButtonClicked, this, [=]() {
    Q_EMIT cardNavigation();
  });
  homeCard4->setTitle(QStringLiteral("NXCard"));
  homeCard4->setSubTitle(QStringLiteral("5.0⭐ 实用程序与工具"));
  homeCard4->setCardPixmap(QPixmap(QStringLiteral(":/Resource/Image/control/FlipView.png")));
  homeCard4->setInteractiveTips(QStringLiteral("免费使用"));
  homeCard4->setDetailedText(QStringLiteral("使用NXCard系列组件，包括促销卡片和促销卡片视窗来快速建立循环动画。"));
  homeCard4->setCardFloatPixmap(QPixmap(QStringLiteral(":/Resource/Image/IARC/IARC_7+.svg.png")));

  NXPopularCard *homeCard5 = new NXPopularCard(this);
  connect(homeCard5, &NXPopularCard::popularCardButtonClicked, this, [=]() {
    Q_EMIT iconNavigation();
  });
  homeCard5->setTitle(QStringLiteral("NXIcon"));
  homeCard5->setSubTitle(QStringLiteral("5.0⭐ 实用程序与工具"));
  homeCard5->setCardPixmap(QPixmap(QStringLiteral(":/Resource/Image/control/CommandBarFlyout.png")));
  homeCard5->setInteractiveTips(QStringLiteral("免费使用"));
  homeCard5->setDetailedText(QStringLiteral("在该界面快速挑选你喜欢的图标应用到项目中，以枚举的形式使用它"));
  homeCard5->setCardFloatPixmap(QPixmap(QStringLiteral(":/Resource/Image/IARC/IARC_7+.svg.png")));

  NXFlowLayout *flowLayout = new NXFlowLayout(0, 5, 5);
  flowLayout->setContentsMargins(30, 0, 0, 0);
  flowLayout->setIsAnimation(true);
  flowLayout->addWidget(homeCard);
  flowLayout->addWidget(homeCard1);
  flowLayout->addWidget(homeCard2);
  flowLayout->addWidget(homeCard3);
  flowLayout->addWidget(homeCard4);
  flowLayout->addWidget(homeCard5);

  // 菜单
  _homeMenu         = new NXMenu(this);
  NXMenu *checkMenu = _homeMenu->addMenu(NXIconType::Cubes, QStringLiteral("查看"));
  checkMenu->addAction(QStringLiteral("查看1"));
  checkMenu->addAction(QStringLiteral("查看2"));
  checkMenu->addAction(QStringLiteral("查看3"));
  checkMenu->addAction(QStringLiteral("查看4"));

  NXMenu *checkMenu1 = _homeMenu->addMenu(NXIconType::Cubes, QStringLiteral("查看"));
  checkMenu1->addAction(QStringLiteral("查看1"));
  checkMenu1->addAction(QStringLiteral("查看2"));
  checkMenu1->addAction(QStringLiteral("查看3"));
  checkMenu1->addAction(QStringLiteral("查看4"));

  NXMenu *checkMenu2 = checkMenu->addMenu(NXIconType::Cubes, QStringLiteral("查看"));
  checkMenu2->addAction(QStringLiteral("查看1"));
  checkMenu2->addAction(QStringLiteral("查看2"));
  checkMenu2->addAction(QStringLiteral("查看3"));
  checkMenu2->addAction(QStringLiteral("查看4"));

  // QKeySequence key = QKeySequence(Qt::CTRL | Qt::Key_S);

  _homeMenu->addSeparator();
  _homeMenu->addNXIconAction(NXIconType::BoxCheck, QStringLiteral("排序方式"), QKeySequence::Save);
  _homeMenu->addNXIconAction(NXIconType::ArrowRotateRight, QStringLiteral("刷新"));
  QAction *action = _homeMenu->addNXIconAction(NXIconType::ArrowRotateLeft, QStringLiteral("撤销"));
  connect(action, &QAction::triggered, this, [=]() {
    NXNavigationRouter::getInstance()->navigationRouteBack();
  });

  _homeMenu->addNXIconAction(NXIconType::Copy, QStringLiteral("复制"));
  _homeMenu->addNXIconAction(NXIconType::MagnifyingGlassPlus, QStringLiteral("显示设置"));

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("Home"));
  QVBoxLayout *centerVLayout = new QVBoxLayout(centralWidget);
  centerVLayout->setSpacing(0);
  centerVLayout->setContentsMargins(0, 0, 0, 0);
  centerVLayout->addWidget(backgroundCard);
  centerVLayout->addSpacing(20);
  centerVLayout->addLayout(flowTextLayout);
  centerVLayout->addSpacing(10);
  centerVLayout->addLayout(flowLayout);
  centerVLayout->addStretch();
  addCentralWidget(centralWidget);

  // 初始化提示
  NXMessageBar::success(NXMessageBarType::BottomRight, QStringLiteral("Success"), QStringLiteral("初始化成功!"), 2000);
  qDebug() << "初始化成功";
}

T_Home::~T_Home()
{
}

void T_Home::mouseReleaseEvent(QMouseEvent *event)
{
  switch (event->button()) {
  case Qt::RightButton : {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    _homeMenu->popup(event->globalPosition().toPoint());
#else
    _homeMenu->popup(event->globalPos());
#endif
    break;
  }
  default : {
    break;
  }
  }
  NXScrollPage::mouseReleaseEvent(event);
}
