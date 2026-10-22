#include "NXTabWidgetPrivate.h"

#include <QApplication>
#include <QDebug>
#include <QMimeData>
#include <QMouseEvent>
#include <QTimer>
#include <QWindow>

#include "DeveloperComponents/NXCustomTabWidget.h"
#include "NXTabBar.h"
#include "NXTabWidget.h"

NXDragMonitor::NXDragMonitor(QObject *parent)
    : QObject(parent)
{
  _pIsInDrag = false;
}

NXDragMonitor::~NXDragMonitor()
{
}

NXTabWidgetPrivate::NXTabWidgetPrivate(QObject *parent)
    : QObject { parent }
{
}

NXTabWidgetPrivate::~NXTabWidgetPrivate()
{
}

void NXTabWidgetPrivate::onTabDragCreate(QMimeData *mimeData)
{
  Q_Q(NXTabWidget);
  if (NXDragMonitor::getInstance()->getIsInDrag()) {
    return;
  }
  NXDragMonitor::getInstance()->setIsInDrag(true);
  mimeData->setProperty("NXTabWidgetObject", QVariant::fromValue(q));
  int index           = q->currentIndex();
  QString tabText     = q->tabText(index);
  QIcon tabIcon       = q->tabIcon(index);
  QWidget *dragWidget = q->widget(index);
  dragWidget->setProperty("TabIcon", q->tabIcon(index));
  dragWidget->setProperty("TabText", q->tabText(index));
  NXTabWidget *originTabWidget = dragWidget->property("NXOriginTabWidget").value<NXTabWidget *>();
  QWidget *floatParentWidget   = originTabWidget;
  if (!originTabWidget && !dragWidget->property("IsMetaWidget").toBool()) {
    dragWidget->setProperty("NXOriginTabWidget", QVariant::fromValue<NXTabWidget *>(q));
    originTabWidget   = q;
    floatParentWidget = q;
  }
  mimeData->setProperty("DragWidget", QVariant::fromValue(dragWidget));
  NXTabBar *tabBarObject = mimeData->property("NXTabBarObject").value<NXTabBar *>();
  QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(-1, -1), QPoint(-1, -1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
  QApplication::sendEvent(tabBarObject, &releaseEvent);
  if (!floatParentWidget) {
    floatParentWidget = dragWidget->property("NXFloatParentWidget").value<QWidget *>();
  }
  bool isFloatWidget              = mimeData->property("IsFloatWidget").toBool();
  bool isSelectedIndicatorVisible = mimeData->property("IsSelectedIndicatorVisible").toBool();
  int tabCornerRadius             = mimeData->property("TabCornerRadius").toInt();
  NXTabBarType::TabBarStyle tabBarStyle =
      static_cast<NXTabBarType::TabBarStyle>(mimeData->property("TabBarStyle").toInt());
  QSize tabSize                  = mimeData->property("TabSize").toSize();
  NXCustomTabWidget *floatWidget = nullptr;
  if (isFloatWidget) {
    floatWidget = dynamic_cast<NXCustomTabWidget *>(q->window());
    if (floatWidget->windowHandle()) {
      floatWidget->windowHandle()->setFlag(Qt::WindowTransparentForInput, true);
    }
  } else {
    floatWidget = new NXCustomTabWidget(floatParentWidget);
    if (originTabWidget) {
      originTabWidget->d_func()->_registerCustomTabWidget(floatWidget);
    }
    q->removeTab(index);
    // 创建新窗口
    NXTabBar *originCustomTabBar = tabBarObject;
    if (originCustomTabBar && originCustomTabBar->objectName() == QStringLiteral("NXCustomTabBar")) {
      originCustomTabBar->removeTab(index);
    }
    NXTabBar *customTabBar = floatWidget->getCustomTabBar();
    customTabBar->setIsSelectedIndicatorVisible(isSelectedIndicatorVisible);
    customTabBar->setTabCornerRadius(tabCornerRadius);
    customTabBar->setTabBarStyle(tabBarStyle);
    customTabBar->setTabSize(tabSize);
    floatWidget->getCustomTabWidget()->setFloatWidgetSize(_pFloatWidgetSize);
    dragWidget->setProperty("CurrentCustomBar", QVariant::fromValue<NXTabBar *>(customTabBar));
    floatWidget->addTab(dragWidget, tabIcon, tabText);
    connect(customTabBar, &NXTabBar::currentChanged, this, [=](int currentIndex) {
      if (currentIndex < 0) {
        return;
      }
      Q_EMIT q->currentWidgetChanged(floatWidget->getCustomTabWidget()->widget(currentIndex));
    });
  }
  QPoint dragPos    = mimeData->property("DragPos").toPoint();
  QTimer *dragTimer = new QTimer(this);
  dragTimer->start(10);
  connect(dragTimer, &QTimer::timeout, floatWidget, [=]() {
    if (floatWidget->getIsFinished() && !isFloatWidget) {
      dragTimer->stop();
    } else {
      QPoint cursorPoint = QCursor::pos();
      if (isFloatWidget) {
        floatWidget->move(cursorPoint.x() - dragPos.x() - 10, cursorPoint.y() - dragPos.y() - 10);
      } else {
        floatWidget->move(cursorPoint.x() - tabSize.width() / 2 - 10, cursorPoint.y() - tabSize.height() / 2 - 10);
      }
    }
  });
  QDrag *drag = new QDrag(this);
  QPixmap pix(1, 1);
  pix.fill(Qt::transparent);
  drag->setPixmap(pix);
  drag->setMimeData(mimeData);
  connect(drag, &QDrag::destroyed, this, [=]() {
    dragTimer->deleteLater();
  });
  drag->setHotSpot(QPoint(tabSize.width() / 2, 0));
  QTimer::singleShot(1, this, [=]() {
    floatWidget->show();
    if (floatWidget->windowHandle()) {
      floatWidget->windowHandle()->setFlag(Qt::WindowTransparentForInput, true);
    }
    if (!isFloatWidget) {
      floatWidget->resize(_pFloatWidgetSize);
    }
  });
  auto ret = drag->exec();
  NXDragMonitor::getInstance()->setIsInDrag(false);
  NXCustomTabWidget *tempFloatWidget = mimeData->property("TempFloatWidget").value<NXCustomTabWidget *>();
  if (tempFloatWidget) {
    if (tempFloatWidget->windowHandle()) {
      tempFloatWidget->windowHandle()->setFlag(Qt::WindowTransparentForInput, false);
    }
    floatWidget->deleteLater();
  } else {
    if (floatWidget->isVisible()) {
      if (floatWidget->windowHandle()) {
        floatWidget->windowHandle()->setFlag(Qt::WindowTransparentForInput, false);
      }
    } else {
      floatWidget->deleteLater();
    }
  }
}

void NXTabWidgetPrivate::onTabDragEnter(QMimeData *mimeData)
{
  Q_Q(NXTabWidget);
  mimeData->setProperty("NXTabBarObject", QVariant::fromValue<NXTabBar *>(dynamic_cast<NXTabBar *>(q->tabBar())));
  onTabDragDrop(mimeData);
}

void NXTabWidgetPrivate::onTabDragLeave(QMimeData *mimeData)
{
  Q_Q(NXTabWidget);
  QWidget *dragWidget = mimeData->property("DragWidget").value<QWidget *>();
  int index           = q->indexOf(dragWidget);
  mimeData->setProperty("NXTabWidgetObject", QVariant::fromValue(q));
  QString tabText = q->tabText(index);
  QIcon tabIcon   = q->tabIcon(index);

  NXTabBar *tabBarObject = mimeData->property("NXTabBarObject").value<NXTabBar *>();
  mimeData->setProperty("DragWidget", QVariant::fromValue(dragWidget));
  q->removeTab(index);
  // 创建新窗口
  if (_customTabBar && _customTabBar != tabBarObject) {
    _customTabBar->removeTab(index);
  }
  NXTabWidget *originTabWidget = dragWidget->property("NXOriginTabWidget").value<NXTabWidget *>();
  QWidget *floatParentWidget   = originTabWidget;
  if (!floatParentWidget) {
    floatParentWidget = dragWidget->property("NXFloatParentWidget").value<QWidget *>();
  }
  NXCustomTabWidget *floatWidget = new NXCustomTabWidget(floatParentWidget);
  if (originTabWidget) {
    originTabWidget->d_func()->_registerCustomTabWidget(floatWidget);
  }
  bool isSelectedIndicatorVisible = mimeData->property("IsSelectedIndicatorVisible").toBool();
  int tabCornerRadius             = mimeData->property("TabCornerRadius").toInt();
  NXTabBarType::TabBarStyle tabBarStyle =
      static_cast<NXTabBarType::TabBarStyle>(mimeData->property("TabBarStyle").toInt());
  QSize tabSize          = mimeData->property("TabSize").toSize();
  NXTabBar *customTabBar = floatWidget->getCustomTabBar();
  customTabBar->setIsSelectedIndicatorVisible(isSelectedIndicatorVisible);
  customTabBar->setTabCornerRadius(tabCornerRadius);
  customTabBar->setTabBarStyle(tabBarStyle);
  customTabBar->setTabSize(tabSize);
  floatWidget->getCustomTabWidget()->setFloatWidgetSize(_pFloatWidgetSize);
  dragWidget->setProperty("CurrentCustomBar", QVariant::fromValue<NXTabBar *>(customTabBar));
  floatWidget->addTab(dragWidget, tabIcon, tabText);
  connect(customTabBar, &NXTabBar::currentChanged, this, [=](int currentIndex) {
    if (currentIndex < 0) {
      return;
    }
    Q_EMIT q->currentWidgetChanged(floatWidget->getCustomTabWidget()->widget(currentIndex));
  });
  floatWidget->show();
  floatWidget->resize(_pFloatWidgetSize);
  if (floatWidget->windowHandle()) {
    floatWidget->windowHandle()->setFlag(Qt::WindowTransparentForInput, true);
  }
  QPoint cursorPoint = QCursor::pos();
  floatWidget->move(cursorPoint.x() - tabSize.width() / 2, cursorPoint.y() - tabSize.height() / 2);
  QTimer *dragTimer = new QTimer(this);
  dragTimer->start(10);
  connect(dragTimer, &QTimer::timeout, floatWidget, [=]() {
    QPoint cursorPoint = QCursor::pos();
    floatWidget->move(cursorPoint.x() - tabSize.width() / 2 - 10, cursorPoint.y() - tabSize.height() / 2 - 10);
  });
  connect(mimeData, &QMimeData::destroyed, this, [=]() {
    dragTimer->deleteLater();
  });
  mimeData->setProperty("TempFloatWidget", QVariant::fromValue<NXCustomTabWidget *>(floatWidget));
}

void NXTabWidgetPrivate::onTabDragDrop(QMimeData *mimeData)
{
  Q_Q(NXTabWidget);
  QWidget *dragWidget = mimeData->property("DragWidget").value<QWidget *>();
  QString tabText     = dragWidget->property("TabText").toString();
  QIcon tabIcon       = dragWidget->property("TabIcon").value<QIcon>();
  int dropIndex       = mimeData->property("TabDropIndex").toInt();
  if (dropIndex < 0) {
    dropIndex = q->count();
  }
  q->insertTab(dropIndex, dragWidget, tabIcon, tabText);
  q->setCurrentWidget(dragWidget);
  if (_customTabBar) {
    dragWidget->setProperty("CurrentCustomBar", QVariant::fromValue<NXTabBar *>(_customTabBar));
    _customTabBar->insertTab(dropIndex, tabIcon, tabText);
    _customTabBar->setCurrentIndex(dropIndex);
  }
  if (const auto tempFloatWidget = mimeData->property("TempFloatWidget").value<NXCustomTabWidget *>()) {
    tempFloatWidget->deleteLater();
    mimeData->setProperty("TempFloatWidget", {});
  }
}

void NXTabWidgetPrivate::onTabCloseRequested(int index)
{
  Q_Q(NXTabWidget);
  QWidget *closeWidget         = q->widget(index);
  NXTabWidget *originTabWidget = closeWidget->property("NXOriginTabWidget").value<NXTabWidget *>();
  if (originTabWidget && originTabWidget != q) {
    NXTabBar *customTabBar = closeWidget->property("CurrentCustomBar").value<NXTabBar *>();
    if (customTabBar) {
      customTabBar->removeTab(index);
      closeWidget->setProperty("CurrentCustomBar", QVariant::fromValue<NXTabBar *>(nullptr));
    }
    originTabWidget->addTab(closeWidget, q->tabIcon(index), q->tabText(index));
    originTabWidget->setCurrentWidget(closeWidget);
  } else {
    if (!originTabWidget && q->objectName() == QStringLiteral("NXCustomTabWidget")) {
      _customTabBar->removeTab(index);
    }
    q->removeTab(index);
    _removeTabWidgetFromList(closeWidget);
    closeWidget->deleteLater();
  }
}

void NXTabWidgetPrivate::_clearAllTabWidgetList()
{
  Q_Q(NXTabWidget);
  for (auto widget : _allTabWidgetList) {
    if (!widget) {
      continue;
    }
    auto originTabWidgetVariant = widget->property("NXOriginTabWidget");
    if (originTabWidgetVariant.isValid() && originTabWidgetVariant.value<NXTabWidget *>() == q) {
      widget->setProperty("NXOriginTabWidget", QVariant());
    }
  }
  _allTabWidgetList.clear();
}

void NXTabWidgetPrivate::_addTabWidgetToList(QWidget *widget)
{
  if (!widget) {
    return;
  }
  for (const auto &listWidget : _allTabWidgetList) {
    if (listWidget == widget) {
      return;
    }
  }
  _allTabWidgetList.append(QPointer<QWidget>(widget));
  connect(widget, &QObject::destroyed, this, [=]() {
    _removeTabWidgetFromList(widget);
  });
}

void NXTabWidgetPrivate::_removeTabWidgetFromList(QWidget *widget)
{
  for (int i = _allTabWidgetList.size() - 1; i >= 0; --i) {
    QPointer<QWidget> listWidget = _allTabWidgetList.at(i);
    if (listWidget.isNull() || listWidget == widget) {
      _allTabWidgetList.removeAt(i);
    }
  }
}

void NXTabWidgetPrivate::_registerCustomTabWidget(NXCustomTabWidget *customTabWidget)
{
  if (!customTabWidget) {
    return;
  }
  for (const auto &listCustomTabWidget : _customTabWidgetList) {
    if (listCustomTabWidget == customTabWidget) {
      return;
    }
  }
  _customTabWidgetList.append(QPointer<NXCustomTabWidget>(customTabWidget));
  NXTabBar *customTabBar = customTabWidget->getCustomTabBar();
  connect(customTabWidget, &QObject::destroyed, this, [=]() {
    _unregisterCustomTabWidget(customTabWidget, customTabBar);
  });
}

void NXTabWidgetPrivate::_unregisterCustomTabWidget(NXCustomTabWidget *customTabWidget, NXTabBar *customTabBar)
{
  if (customTabBar) {
    for (auto &widget : _allTabWidgetList) {
      if (!widget) {
        continue;
      }
      if (widget->property("CurrentCustomBar").value<NXTabBar *>() == customTabBar) {
        widget->setProperty("CurrentCustomBar", QVariant::fromValue<NXTabBar *>(nullptr));
      }
    }
  }
  for (int i = _customTabWidgetList.size() - 1; i >= 0; --i) {
    QPointer<NXCustomTabWidget> listCustomTabWidget = _customTabWidgetList.at(i);
    if (listCustomTabWidget.isNull() || listCustomTabWidget == customTabWidget) {
      _customTabWidgetList.removeAt(i);
    }
  }
}
