#include "T_TreeView.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>

#include "NXPushButton.h"
#include "NXScrollBar.h"
#include "NXSlider.h"
#include "NXText.h"
#include "NXTreeView.h"
#include "ModelView/T_TreeViewModel.h"

T_TreeView::T_TreeView(QWidget *parent)
    : T_BasePage(parent)
{
  // 预览窗口标题
  setWindowTitle(QStringLiteral("NXTreeView"));

  // 顶部元素
  createCustomWidget(QStringLiteral("树型视图被放置于此，可在此界面体验其效果并按需添加进项目中"));

  // NXTreeView
  T_TreeViewModel *treeModel = new T_TreeViewModel(this);
  QHBoxLayout *treeLayout    = new QHBoxLayout();
  treeLayout->setContentsMargins(0, 0, 10, 0);
  QWidget *treeSettingWidget           = new QWidget(this);
  QVBoxLayout *treeSettingWidgetLayout = new QVBoxLayout(treeSettingWidget);
  treeSettingWidgetLayout->setContentsMargins(0, 0, 0, 0);
  treeSettingWidgetLayout->setSpacing(15);

  // 数据统计
  NXText *dataText = new NXText(QString(QStringLiteral("树模型总数据条数：%1")).arg(treeModel->getItemCount()), this);
  dataText->setTextPixelSize(15);

  // ItemHeight
  NXText *itemHeightText = new NXText(QStringLiteral("ItemHeight"), this);
  itemHeightText->setTextPixelSize(15);
  NXSlider *itemHeightSlider = new NXSlider(this);
  itemHeightSlider->setRange(200, 600);
  itemHeightSlider->setValue(350);
  NXText *itemHeightValueText = new NXText(QStringLiteral("35"), this);
  itemHeightValueText->setTextPixelSize(15);
  connect(itemHeightSlider, &NXSlider::valueChanged, this, [=](int value) {
    itemHeightValueText->setText(QString::number(value / 10));
    _treeView->setItemHeight(value / 10);
  });
  QHBoxLayout *itemHeightLayout = new QHBoxLayout();
  itemHeightLayout->setContentsMargins(0, 0, 0, 0);
  itemHeightLayout->addWidget(itemHeightText);
  itemHeightLayout->addWidget(itemHeightSlider);
  itemHeightLayout->addWidget(itemHeightValueText);

  // HeaderMargin
  NXText *headerMarginText = new NXText(QStringLiteral("HeaderMargin"), this);
  headerMarginText->setTextPixelSize(15);
  NXSlider *headerMarginSlider = new NXSlider(this);
  headerMarginSlider->setRange(0, 200);
  headerMarginSlider->setValue(50);
  NXText *headerMarginValueText = new NXText(QStringLiteral("5"), this);
  headerMarginValueText->setTextPixelSize(15);
  connect(headerMarginSlider, &NXSlider::valueChanged, this, [=](int value) {
    headerMarginValueText->setText(QString::number(value / 10));
    _treeView->setHeaderMargin(value / 10);
  });
  QHBoxLayout *headerMarginLayout = new QHBoxLayout();
  headerMarginLayout->setContentsMargins(0, 0, 0, 0);
  headerMarginLayout->addWidget(headerMarginText);
  headerMarginLayout->addWidget(headerMarginSlider);
  headerMarginLayout->addWidget(headerMarginValueText);

  // Indentation
  NXText *indentationText = new NXText(QStringLiteral("Indentation"), this);
  indentationText->setTextPixelSize(15);
  NXSlider *indentationSlider = new NXSlider(this);
  indentationSlider->setRange(200, 1000);
  indentationSlider->setValue(200);
  NXText *indentationValueText = new NXText(QStringLiteral("20"), this);
  indentationValueText->setTextPixelSize(15);
  connect(indentationSlider, &NXSlider::valueChanged, this, [=](int value) {
    indentationValueText->setText(QString::number(value / 10));
    _treeView->setIndentation(value / 10);
  });
  QHBoxLayout *indentationLayout = new QHBoxLayout();
  indentationLayout->setContentsMargins(0, 0, 0, 0);
  indentationLayout->addWidget(indentationText);
  indentationLayout->addWidget(indentationSlider);
  indentationLayout->addWidget(indentationValueText);

  //展开全部
  QHBoxLayout *expandCollapseLayout = new QHBoxLayout();
  expandCollapseLayout->setContentsMargins(0, 0, 0, 0);
  NXPushButton *expandButton = new NXPushButton(QStringLiteral("展开全部"), this);
  expandButton->setFixedWidth(80);
  connect(expandButton, &NXPushButton::clicked, this, [=]() {
    _treeView->expandAll();
  });

  //收起全部
  NXPushButton *collapseButton = new NXPushButton(QStringLiteral("收起全部"), this);
  collapseButton->setFixedWidth(80);
  connect(collapseButton, &NXPushButton::clicked, this, [=]() {
    _treeView->collapseAll();
  });
  expandCollapseLayout->addWidget(expandButton);
  expandCollapseLayout->addWidget(collapseButton);
  expandCollapseLayout->addStretch();

  treeSettingWidgetLayout->addWidget(dataText);
  treeSettingWidgetLayout->addLayout(itemHeightLayout);
  treeSettingWidgetLayout->addLayout(headerMarginLayout);
  treeSettingWidgetLayout->addLayout(indentationLayout);
  treeSettingWidgetLayout->addLayout(expandCollapseLayout);
  treeSettingWidgetLayout->addStretch();

  // TreeView
  NXText *treeText = new NXText(QStringLiteral("NXTreeView"), this);
  treeText->setTextPixelSize(18);
  _treeView                           = new NXTreeView(this);
  NXScrollBar *treeViewFloatScrollBar = new NXScrollBar(_treeView->verticalScrollBar(), _treeView);
  treeViewFloatScrollBar->setIsAnimation(true);
  QFont headerFont = _treeView->header()->font();
  headerFont.setPixelSize(16);
  _treeView->header()->setFont(headerFont);
  _treeView->setFixedHeight(450);
  _treeView->setModel(treeModel);
  QVBoxLayout *treeViewLayout = new QVBoxLayout();
  treeViewLayout->setContentsMargins(0, 0, 0, 0);
  treeViewLayout->addWidget(_treeView);
  treeViewLayout->addStretch();
  treeLayout->addWidget(treeSettingWidget);
  treeLayout->addLayout(treeViewLayout);

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("NXView"));
  QVBoxLayout *centerVLayout = new QVBoxLayout(centralWidget);
  centerVLayout->setContentsMargins(0, 0, 0, 0);
  centerVLayout->addWidget(treeText);
  centerVLayout->addSpacing(10);
  centerVLayout->addLayout(treeLayout);
  addCentralWidget(centralWidget, true, false, 0);
}

T_TreeView::~T_TreeView()
{
}
