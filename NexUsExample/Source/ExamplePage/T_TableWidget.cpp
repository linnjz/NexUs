#include "T_TableWidget.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "NXScrollBar.h"
#include "NXTableWidget.h"
#include "NXText.h"

T_TableWidget::T_TableWidget(QWidget *parent)
    : T_BasePage(parent)
{
  // 预览窗口标题
  setWindowTitle(QStringLiteral("NXTableWidget"));

  // 顶部元素
  createCustomWidget(QStringLiteral("表格部件被放置于此，可在此界面体验其效果并按需添加进项目中"));

  // NXTableWidget
  NXText *tableText = new NXText(QStringLiteral("NXTableWidget"), this);
  tableText->setTextPixelSize(18);

  _tableWidget = new NXTableWidget(this);
  _tableWidget->setFixedHeight(450);

  // 配置表格
  _tableWidget->setColumnCount(5);
  _tableWidget->setRowCount(9);

  // 设置表头
  QStringList headers;
  headers << QStringLiteral("预览") << QStringLiteral("歌名") << QStringLiteral("歌手") << QStringLiteral("专辑")
          << QStringLiteral("时长");
  _tableWidget->setHorizontalHeaderLabels(headers);

  // 配置表头外观
  QFont tableHeaderFont = _tableWidget->horizontalHeader()->font();
  tableHeaderFont.setPixelSize(16);
  _tableWidget->horizontalHeader()->setFont(tableHeaderFont);
  _tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
  _tableWidget->horizontalHeader()->setMinimumSectionSize(60);

  // 隐藏垂直表头
  _tableWidget->verticalHeader()->setHidden(true);
  _tableWidget->verticalHeader()->setMinimumSectionSize(46);

  // 配置表格行为
  _tableWidget->setAlternatingRowColors(true);
  _tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  _tableWidget->setIconSize(QSize(38, 38));

  // 添加示例数据
  QStringList songData[9] = {
    { QStringLiteral("夜航星(Night Voyager)"),          QStringLiteral("不才/三体宇宙"),         QStringLiteral("我的三体之章北海传"),
     QStringLiteral("05:03")                                                                                                                                   },
    { QStringLiteral("玫瑰少年"),                       QStringLiteral("五月天"),                QStringLiteral("玫瑰少年"),           QStringLiteral("03:55") },
    { QStringLiteral("Collapsing World(Original Mix)"), QStringLiteral("Lightscape"),
     QStringLiteral("Collapsing World"),                                                                                               QStringLiteral("03:10") },
    { QStringLiteral("RAIN MAN (雨人)"),                QStringLiteral("AKIHIDE (佐藤彰秀)"),    QStringLiteral("RAIN STORY"),
     QStringLiteral("05:37")                                                                                                                                   },
    { QStringLiteral("黑暗森林"),                       QStringLiteral("雲翼星辰"),              QStringLiteral("黑暗森林"),           QStringLiteral("05:47") },
    { QStringLiteral("轻(我的三体第四季主题曲)"),       QStringLiteral("刘雪茗"),                QStringLiteral("我的三体第四季"),
     QStringLiteral("01:59")                                                                                                                                   },
    { QStringLiteral("STYX HELIX"),                     QStringLiteral("MYTH & ROID"),           QStringLiteral("STYX HELIX"),
     QStringLiteral("04:51")                                                                                                                                   },
    { QStringLiteral("LAST STARDUST"),                  QStringLiteral("Aimer"),                 QStringLiteral("DAWN"),               QStringLiteral("05:18") },
    { QStringLiteral("Running In The Dark"),            QStringLiteral("MONKEY MAJIK/塞壬唱片"),
     QStringLiteral("Running In The Dark"),                                                                                            QStringLiteral("03:40") }
  };

  QString iconPaths[9] = { QStringLiteral(":/Resource/Image/Model/NaightNavigationStar.jpg"),
                           QStringLiteral(":/Resource/Image/Model/MaVieEnRose.jpg"),
                           QStringLiteral(":/Resource/Image/Model/CollapsingWorld.jpg"),
                           QStringLiteral(":/Resource/Image/Model/RainMan.jpg"),
                           QStringLiteral(":/Resource/Image/Model/DarkForest.jpg"),
                           QStringLiteral(":/Resource/Image/Model/Light.jpg"),
                           QStringLiteral(":/Resource/Image/Model/STYXHELIX.jpg"),
                           QStringLiteral(":/Resource/Image/Model/LASTSTARDUST.jpg"),
                           QStringLiteral(":/Resource/Image/Model/RunningInTheDark.jpg") };

  for (int row = 0; row < 9; ++row) {
    // 预览列 - 图标
    QTableWidgetItem *previewItem = new QTableWidgetItem();
    QIcon icon(QPixmap(iconPaths[row]).scaled(38, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    previewItem->setIcon(icon);
    previewItem->setTextAlignment(Qt::AlignCenter);
    _tableWidget->setItem(row, 0, previewItem);

    // 歌曲数据列
    for (int col = 0; col < 4; ++col) {
      QTableWidgetItem *item = new QTableWidgetItem(songData[row][col]);
      if (col == 3) {
        // 时长列居中对齐
        item->setTextAlignment(Qt::AlignCenter);
      }
      _tableWidget->setItem(row, col + 1, item);
    }

    // 为前三行设置不同的颜色
    if (row == 0) {
      // 第一行 - 红色背景
      for (int col = 0; col < 5; ++col) {
        _tableWidget->item(row, col)->setBackground(QColor(255, 150, 150));
      }
    } else if (row == 1) {
      // 第二行 - 绿色背景
      for (int col = 0; col < 5; ++col) {
        _tableWidget->item(row, col)->setBackground(QColor(150, 255, 150));
      }
    } else if (row == 2) {
      // 第三行 - 蓝色文字（不改变背景）
      for (int col = 0; col < 5; ++col) {
        _tableWidget->item(row, col)->setForeground(QColor(50, 50, 255));
      }
    }
  }

  // 设置列宽
  connect(_tableWidget, &NXTableWidget::tableWidgetShow, this, [=]() {
    _tableWidget->setColumnWidth(0, 60);
    _tableWidget->setColumnWidth(1, 205);
    _tableWidget->setColumnWidth(2, 170);
    _tableWidget->setColumnWidth(3, 150);
    _tableWidget->setColumnWidth(4, 60);
  });

  // 可选: 浮动滚动条
  NXScrollBar *tableWidgetFloatScrollBar = new NXScrollBar(_tableWidget->verticalScrollBar(), _tableWidget);
  tableWidgetFloatScrollBar->setIsAnimation(true);

  // 布局
  QHBoxLayout *tableWidgetLayout = new QHBoxLayout();
  tableWidgetLayout->setContentsMargins(0, 0, 10, 0);
  tableWidgetLayout->addWidget(_tableWidget);

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("NXTableWidget"));
  QVBoxLayout *centerVLayout = new QVBoxLayout(centralWidget);
  centerVLayout->setContentsMargins(0, 0, 0, 0);
  centerVLayout->addWidget(tableText);
  centerVLayout->addSpacing(10);
  centerVLayout->addLayout(tableWidgetLayout);
  centerVLayout->addStretch();
  addCentralWidget(centralWidget, true, false, 0);
}

T_TableWidget::~T_TableWidget()
{
}
