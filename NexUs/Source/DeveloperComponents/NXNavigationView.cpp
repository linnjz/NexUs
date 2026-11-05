#include "NXNavigationView.h"

#include <QDebug>
#include <QHeaderView>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QScroller>

#include "NXNavigationModel.h"
#include "NXNavigationNode.h"
#include "NXNavigationStyle.h"
#include "NXMenu.h"
#include "NXScrollBar.h"
#include "NXToolTip.h"

NXNavigationView::NXNavigationView(QWidget *parent)
    : QTreeView(parent)
{
  setObjectName("NXNavigationView");
  setStyleSheet(QStringLiteral("#NXNavigationView{background-color:transparent;}"));
  setAnimated(true);
  setHeaderHidden(true);
  setRootIsDecorated(false);
  setExpandsOnDoubleClick(false);
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
  floatVScrollBar->installEventFilter(this);

  m_NavigationStyle = new NXNavigationStyle(this->style());
  m_NavigationStyle->setNavigationView(this);
  setStyle(m_NavigationStyle);

  QScroller::grabGesture(this->viewport(), QScroller::LeftMouseButtonGesture);
  QScroller *scroller            = QScroller::scroller(this->viewport());
  QScrollerProperties properties = scroller->scrollerProperties();
  properties.setScrollMetric(QScrollerProperties::MousePressEventDelay, 0);
  properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
  properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOn);
  properties.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor, 0.35);
  properties.setScrollMetric(QScrollerProperties::OvershootScrollTime, 0.5);
  properties.setScrollMetric(QScrollerProperties::FrameRate, QScrollerProperties::Fps60);
  scroller->setScrollerProperties(properties);

  connect(scroller, &QScroller::stateChanged, this, [=](QScroller::State newstate) {
    if (newstate == QScroller::Pressed) {
      m_NavigationStyle->setPressIndex(indexAt(mapFromGlobal(QCursor::pos())));
      viewport()->update();
    } else if (newstate == QScroller::Scrolling || newstate == QScroller::Inactive) {
      m_NavigationStyle->setPressIndex(QModelIndex());
    }
  });

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &NXNavigationView::customContextMenuRequested, this, &NXNavigationView::onCustomContextMenuRequested);
}

NXNavigationView::~NXNavigationView()
{
}

NXToolTip *NXNavigationView::getCompactToolTip() const
{
  return m_CompactToolTip;
}

void NXNavigationView::setIsDragDropEnable(bool enable)
{
  setDragEnabled(enable);
  setAcceptDrops(enable);
  setDropIndicatorShown(enable);
  setDragDropMode(enable ? QAbstractItemView::InternalMove : QAbstractItemView::NoDragDrop);
}

void NXNavigationView::navigationNodeStateChange(QVariantMap const &data)
{
  this->m_NavigationStyle->navigationNodeStateChange(data);
}

void NXNavigationView::onCustomContextMenuRequested(QPoint pos)
{
  if (!m_NavigationBarPrivate->m_IsAllowPageOpenInNewWindow) {
    return;
  }
  QModelIndex posIndex = indexAt(pos);
  if (!posIndex.isValid()) {
    return;
  }
  NXNavigationNode *posNode = static_cast<NXNavigationNode *>(posIndex.internalPointer());
  if (!posNode->getIsExpanderNode() && !posNode->getIsCategoryNode()) {
    NXMenu menu;
    menu.setMenuItemHeight(27);
    QAction *openAction = menu.addNXIconAction(NXIconType::ObjectGroup, QStringLiteral("在新窗口中打开"));
    connect(openAction, &QAction::triggered, this, [=]() {
      Q_EMIT navigationOpenNewWindow(posNode->getNodeKey());
    });
    QAction *closeAction = menu.addNXIconAction(NXIconType::FilmSlash, QStringLiteral("关闭当前导航窗口"));
    connect(closeAction, &QAction::triggered, this, [=]() {
      Q_EMIT navigationCloseCurrentWindow(posNode->getNodeKey());
    });
    menu.exec(mapToGlobal(pos));
  }
}

void NXNavigationView::mouseMoveEvent(QMouseEvent *event)
{
  _doCompactToolTip();
  QTreeView::mouseMoveEvent(event);
}

void NXNavigationView::mouseDoubleClickEvent(QMouseEvent *event)
{
  m_NavigationStyle->setPressIndex(indexAt(event->pos()));
  viewport()->update();
  QTreeView::mouseDoubleClickEvent(event);
}

void NXNavigationView::mouseReleaseEvent(QMouseEvent *event)
{
  QTreeView::mouseReleaseEvent(event);
  if (event->button() == Qt::LeftButton) {
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
      Q_EMIT navigationClicked(index);
    }
    m_NavigationStyle->setPressIndex(QModelIndex());
  }
}

bool NXNavigationView::eventFilter(QObject *watched, QEvent *event)
{
  switch (event->type()) {
  case QEvent::MouseMove :
  case QEvent::HoverMove : {
    _doCompactToolTip();
    break;
  }
  default : {
    break;
  }
  }
  return QAbstractItemView::eventFilter(watched, event);
}

void NXNavigationView::_doCompactToolTip()
{
  if (m_NavigationBarPrivate->m_CurrentDisplayMode == NXNavigationType::NavigationDisplayMode::Compact) {
    if (!m_CompactToolTip) {
      m_CompactToolTip = new NXToolTip(this);
    }
    QModelIndex posIndex = indexAt(mapFromGlobal(QCursor::pos()));
    if (!posIndex.isValid()) {
      m_CompactToolTip->hide();
      return;
    }
    NXNavigationNode *posNode = static_cast<NXNavigationNode *>(posIndex.internalPointer());
    m_CompactToolTip->setToolTip(posNode->getNodeTitle());
    m_CompactToolTip->setOffSetX(10);
    m_CompactToolTip->updatePos();
    m_CompactToolTip->show();
  } else {
    if (m_CompactToolTip) {
      m_CompactToolTip->deleteLater();
      m_CompactToolTip = nullptr;
    }
  }
}
