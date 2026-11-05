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
  d->m_Style = new NXTabBarStyle(style());
  setStyle(d->m_Style);

  d->m_TargetScrollOffset = 0;
  d->m_TabBarPrivate       = dynamic_cast<QTabBarPrivate *>(this->QTabBar::d_ptr.data());
  // 关闭自带滚动按钮，避免 layoutTabs 在 tab 溢出时重置 scrollOffset
  setUsesScrollButtons(false);
  connect(d, &NXTabBarPrivate::signalScrollOffsetChanged, this, [=]() {
    // 动画每帧将偏移应用到 QTabBar 官方滚动机制
    d->m_TabBarPrivate->scrollOffset = qRound(d->getScrollOffset());
    update();
  });
  // Qt 内部(refresh/makeVisible 等)修改 scrollOffset 时同步动画状态
  connect(this, &QTabBar::currentChanged, this, [=]() {
    d->setTargetScrollOffset(d->m_TabBarPrivate->scrollOffset);
    d->setScrollOffset(d->m_TabBarPrivate->scrollOffset);
  });
}

NXTabBar::~NXTabBar()
{
  Q_D(NXTabBar);
  delete d->m_Style;
}

void NXTabBar::locateCurrentTab()
{
  Q_D(NXTabBar);
  int const index = currentIndex();
  if (index < 0) {
    return;
  }
  int const tabWidth  = d->m_Style->getTabSize().width();
  int const maxOffset = qMax(0, d->m_TabBarPrivate->tabList.size() * tabWidth - width());
  d->setTargetScrollOffset(qBound(0.0, static_cast<qreal>(index * tabWidth), static_cast<qreal>(maxOffset)));
  d->startScrollAnimation();
}

void NXTabBar::setTabText(int index, QString const &text)
{
  QTabBar::setTabText(index, text);
  Q_EMIT tabBarTextChanged(index, text);
}

void NXTabBar::setTabBarStyle(NXTabBarType::TabBarStyle style)
{
  Q_D(NXTabBar);
  d->m_Style->setTabBarStyle(style);
}

NXTabBarType::TabBarStyle NXTabBar::getTabBarStyle() const noexcept(std::is_nothrow_copy_constructible_v<NXTabBarType::TabBarStyle>)
{
  Q_CONST_D(NXTabBar);
  return d->m_Style->getTabBarStyle();
}

void NXTabBar::setTabCornerRadius(int radius)
{
  Q_D(NXTabBar);
  d->m_Style->setTabCornerRadius(radius);
}

int NXTabBar::getTabCornerRadius() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXTabBar);
  return d->m_Style->getTabCornerRadius();
}

void NXTabBar::setIsSelectedIndicatorVisible(bool isVisible)
{
  Q_D(NXTabBar);
  d->m_Style->setIsSelectedIndicatorVisible(isVisible);
}

bool NXTabBar::getIsSelectedIndicatorVisible() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXTabBar);
  return d->m_Style->getIsSelectedIndicatorVisible();
}

void NXTabBar::setTabSize(QSize tabSize)
{
  Q_D(NXTabBar);
  d->m_Style->setTabSize(tabSize);
}

QSize NXTabBar::getTabSize() const noexcept(std::is_nothrow_copy_constructible_v<QSize>)
{
  Q_CONST_D(NXTabBar);
  return d->m_Style->getTabSize();
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
  // m_TabBarPrivate->scrollOffset 已被重置，动画状态仍是插入前的值
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
  if (d->m_TabBarPrivate->pressedIndex >= 0) {
    QPoint currentPos = event->pos();
    if (objectName() == QStringLiteral("NXCustomTabBar") && count() == 1) {
      if (!d->m_MimeData) {
        d->m_MimeData = new QMimeData();
        d->m_MimeData->setProperty("DragType", "NXTabBarDrag");
        d->m_MimeData->setProperty("NXTabBarObject", QVariant::fromValue(this));
        d->m_MimeData->setProperty("IsFloatWidget", true);
        d->m_MimeData->setProperty("IsSelectedIndicatorVisible", d->m_Style->getIsSelectedIndicatorVisible());
        d->m_MimeData->setProperty("TabCornerRadius", d->m_Style->getTabCornerRadius());
        d->m_MimeData->setProperty("TabBarStyle", d->m_Style->getTabBarStyle());
        d->m_MimeData->setProperty("TabSize", d->m_Style->getTabSize());
        QRect currentTabRect = tabRect(currentIndex());
        d->m_MimeData->setProperty("DragPos", QPoint(currentPos.x() - currentTabRect.x(), currentPos.y() - currentTabRect.y()));
        Q_EMIT tabDragCreate(d->m_MimeData);
        d->m_MimeData = nullptr;
      }
    } else {
      if (d->m_TabBarPrivate->dragInProgress) {
        auto &pressTabData   = d->m_TabBarPrivate->tabList[d->m_TabBarPrivate->pressedIndex];
        QRect firstTabRect   = tabRect(0);
        int const leftBound  = -firstTabRect.x();
        int const rightBound = width() - firstTabRect.x();
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
        if (!d->m_MimeData) {
          d->m_MimeData = new QMimeData();
          d->m_MimeData->setProperty("DragType", "NXTabBarDrag");
          d->m_MimeData->setProperty("NXTabBarObject", QVariant::fromValue(this));
          d->m_MimeData->setProperty("IsSelectedIndicatorVisible", d->m_Style->getIsSelectedIndicatorVisible());
          d->m_MimeData->setProperty("TabCornerRadius", d->m_Style->getTabCornerRadius());
          d->m_MimeData->setProperty("TabBarStyle", d->m_Style->getTabBarStyle());
          d->m_MimeData->setProperty("TabSize", d->m_Style->getTabSize());
          Q_EMIT tabDragCreate(d->m_MimeData);
          d->m_MimeData = nullptr;
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
    d->m_MimeData  = mimeData;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    mimeData->setProperty("TabDropIndex", tabAt(event->position().toPoint()));
#else
    mimeData->setProperty("TabDropIndex", tabAt(event->pos()));
#endif
    Q_EMIT tabDragEnter(mimeData);
    qApp->processEvents();

    QPoint localPressPos(tabRect(currentIndex()).x() + d->m_Style->getTabSize().width() / 2, 0);
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
  if (d->m_MimeData) {
    Q_EMIT tabDragLeave(d->m_MimeData);
    d->m_MimeData = nullptr;
  }
  QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(-1, -1), QPoint(-1, -1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
  QApplication::sendEvent(this, &releaseEvent);
  QTabBar::dragLeaveEvent(event);
}

void NXTabBar::dropEvent(QDropEvent *event)
{
  Q_D(NXTabBar);
  d->m_MimeData = nullptr;
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
  int maxOffset = qMax(0, d->m_TabBarPrivate->tabList.size() * d->m_Style->getTabSize().width() - width());
  int step      = qMax(1, d->m_Style->getTabSize().width() / 4);
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
  int maxOffset = qMax(0, d->m_TabBarPrivate->tabList.size() * d->m_Style->getTabSize().width() - width());
  d->setTargetScrollOffset(qMin(d->getTargetScrollOffset(), qreal(maxOffset)));
  d->setScrollOffset(qMin(d->getScrollOffset(), qreal(maxOffset)));
  d->m_TabBarPrivate->scrollOffset = qRound(d->getScrollOffset());
}

void NXTabBar::paintEvent(QPaintEvent *event)
{
  Q_D(NXTabBar);
  QSize tabSize = d->m_Style->getTabSize();
  for (int i = 0; i < d->m_TabBarPrivate->tabList.size(); i++) {
#if (QT_VERSION > QT_VERSION_CHECK(6, 0, 0))
    d->m_TabBarPrivate->tabList[i]->rect =
        QRect(tabSize.width() * i, d->m_TabBarPrivate->tabList[i]->rect.y(), tabSize.width(), tabSize.height());
#else
    d->m_TabBarPrivate->tabList[i].rect =
        QRect(tabSize.width() * i, d->m_TabBarPrivate->tabList[i].rect.y(), tabSize.width(), tabSize.height());
#endif
  }
  d->m_TabBarPrivate->layoutWidgets();
  QTabBar::paintEvent(event);
}
