#include "NXCustomTabWidget.h"

#include <QDebug>
#include <QEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>

#include "NXAppBar.h"
#include "NXTabBar.h"
#include "NXTabWidget.h"
#include "private/NXTabWidgetPrivate.h"

NXCustomTabWidget::NXCustomTabWidget(QWidget *parent)
    : NXCustomWidget(parent)
{
  _pIsFinished = false;
  resize(700, 500);
  setWindowTitle(QStringLiteral(""));
#ifndef Q_OS_WIN
  setAttribute(Qt::WA_Hover);
#endif
  setMouseTracking(true);
  setWindowIcon(QIcon());
  _customTabWidget = new NXTabWidget(this);
  _customTabWidget->setIsTabTransparent(true);
  _customTabWidget->setObjectName("NXCustomTabWidget");
  QTabBar *originTabBar = _customTabWidget->tabBar();
  originTabBar->hide();
  setAcceptDrops(true);
  _customTabBar = new NXTabBar(this);
  _customTabBar->setObjectName("NXCustomTabBar");
  connect(_customTabBar, &NXTabBar::tabMoved, this, [=](int from, int to) {
    _customTabWidget->tabBar()->moveTab(from, to);
  });
  connect(_customTabBar, &NXTabBar::currentChanged, this, [=](int index) {
    _customTabWidget->setCurrentIndex(index);
  });
  connect(_customTabWidget, &NXTabWidget::currentChanged, this, [=](int index) {
    if (index == -1) {
      _pIsFinished = true;
      hide();
    }
  });
  connect(_customTabBar, &NXTabBar::tabCloseRequested, originTabBar, &QTabBar::tabCloseRequested);
  connect(_customTabBar, &NXTabBar::tabBarTextChanged, originTabBar, [=](int tabIndex, const QString &tabText) {
    originTabBar->setTabText(tabIndex, tabText);
  });
  _customTabWidget->d_ptr->_customTabBar = _customTabBar;
  connect(_customTabBar, &NXTabBar::tabDragCreate, _customTabWidget->d_func(), &NXTabWidgetPrivate::onTabDragCreate);
  connect(_customTabBar, &NXTabBar::tabDragDrop, _customTabWidget->d_func(), &NXTabWidgetPrivate::onTabDragDrop);
  connect(_customTabBar, &NXTabBar::tabDragEnter, _customTabWidget->d_func(), &NXTabWidgetPrivate::onTabDragEnter);
  connect(_customTabBar, &NXTabBar::tabDragLeave, _customTabWidget->d_func(), &NXTabWidgetPrivate::onTabDragLeave);
  QWidget *customWidget     = new QWidget(this);
  QVBoxLayout *customLayout = new QVBoxLayout(customWidget);
  customLayout->setContentsMargins(10, 0, 10, 0);
  customLayout->addStretch();
  customLayout->addWidget(_customTabBar);
  _appBar->setCustomWidget(NXAppBarType::LeftArea, customWidget, this, QStringLiteral("processHitTest"));
  setCentralWidget(_customTabWidget);
}

NXCustomTabWidget::~NXCustomTabWidget()
{
  while (_customTabWidget->count() > 0) {
    QWidget *closeWidget         = _customTabWidget->widget(0);
    NXTabWidget *originTabWidget = closeWidget->property("NXOriginTabWidget").value<NXTabWidget *>();
    closeWidget->setProperty("CurrentCustomBar", QVariant::fromValue<NXTabBar *>(nullptr));
    if (originTabWidget && originTabWidget != _customTabWidget) {
      originTabWidget->addTab(closeWidget, _customTabWidget->tabIcon(0), _customTabWidget->tabText(0));
      originTabWidget->setCurrentWidget(closeWidget);
    } else {
      _customTabWidget->removeTab(0);
    }
  }
}

void NXCustomTabWidget::addTab(QWidget *widget, const QIcon &tabIcon, const QString &tabTitle)
{
  _customTabBar->addTab(tabIcon, tabTitle);
  _customTabWidget->addTab(widget, tabIcon, tabTitle);
}

NXTabBar *NXCustomTabWidget::getCustomTabBar() const
{
  return _customTabBar;
}

NXTabWidget *NXCustomTabWidget::getCustomTabWidget() const
{
  return _customTabWidget;
}

bool NXCustomTabWidget::processHitTest()
{
  auto point = _customTabBar->mapFromGlobal(QCursor::pos());
  return _customTabBar->tabAt(point) < 0;
}
