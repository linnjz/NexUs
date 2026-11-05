#include "T_TableView.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>

#include "NXTableView.h"
#include "NXText.h"
#include "ModelView/T_TableViewModel.h"

T_TableView::T_TableView(QWidget *parent)
    : T_BasePage(parent)
{
  // 预览窗口标题
  setWindowTitle(QStringLiteral("NXTableView"));

  // 顶部元素
  createCustomWidget(QStringLiteral("表格视图被放置于此，可在此界面体验其效果并按需添加进项目中"));

  //NXTableView
  NXText *tableText = new NXText(QStringLiteral("NXTableView"), this);
  tableText->setTextPixelSize(18);
  _tableView = new NXTableView(this);
  // NXScrollBar* tableViewFloatScrollBar = new NXScrollBar(_tableView->verticalScrollBar(), _tableView);
  // tableViewFloatScrollBar->setIsAnimation(true);
  QFont tableHeaderFont = _tableView->horizontalHeader()->font();
  tableHeaderFont.setPixelSize(16);
  _tableView->horizontalHeader()->setFont(tableHeaderFont);
  _tableView->setModel(new T_TableViewModel(this));
  _tableView->setAlternatingRowColors(true);
  _tableView->setIconSize(QSize(38, 38));
  _tableView->verticalHeader()->setHidden(true);
  _tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
  _tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  _tableView->horizontalHeader()->setMinimumSectionSize(60);
  _tableView->verticalHeader()->setMinimumSectionSize(46);
  _tableView->setFixedHeight(450);
  connect(_tableView, &NXTableView::tableViewShow, this, [=]() {
    _tableView->setColumnWidth(0, 60);
    _tableView->setColumnWidth(1, 205);
    _tableView->setColumnWidth(2, 170);
    _tableView->setColumnWidth(3, 150);
    _tableView->setColumnWidth(4, 60);
  });
  QHBoxLayout *tableViewLayout = new QHBoxLayout();
  tableViewLayout->setContentsMargins(0, 0, 10, 0);
  tableViewLayout->addWidget(_tableView);

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("NXView"));
  QVBoxLayout *centerVLayout = new QVBoxLayout(centralWidget);
  centerVLayout->setContentsMargins(0, 0, 0, 0);
  centerVLayout->addWidget(tableText);
  centerVLayout->addSpacing(10);
  centerVLayout->addLayout(tableViewLayout);
  centerVLayout->addStretch();
  addCentralWidget(centralWidget, true, false, 0);
}

T_TableView::~T_TableView()
{
}
