#include "T_Navigation.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "NXBreadcrumbBar.h"
#include "NXPivot.h"
#include "NXPushButton.h"
#include "NXScrollPageArea.h"
#include "NXTabWidget.h"
#include "NXText.h"

T_Navigation::T_Navigation(QWidget *parent)
    : T_BasePage(parent)
{
  // 预览窗口标题
  setWindowTitle(QStringLiteral("NXNavigation"));

  // 顶部元素
  createCustomWidget(QStringLiteral("一些导航组件被放置于此，可在此界面体验其效果并按需添加进项目中"));

  //NXBreadcrumbBar
  NXText *breadcrumbBarText = new NXText(QStringLiteral("NXBreadcrumbBar"), this);
  breadcrumbBarText->setTextPixelSize(18);
  _breadcrumbBar = new NXBreadcrumbBar(this);
  QStringList breadcrumbBarList;
  for (int i = 0; i < 20; i++) {
    breadcrumbBarList << QString(QStringLiteral("Item%1")).arg(i + 1);
  }
  _breadcrumbBar->setBreadcrumbList(breadcrumbBarList);

  NXPushButton *resetButton = new NXPushButton(QStringLiteral("还原"), this);
  resetButton->setFixedSize(60, 32);
  connect(resetButton, &NXPushButton::clicked, this, [=]() {
    _breadcrumbBar->setBreadcrumbList(breadcrumbBarList);
  });

  QHBoxLayout *breadcrumbBarTextLayout = new QHBoxLayout();
  breadcrumbBarTextLayout->addWidget(breadcrumbBarText);
  breadcrumbBarTextLayout->addSpacing(15);
  breadcrumbBarTextLayout->addWidget(resetButton);
  breadcrumbBarTextLayout->addStretch();

  NXScrollPageArea *breadcrumbBarArea = new NXScrollPageArea(this);
  QVBoxLayout *breadcrumbBarLayout    = new QVBoxLayout(breadcrumbBarArea);
  breadcrumbBarLayout->addWidget(_breadcrumbBar);

  // NXPivot
  NXText *pivotText = new NXText(QStringLiteral("NXPivot"), this);
  pivotText->setTextPixelSize(18);
  _pivot = new NXPivot(this);
  _pivot->setIndicatorWidth(75);
  _pivot->appendPivot(QStringLiteral("本地歌曲"));
  _pivot->appendPivot(QStringLiteral("下载歌曲"));
  _pivot->appendPivot(QStringLiteral("下载视频"));
  _pivot->appendPivot(QStringLiteral("正在下载"));
  _pivot->appendPivot(QStringLiteral("本地歌曲"));
  _pivot->appendPivot(QStringLiteral("下载歌曲"));
  _pivot->appendPivot(QStringLiteral("下载视频"));
  _pivot->appendPivot(QStringLiteral("正在下载"));
  _pivot->appendPivot(QStringLiteral("本地歌曲"));
  _pivot->appendPivot(QStringLiteral("下载歌曲"));
  _pivot->appendPivot(QStringLiteral("下载视频"));
  _pivot->appendPivot(QStringLiteral("正在下载"));
  _pivot->setCurrentIndex(0);

  NXScrollPageArea *pivotArea = new NXScrollPageArea(this);
  QVBoxLayout *pivotLayout    = new QVBoxLayout(pivotArea);
  pivotLayout->addWidget(_pivot);

  // NXTabWidget
  NXText *tabWidgetText = new NXText(QStringLiteral("NXTabWidget"), this);
  tabWidgetText->setTextPixelSize(18);
  _tabWidget = new NXTabWidget(this);
  _tabWidget->setFixedHeight(600);
  _tabWidget->setIsTabTransparent(true);
  NXText *page1 = new NXText(QStringLiteral("新标签页"), this);
  page1->setTextPixelSize(32);
  page1->setAlignment(Qt::AlignCenter);
  _tabWidget->addTab(page1, QIcon(QStringLiteral(":/Resource/Image/Cirno.jpg")), QStringLiteral("新标签页"));
  for (int i = 0; i < 5; i++) {
    NXText *page = new NXText(QString(QStringLiteral("新标签页%1")).arg(i), this);
    page->setTextPixelSize(32);
    page->setAlignment(Qt::AlignCenter);
    _tabWidget->addTab(page, QString(QStringLiteral("新标签页%1")).arg(i));
  }
  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("NXNavigation"));
  QVBoxLayout *centerVLayout = new QVBoxLayout(centralWidget);
  centerVLayout->setContentsMargins(0, 0, 0, 0);
  centerVLayout->addLayout(breadcrumbBarTextLayout);
  centerVLayout->addSpacing(10);
  centerVLayout->addWidget(breadcrumbBarArea);
  centerVLayout->addSpacing(15);
  centerVLayout->addWidget(pivotText);
  centerVLayout->addSpacing(10);
  centerVLayout->addWidget(pivotArea);
  centerVLayout->addSpacing(15);
  centerVLayout->addWidget(tabWidgetText);
  centerVLayout->addSpacing(10);
  centerVLayout->addWidget(_tabWidget);
  centerVLayout->addStretch();
  addCentralWidget(centralWidget, true, false, 0);
}

T_Navigation::~T_Navigation()
{
}
