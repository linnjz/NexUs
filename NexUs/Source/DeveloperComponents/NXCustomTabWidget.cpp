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
    , m_IsFinished { false }
    , m_CustomTabWidget { new NXTabWidget(this) }
    , m_CustomTabBar { new NXTabBar(this) }
{
  resize(700, 500);
  setWindowTitle(QStringLiteral(""));
#ifndef Q_OS_WIN
  setAttribute(Qt::WA_Hover);
#endif
  setMouseTracking(true);
  setWindowIcon(QIcon());

  m_CustomTabWidget->setIsTabTransparent(true);
  m_CustomTabWidget->setObjectName("NXCustomTabWidget");
  QTabBar *originTabBar = m_CustomTabWidget->tabBar();
  originTabBar->hide();
  setAcceptDrops(true);

  m_CustomTabBar->setObjectName("NXCustomTabBar");
  connect(m_CustomTabBar, &NXTabBar::tabMoved, this, [=](int from, int to) {
    m_CustomTabWidget->tabBar()->moveTab(from, to);
  });
  connect(m_CustomTabBar, &NXTabBar::currentChanged, this, [=](int index) {
    m_CustomTabWidget->setCurrentIndex(index);
  });
  connect(m_CustomTabWidget, &NXTabWidget::currentChanged, this, [=](int index) {
    if (index == -1) {
      m_IsFinished = true;
      hide();
    }
  });
  connect(m_CustomTabBar, &NXTabBar::tabCloseRequested, originTabBar, &QTabBar::tabCloseRequested);
  connect(m_CustomTabBar, &NXTabBar::tabBarTextChanged, originTabBar, [=](int tabIndex, QString const &tabText) {
    originTabBar->setTabText(tabIndex, tabText);
  });
  m_CustomTabWidget->d_ptr->m_CustomTabBar = m_CustomTabBar;
  connect(m_CustomTabBar, &NXTabBar::tabDragCreate, m_CustomTabWidget->d_func(), &NXTabWidgetPrivate::onTabDragCreate);
  connect(m_CustomTabBar, &NXTabBar::tabDragDrop, m_CustomTabWidget->d_func(), &NXTabWidgetPrivate::onTabDragDrop);
  connect(m_CustomTabBar, &NXTabBar::tabDragEnter, m_CustomTabWidget->d_func(), &NXTabWidgetPrivate::onTabDragEnter);
  connect(m_CustomTabBar, &NXTabBar::tabDragLeave, m_CustomTabWidget->d_func(), &NXTabWidgetPrivate::onTabDragLeave);
  
  QWidget *customWidget     = new QWidget(this);
  QVBoxLayout *customLayout = new QVBoxLayout(customWidget);
  customLayout->setContentsMargins(10, 0, 10, 0);
  customLayout->addStretch();
  customLayout->addWidget(m_CustomTabBar);
  m_AppBar->setCustomWidget(NXAppBarType::LeftArea, customWidget, this, QStringLiteral("processHitTest"));
  setCentralWidget(m_CustomTabWidget);
}

NXCustomTabWidget::~NXCustomTabWidget()
{
  while (m_CustomTabWidget->count() > 0) {
    QWidget *closeWidget         = m_CustomTabWidget->widget(0);
    NXTabWidget *originTabWidget = closeWidget->property("NXOriginTabWidget").value<NXTabWidget *>();
    closeWidget->setProperty("CurrentCustomBar", QVariant::fromValue<NXTabBar *>(nullptr));
    if (originTabWidget && originTabWidget != m_CustomTabWidget) {
      originTabWidget->addTab(closeWidget, m_CustomTabWidget->tabIcon(0), m_CustomTabWidget->tabText(0));
      originTabWidget->setCurrentWidget(closeWidget);
    } else {
      m_CustomTabWidget->removeTab(0);
    }
  }
}

void NXCustomTabWidget::addTab(QWidget *widget, QIcon const &tabIcon, QString const &tabTitle)
{
  m_CustomTabBar->addTab(tabIcon, tabTitle);
  m_CustomTabWidget->addTab(widget, tabIcon, tabTitle);
}

NXTabBar *NXCustomTabWidget::getCustomTabBar() const
{
  return m_CustomTabBar;
}

NXTabWidget *NXCustomTabWidget::getCustomTabWidget() const
{
  return m_CustomTabWidget;
}

bool NXCustomTabWidget::processHitTest()
{
  auto point = m_CustomTabBar->mapFromGlobal(QCursor::pos());
  return m_CustomTabBar->tabAt(point) < 0;
}
