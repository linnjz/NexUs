#include "NXActionCommanderView.h"

#include <QPainter>
#include <QPropertyAnimation>

#include "DeveloperComponents/NXActionCommanderViewStyle.h"
#include "NXActionCommander.h"
#include "NXMenu.h"
#include "NXScrollBar.h"
#include "NXTheme.h"

NXActionCommanderView::NXActionCommanderView(QWidget *parent)
    : QTreeView(parent)
{
  setObjectName("NXActionCommanderView");
  setStyleSheet(QStringLiteral("#NXActionCommanderView{border:0px;background-color:transparent;}"));
  setAnimated(true);
  setHeaderHidden(true);
  setAutoScroll(false);
  setMouseTracking(true);
  setSelectionMode(QAbstractItemView::NoSelection);

  // 滚动条设置
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  NXScrollBar *vScrollBar = new NXScrollBar(this);
  connect(vScrollBar, &NXScrollBar::rangeAnimationFinished, this, [=]() {
    doItemsLayout();
  });
  setVerticalScrollBar(vScrollBar);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  NXScrollBar *floatVScrollBar = new NXScrollBar(vScrollBar, this);
  floatVScrollBar->setIsAnimation(true);

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &NXActionCommanderView::customContextMenuRequested, this, &NXActionCommanderView::onCustomContextMenuRequested);

  _style = new NXActionCommanderViewStyle(style());
  setStyle(_style);
}

NXActionCommanderView::~NXActionCommanderView()
{
}

void NXActionCommanderView::onCustomContextMenuRequested(const QPoint &pos)
{
  auto modelIndex      = indexAt(pos);
  auto actionCommander = NXActionCommander::getInstance();
  NXMenu menu(this);
  menu.setMinimumWidth(200);
  menu.setMenuItemHeight(35);
  connect(menu.addNXIconAction(NXIconType::LocationArrow, QStringLiteral("定位到当前位置")), &QAction::triggered, this, [=]() {
    auto selectedIndexList = selectionModel()->selectedRows(0);
    if (selectedIndexList.isEmpty()) {
      return;
    }
    auto vScrollBar = verticalScrollBar();
    int startValue  = vScrollBar->value();
    scrollTo(selectedIndexList[0], QAbstractItemView::PositionAtCenter);
    int endValue                        = vScrollBar->value();
    QPropertyAnimation *scrollAnimation = new QPropertyAnimation(vScrollBar, "value");
    scrollAnimation->setEasingCurve(QEasingCurve::OutSine);
    scrollAnimation->setDuration(255);
    scrollAnimation->setStartValue(startValue);
    scrollAnimation->setEndValue(endValue);
    scrollAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  });
  connect(menu.addNXIconAction(NXIconType::ListTree, QStringLiteral("展开全部")), &QAction::triggered, this, [=]() {
    expandAll();
  });
  connect(menu.addNXIconAction(NXIconType::ListUl, QStringLiteral("收起全部")), &QAction::triggered, this, [=]() {
    collapseAll();
  });
  auto undoAction = menu.addNXIconAction(NXIconType::RotateLeft, QStringLiteral("撤销"));
  undoAction->setEnabled(actionCommander->getCommanderUndoState(_pDomainName) == NXActionCommanderType::UndoValid);
  connect(undoAction, &QAction::triggered, this, [=]() {
    actionCommander->undoCommand(_pDomainName);
  });
  auto redoAction = menu.addNXIconAction(NXIconType::RotateRight, QStringLiteral("重做"));
  redoAction->setEnabled(actionCommander->getCommanderRedoState(_pDomainName) == NXActionCommanderType::RedoValid);
  connect(redoAction, &QAction::triggered, this, [=]() {
    actionCommander->redoCommand(_pDomainName);
  });
  if (modelIndex.isValid()) {
    // 如果是次要行 定位到主要行
    if (modelIndex.parent().isValid()) {
      modelIndex = modelIndex.parent();
    }
    connect(menu.addNXIconAction(NXIconType::ArrowsRotate, QStringLiteral("运行到此处")), &QAction::triggered, this, [=]() {
      Q_EMIT runToCommand(_pDomainName, modelIndex.row());
    });
  }
  menu.exec(QCursor::pos());
}

void NXActionCommanderView::paintEvent(QPaintEvent *event)
{
  QTreeView::paintEvent(event);
  // 绘制底边线
  auto viewPort = viewport();
  QPainter painter(viewPort);
  painter.save();
  painter.setPen(NXThemeColor(_style->getThemeMode(), BasicBorder));
  QRect viewPortRect = viewPort->rect();
  painter.drawLine(viewPortRect.x(), viewPortRect.bottom(), viewPortRect.right(), viewPortRect.bottom());
  painter.restore();
}
