#include "NXTabBar.h"

#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QTimer>

#include "DeveloperComponents/NXTabBarStyle.h"
#include "private/NXTabBarPrivate.h"
#include "private/qtabbar_p.h"

NXTabBar::NXTabBar(QWidget *parent)
    : QTabBar(parent)
    , d_ptr(new NXTabBarPrivate())
{
  Q_D(NXTabBar);
  d->q_ptr = this;
  setObjectName("NXTabBar");
  setMouseTracking(true);
  setStyleSheet(QStringLiteral("#NXTabBar{background-color:transparent;}"));
  setTabsClosable(true);
  setMovable(true);
  setAcceptDrops(true);
  d->_style = new NXTabBarStyle(style());
  setStyle(d->_style);

  d->_pTargetScrollOffset = 0;
  d->_tabBarPrivate       = dynamic_cast<QTabBarPrivate *>(this->QTabBar::d_ptr.data());
  // 关闭自带滚动按钮，避免 layoutTabs 在 tab 溢出时重置 scrollOffset
  setUsesScrollButtons(false);
  connect(d, &NXTabBarPrivate::pScrollOffsetChanged, this, [=]() {
    // 动画每帧将偏移应用到 QTabBar 官方滚动机制
    d->_tabBarPrivate->scrollOffset = qRound(d->getScrollOffset());
    update();
  });
  // Qt 内部(refresh/makeVisible 等)修改 scrollOffset 时同步动画状态
  connect(this, &QTabBar::currentChanged, this, [=]() {
    d->setTargetScrollOffset(d->_tabBarPrivate->scrollOffset);
    d->setScrollOffset(d->_tabBarPrivate->scrollOffset);
  });
}

NXTabBar::~NXTabBar()
{
  Q_D(NXTabBar);
  delete d->_style;
}

void NXTabBar::locateCurrentTab()
{
  Q_D(NXTabBar);
  const int index = currentIndex();
  if (index < 0) {
    return;
  }
  const int tabWidth  = d->_style->getTabSize().width();
  const int maxOffset = qMax(0, d->_tabBarPrivate->tabList.size() * tabWidth - width());
  d->setTargetScrollOffset(qBound(0.0, static_cast<qreal>(index * tabWidth), static_cast<qreal>(maxOffset)));
  d->startScrollAnimation();
}

void NXTabBar::setTabText(int index, const QString &text)
{
  QTabBar::setTabText(index, text);
  Q_EMIT tabBarTextChanged(index, text);
}

void NXTabBar::setTabBarStyle(NXTabBarType::TabBarStyle style)
{
  Q_D(NXTabBar);
  d->_style->setTabBarStyle(style);
}

NXTabBarType::TabBarStyle NXTabBar::getTabBarStyle() const
{
  Q_D(const NXTabBar);
  return d->_style->getTabBarStyle();
}

void NXTabBar::setTabCornerRadius(int radius)
{
  Q_D(NXTabBar);
  d->_style->setTabCornerRadius(radius);
}

int NXTabBar::getTabCornerRadius() const
{
  Q_D(const NXTabBar);
  return d->_style->getTabCornerRadius();
}

void NXTabBar::setIsSelectedIndicatorVisible(bool isVisible)
{
  Q_D(NXTabBar);
  d->_style->setIsSelectedIndicatorVisible(isVisible);
}

bool NXTabBar::getIsSelectedIndicatorVisible() const
{
  Q_D(const NXTabBar);
  return d->_style->getIsSelectedIndicatorVisible();
}

void NXTabBar::setTabSize(QSize tabSize)
{
  Q_D(NXTabBar);
  d->_style->setTabSize(tabSize);
}

QSize NXTabBar::getTabSize() const
{
  Q_D(const NXTabBar);
  return d->_style->getTabSize();
}

QSize NXTabBar::minimumSizeHint() const
{
  // useScrollButtons=false 时基类会返回全部 tab 宽度总和，会撑大窗口，宽度最小设为 0 由布局分配
  return { 0, QTabBar::minimumSizeHint().height() };
}

void NXTabBar::tabInserted(int index)
{
  Q_D(NXTabBar);
  // 基类 addTab 内部先执行 refresh/makeVisible 再调用本虚函数，
  // _tabBarPrivate->scrollOffset 已被重置，动画状态仍是插入前的值
  qreal preScrollOffset = d->getScrollOffset();
  QTabBar::tabInserted(index);
  d->restoreScrollOffset(preScrollOffset);
}

void NXTabBar::tabRemoved(int index)
{
  Q_D(NXTabBar);
  // 记录移除前的偏移，基类 removeTab 的 refresh/makeVisible 会把偏移重置回最左
  qreal preScrollOffset = d->getScrollOffset();
  QTabBar::tabRemoved(index);
  // 保留拖出前的偏移，但不超过移除后的最大偏移
  d->restoreScrollOffset(preScrollOffset);
}

QSize NXTabBar::sizeHint() const
{
  QSize oldSize = QTabBar::sizeHint();
  QSize newSize = oldSize;
  newSize.setWidth(parentWidget()->maximumWidth());
  return oldSize.expandedTo(newSize);
}

void NXTabBar::mousePressEvent(QMouseEvent *event)
{
  if (objectName() == QStringLiteral("NXCustomTabBar") && event->button() == Qt::LeftButton && tabAt(event->pos()) < 0) {
    event->ignore();
    return;
  }
  QTabBar::mousePressEvent(event);
}

void NXTabBar::mouseMoveEvent(QMouseEvent *event)
{
  QTabBar::mouseMoveEvent(event);
  Q_D(NXTabBar);
  if (d->_tabBarPrivate->pressedIndex >= 0) {
    QPoint currentPos = event->pos();
    if (objectName() == QStringLiteral("NXCustomTabBar") && count() == 1) {
      if (!d->_mimeData) {
        d->_mimeData = new QMimeData();
        d->_mimeData->setProperty("DragType", "NXTabBarDrag");
        d->_mimeData->setProperty("NXTabBarObject", QVariant::fromValue(this));
        d->_mimeData->setProperty("IsFloatWidget", true);
        d->_mimeData->setProperty("IsSelectedIndicatorVisible", d->_style->getIsSelectedIndicatorVisible());
        d->_mimeData->setProperty("TabCornerRadius", d->_style->getTabCornerRadius());
        d->_mimeData->setProperty("TabBarStyle", d->_style->getTabBarStyle());
        d->_mimeData->setProperty("TabSize", d->_style->getTabSize());
        QRect currentTabRect = tabRect(currentIndex());
        d->_mimeData->setProperty("DragPos", QPoint(currentPos.x() - currentTabRect.x(), currentPos.y() - currentTabRect.y()));
        Q_EMIT tabDragCreate(d->_mimeData);
        d->_mimeData = nullptr;
      }
    } else {
      if (d->_tabBarPrivate->dragInProgress) {
        auto &pressTabData   = d->_tabBarPrivate->tabList[d->_tabBarPrivate->pressedIndex];
        QRect firstTabRect   = tabRect(0);
        const int leftBound  = -firstTabRect.x();
        const int rightBound = width() - firstTabRect.x();
#if (QT_VERSION > QT_VERSION_CHECK(6, 0, 0))
        QRect pressTabRect = pressTabData->rect;
        if (pressTabData->dragOffset > 0 && pressTabRect.right() + pressTabData->dragOffset > rightBound) {
          pressTabData->dragOffset = qMax(0, rightBound - pressTabRect.right());
        }
        if (pressTabData->dragOffset < 0 && pressTabRect.x() + pressTabData->dragOffset < leftBound) {
          pressTabData->dragOffset = qMin(0, leftBound - pressTabRect.x());
        }
#else
        QRect pressTabRect = pressTabData.rect;
        if (pressTabData.dragOffset > 0 && pressTabRect.right() + pressTabData.dragOffset > rightBound) {
          pressTabData.dragOffset = qMax(0, rightBound - pressTabRect.right());
        }
        if (pressTabData.dragOffset < 0 && pressTabRect.x() + pressTabData.dragOffset < leftBound) {
          pressTabData.dragOffset = qMin(0, leftBound - pressTabRect.x());
        }
#endif
      }

      QRect moveRect = rect();
      moveRect.adjust(0, -height(), 0, height());
      if (currentPos.x() < 0 || currentPos.x() > width() || currentPos.y() > moveRect.bottom() ||
          currentPos.y() < moveRect.y()) {
        if (!d->_mimeData) {
          d->_mimeData = new QMimeData();
          d->_mimeData->setProperty("DragType", "NXTabBarDrag");
          d->_mimeData->setProperty("NXTabBarObject", QVariant::fromValue(this));
          d->_mimeData->setProperty("IsSelectedIndicatorVisible", d->_style->getIsSelectedIndicatorVisible());
          d->_mimeData->setProperty("TabCornerRadius", d->_style->getTabCornerRadius());
          d->_mimeData->setProperty("TabBarStyle", d->_style->getTabBarStyle());
          d->_mimeData->setProperty("TabSize", d->_style->getTabSize());
          Q_EMIT tabDragCreate(d->_mimeData);
          d->_mimeData = nullptr;
        }
      }
    }
  }
}

void NXTabBar::dragEnterEvent(QDragEnterEvent *event)
{
  Q_D(NXTabBar);
  if (event->mimeData()->property("DragType").toString() == QStringLiteral("NXTabBarDrag")) {
    event->acceptProposedAction();
    auto mimeData = const_cast<QMimeData *>(event->mimeData());
    d->_mimeData  = mimeData;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    mimeData->setProperty("TabDropIndex", tabAt(event->position().toPoint()));
#else
    mimeData->setProperty("TabDropIndex", tabAt(event->pos()));
#endif
    Q_EMIT tabDragEnter(mimeData);
    qApp->processEvents();

    QPoint localPressPos(tabRect(currentIndex()).x() + d->_style->getTabSize().width() / 2, 0);
    QPoint globalPressPos = mapToGlobal(localPressPos);
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(localPressPos), QPointF(globalPressPos), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
#else
    QMouseEvent pressEvent(QEvent::MouseButtonPress, localPressPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
#endif

    QApplication::sendEvent(this, &pressEvent);

    QPoint localMovePos(event->position().x(), 0);
    QPoint globalMovePos = mapToGlobal(localMovePos);
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    QMouseEvent moveEvent(QEvent::MouseMove, QPointF(localMovePos), QPointF(globalMovePos), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
#else
    QMouseEvent moveEvent(QEvent::MouseMove, localMovePos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
#endif
    QApplication::sendEvent(this, &moveEvent);
  }
  QTabBar::dragEnterEvent(event);
}

void NXTabBar::dragMoveEvent(QDragMoveEvent *event)
{
  Q_D(NXTabBar);
  if (event->mimeData()->property("DragType").toString() == QStringLiteral("NXTabBarDrag")) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPoint movePos(event->position().toPoint().x(), 0);
    QMouseEvent moveEvent(QEvent::MouseMove, movePos, mapToGlobal(movePos), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
#else
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(event->pos().x(), 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
#endif
    QApplication::sendEvent(this, &moveEvent);
  }
  QWidget::dragMoveEvent(event);
}

void NXTabBar::dragLeaveEvent(QDragLeaveEvent *event)
{
  Q_D(NXTabBar);
  if (d->_mimeData) {
    Q_EMIT tabDragLeave(d->_mimeData);
    d->_mimeData = nullptr;
  }
  QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(-1, -1), QPoint(-1, -1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
  QApplication::sendEvent(this, &releaseEvent);
  QTabBar::dragLeaveEvent(event);
}

void NXTabBar::dropEvent(QDropEvent *event)
{
  Q_D(NXTabBar);
  d->_mimeData = nullptr;
  QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(-1, -1), QPoint(-1, -1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
  QApplication::sendEvent(this, &releaseEvent);
  if (objectName() != QStringLiteral("NXCustomTabBar")) {
    QMimeData *data = const_cast<QMimeData *>(event->mimeData());
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    data->setProperty("TabDropIndex", tabAt(event->position().toPoint()));
#else
    data->setProperty("TabDropIndex", tabAt(event->pos()));
#endif
    Q_EMIT tabDragDrop(data);
  }
  QTabBar::dropEvent(event);
}

void NXTabBar::wheelEvent(QWheelEvent *event)
{
  Q_D(NXTabBar);
  // 滚轮平滑横向滚动，不切换 tab
  int maxOffset = qMax(0, d->_tabBarPrivate->tabList.size() * d->_style->getTabSize().width() - width());
  int step      = qMax(1, d->_style->getTabSize().width() / 4);
  d->setTargetScrollOffset(
      qBound(0.0, d->getTargetScrollOffset() - event->angleDelta().y() / 120.0 * step, static_cast<qreal>(maxOffset)));
  d->startScrollAnimation();
  event->accept();
}

void NXTabBar::resizeEvent(QResizeEvent *event)
{
  QTabBar::resizeEvent(event);
  Q_D(NXTabBar);
  // 窗口变宽时收敛超出的滚动偏移
  int maxOffset = qMax(0, d->_tabBarPrivate->tabList.size() * d->_style->getTabSize().width() - width());
  d->setTargetScrollOffset(qMin(d->getTargetScrollOffset(), qreal(maxOffset)));
  d->setScrollOffset(qMin(d->getScrollOffset(), qreal(maxOffset)));
  d->_tabBarPrivate->scrollOffset = qRound(d->getScrollOffset());
}

void NXTabBar::paintEvent(QPaintEvent *event)
{
  Q_D(NXTabBar);
  QSize tabSize = d->_style->getTabSize();
  for (int i = 0; i < d->_tabBarPrivate->tabList.size(); i++) {
#if (QT_VERSION > QT_VERSION_CHECK(6, 0, 0))
    d->_tabBarPrivate->tabList[i]->rect =
        QRect(tabSize.width() * i, d->_tabBarPrivate->tabList[i]->rect.y(), tabSize.width(), tabSize.height());
#else
    d->_tabBarPrivate->tabList[i].rect =
        QRect(tabSize.width() * i, d->_tabBarPrivate->tabList[i].rect.y(), tabSize.width(), tabSize.height());
#endif
  }
  d->_tabBarPrivate->layoutWidgets();
  QTabBar::paintEvent(event);
}
