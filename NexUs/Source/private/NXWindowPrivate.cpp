#include "NXWindowPrivate.h"

#include <QApplication>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>
#include <QtMath>

#include "NXAppBarPrivate.h"
#include "NXCentralStackedWidget.h"
#include "DeveloperComponents/NXThemeAnimationWidget.h"
#include "NXApplication.h"
#include "NXNavigationBar.h"
#include "NXTheme.h"
#include "NXWindow.h"

NXWindowPrivate::NXWindowPrivate(QObject *parent)
    : QObject { parent }
{
}

NXWindowPrivate::~NXWindowPrivate()
{
}

void NXWindowPrivate::onNavigationButtonClicked()
{
  if (m_IsNavigationBarFloat) {
    return;
  }
  auto currentDisplayMode = m_NavigationBar->getDisplayMode();
  if (currentDisplayMode == NXNavigationType::Minimal) {
    m_IsNavigationDisplayModeChanged = false;
    _resetWindowLayout(true);
    m_NavigationBar->setIsTransparent(false);
    m_NavigationBar->setDisplayMode(NXNavigationType::Maximal, false);
    m_NavigationBar->move(-m_NavigationBar->width(), m_NavigationBar->pos().y());
    m_NavigationBar->resize(m_NavigationBar->width(), m_NavigationCenterStackedWidget->height() + 1);
    QPropertyAnimation *navigationMoveAnimation = new QPropertyAnimation(m_NavigationBar, "pos");
    connect(navigationMoveAnimation, &QPropertyAnimation::finished, this, [=]() {
      m_IsNavigationBarExpanded = true;
    });
    navigationMoveAnimation->setEasingCurve(QEasingCurve::OutCubic);
    navigationMoveAnimation->setDuration(225);
    navigationMoveAnimation->setStartValue(m_NavigationBar->pos());
    navigationMoveAnimation->setEndValue(QPoint(0, 0));
    navigationMoveAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    m_IsNavigationBarFloat = true;
  } else {
    if (currentDisplayMode == NXNavigationType::Compact) {
      m_NavigationBar->setDisplayMode(NXNavigationType::Maximal);
    } else {
      m_NavigationBar->setDisplayMode(NXNavigationType::Compact);
    }
  }
}

void NXWindowPrivate::onWMWindowClickedEvent(QVariantMap const &data)
{
  NXAppBarType::WMMouseActionType actionType =
      data.value(QStringLiteral("WMClickType")).value<NXAppBarType::WMMouseActionType>();
  if (actionType == NXAppBarType::WMLBUTTONDBLCLK || actionType == NXAppBarType::WMLBUTTONUP) {
    if (NXApplication::containsCursorToItem(m_NavigationBar)) {
      return;
    }
    if (m_IsNavigationBarExpanded) {
      QPropertyAnimation *navigationMoveAnimation = new QPropertyAnimation(m_NavigationBar, "pos");
      connect(navigationMoveAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
        if (m_IsNavigationDisplayModeChanged) {
          m_IsNavigationBarFloat = false;
          _resetWindowLayout(false);
          navigationMoveAnimation->deleteLater();
        }
      });
      connect(navigationMoveAnimation, &QPropertyAnimation::finished, this, [=]() {
        if (!m_IsNavigationDisplayModeChanged) {
          m_NavigationBar->setDisplayMode(NXNavigationType::Minimal, false);
          _resetWindowLayout(false);
        }
        m_IsNavigationBarFloat = false;
      });
      navigationMoveAnimation->setEasingCurve(QEasingCurve::OutCubic);
      navigationMoveAnimation->setDuration(225);
      navigationMoveAnimation->setStartValue(m_NavigationBar->pos());
      navigationMoveAnimation->setEndValue(QPoint(-m_NavigationBar->width(), 0));
      navigationMoveAnimation->start(QAbstractAnimation::DeleteWhenStopped);
      m_IsNavigationBarExpanded = false;
    }
  }
}

void NXWindowPrivate::onThemeReadyChange()
{
  Q_Q(NXWindow);
  // 主题变更绘制窗口
  switch (nxApp->getWindowDisplayMode()) {
  case NXApplicationType::Normal :
  case NXApplicationType::NXMica : {
    if (m_ThemeChangeTime <= 0 || m_ThemeChangeTime > 3000) {
      if (nxTheme->getThemeMode() == NXThemeType::Light) {
        nxTheme->setThemeMode(NXThemeType::Dark);
      } else {
        nxTheme->setThemeMode(NXThemeType::Light);
      }
      break;
    }
    m_AppBar->setIsOnlyAllowMinAndClose(true);
    if (!m_AnimationWidget) {
      QPoint centerPos = q->mapFromGlobal(QCursor::pos());
      m_AnimationWidget = new NXThemeAnimationWidget(q);
      connect(m_AnimationWidget, &NXThemeAnimationWidget::animationFinished, this, [=]() {
        m_AppBar->setIsOnlyAllowMinAndClose(false);
        m_AnimationWidget = nullptr;
      });
      m_AnimationWidget->move(0, 0);
      m_AnimationWidget->setOldWindowBackground(q->grab(q->rect()).toImage());
      if (nxTheme->getThemeMode() == NXThemeType::Light) {
        nxTheme->setThemeMode(NXThemeType::Dark);
      } else {
        nxTheme->setThemeMode(NXThemeType::Light);
      }

      m_AnimationWidget->setCenter(centerPos);
      qreal topLeftDis     = _distance(centerPos, QPoint(0, 0));
      qreal topRightDis    = _distance(centerPos, QPoint(q->width(), 0));
      qreal bottomLeftDis  = _distance(centerPos, QPoint(0, q->height()));
      qreal bottomRightDis = _distance(centerPos, QPoint(q->width(), q->height()));
      QList<qreal> disList { topLeftDis, topRightDis, bottomLeftDis, bottomRightDis };
      std::sort(disList.begin(), disList.end());
      m_AnimationWidget->setEndRadius(disList[3]);
      m_AnimationWidget->resize(q->width(), q->height());
      m_AnimationWidget->startAnimation(m_ThemeChangeTime);
    }
    break;
  }
  default : {
    if (nxTheme->getThemeMode() == NXThemeType::Light) {
      nxTheme->setThemeMode(NXThemeType::Dark);
    } else {
      nxTheme->setThemeMode(NXThemeType::Light);
    }
    break;
  }
  }
}

void NXWindowPrivate::onThemeModeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXWindow);
  m_ThemeMode = themeMode;
  if (m_WindowPaintMode == NXWindowType::PaintMode::Movie) {
    if (m_WindowPaintMovie->state() == QMovie::Running) {
      m_WindowPaintMovie->stop();
    }
    m_WindowPaintMovie->setFileName(m_ThemeMode == NXThemeType::Light ? m_LightWindowMoviePath : m_DarkWindowMoviePath);
    m_WindowPaintMovie->start();
  }
  q->update();
}

void NXWindowPrivate::onWindowDisplayModeChanged()
{
  Q_Q(NXWindow);
  m_WindowDisplayMode = nxApp->getWindowDisplayMode();
  if (m_WindowPaintMovie->state() == QMovie::Running) {
    m_WindowPaintMovie->stop();
  }
  if (m_WindowDisplayMode == NXApplicationType::WindowDisplayMode::Normal && m_WindowPaintMode == NXWindowType::Movie) {
    m_WindowPaintMovie->setFileName(m_ThemeMode == NXThemeType::Light ? m_LightWindowMoviePath : m_DarkWindowMoviePath);
    m_WindowPaintMovie->start();
  }
  q->update();
}

void NXWindowPrivate::onNavigationNodeClicked(NXNavigationType::NavigationNodeType nodeType, QString const &nodeKey, bool isRouteBack)
{
  Q_Q(NXWindow);

  if (QWidget *page = m_RouteMap.value(nodeKey)) {
    int nodeIndex = m_NavigationCenterStackedWidget->getContainerStackedWidget()->indexOf(page);
    if (m_NavigationTargetIndex == nodeIndex ||
        m_NavigationCenterStackedWidget->getContainerStackedWidget()->count() <= nodeIndex) {
      return;
    }
    m_NavigationTargetIndex = nodeIndex;
    m_NavigationCenterStackedWidget->doWindowStackSwitch(m_StackSwitchMode, nodeIndex, isRouteBack);
  }
  Q_EMIT q->navigationNodeClicked(nodeType, nodeKey);
}

void NXWindowPrivate::onNavigationNodeAdded(NXNavigationType::NavigationNodeType nodeType, QString const &nodeKey, QWidget *page)
{
  if (nodeType == NXNavigationType::CategoryNode) {
    return;
  }
  m_RouteMap.insert(nodeKey, page);
  if (page) {
    m_NavigationCenterStackedWidget->getContainerStackedWidget()->addWidget(page);
  }
}

void NXWindowPrivate::onNavigationNodeRemoved(NXNavigationType::NavigationNodeType nodeType, QString const &nodeKey)
{
  Q_Q(NXWindow);
  auto it = m_RouteMap.find(nodeKey);
  if (it == m_RouteMap.end()) {
    return;
  }
  QWidget *page = it.value();
  m_RouteMap.remove(nodeKey);
  m_PageMetaMap.remove(nodeKey);
  m_NavigationCenterStackedWidget->getContainerStackedWidget()->removeWidget(page);
  page->deleteLater();
  QWidget *currentWidget = m_NavigationCenterStackedWidget->getContainerStackedWidget()->currentWidget();
  if (currentWidget) {
    q->navigation(currentWidget->property("NXPageKey").toString());
  }
  Q_EMIT q->navigationNodeRemoved(nodeType, nodeKey);
}

void NXWindowPrivate::onNavigationRouterStateChanged(NXNavigationRouterType::RouteMode routeMode)
{
  switch (routeMode) {
  case NXNavigationRouterType::BackValid : {
    m_AppBar->setRouteBackButtonEnable(true);
    break;
  }
  case NXNavigationRouterType::BackInvalid : {
    m_AppBar->setRouteBackButtonEnable(false);
    break;
  }
  case NXNavigationRouterType::ForwardValid : {
    m_AppBar->setRouteForwardButtonEnable(true);
    break;
  }
  case NXNavigationRouterType::ForwardInvalid : {
    m_AppBar->setRouteForwardButtonEnable(false);
    break;
  }
  }
}

void NXWindowPrivate::onNavigationRoute(QVariantMap const &routeData)
{
  Q_Q(NXWindow);
  int routeIndex           = -1;
  m_CentralStackTargetIndex = routeIndex;
  bool isRouteBack         = routeData.value(QStringLiteral("NXRouteBackMode")).toBool();
  if (isRouteBack) {
    routeIndex = routeData.value(QStringLiteral("NXBackCentralStackIndex")).toUInt();
  } else {
    routeIndex = routeData.value(QStringLiteral("NXForwardCentralStackIndex")).toUInt();
  }
  if (routeIndex != m_CenterStackedWidget->getContainerStackedWidget()->currentIndex()) {
    m_CenterStackedWidget->doWindowStackSwitch(m_StackSwitchMode, routeIndex, isRouteBack);
  }
}

qreal NXWindowPrivate::_distance(QPoint point1, QPoint point2)
{
  return std::sqrt((point1.x() - point2.x()) * (point1.x() - point2.x()) + (point1.y() - point2.y()) * (point1.y() - point2.y()));
}

void NXWindowPrivate::_resetWindowLayout(bool isAnimation)
{
  if (isAnimation) {
    while (m_CenterLayout->count() > 0) {
      m_CenterLayout->takeAt(0);
    }
  } else {
    if (m_CenterLayout->count() == 0) {
      m_NavigationBar->setIsTransparent(true);
      m_NavigationBar->setDisplayMode(NXNavigationType::Minimal, false);
      m_CenterLayout->addWidget(m_NavigationBar);
      m_CenterLayout->addWidget(m_NavigationCenterStackedWidget);
    }
  }
}

void NXWindowPrivate::_doNavigationDisplayModeChange()
{
  Q_Q(NXWindow);
  if (!m_IsNavigationEnable || !m_IsInitFinished) {
    return;
  }
  if (m_NavigationBarDisplayMode == NXNavigationType::Minimal) {
    _resetWindowLayout(false);
  }
  if (m_NavigationBarDisplayMode == NXNavigationType::Auto) {
    m_IsNavigationDisplayModeChanged = true;
    m_IsNavigationBarFloat           = false;
    _resetWindowLayout(false);
    int width = q->centralWidget()->width();
    if (width >= 850 && m_CurrentNavigationBarDisplayMode != NXNavigationType::Maximal) {
      m_NavigationBar->setDisplayMode(NXNavigationType::Maximal);
      m_CurrentNavigationBarDisplayMode = NXNavigationType::Maximal;
    } else if (width >= 550 && width < 850 && m_CurrentNavigationBarDisplayMode != NXNavigationType::Compact) {
      m_NavigationBar->setDisplayMode(NXNavigationType::Compact);
      m_CurrentNavigationBarDisplayMode = NXNavigationType::Compact;
    } else if (width < 550 && m_CurrentNavigationBarDisplayMode != NXNavigationType::Minimal) {
      m_NavigationBar->setDisplayMode(NXNavigationType::Minimal);
      m_CurrentNavigationBarDisplayMode = NXNavigationType::Minimal;
      m_AppBar->setWindowButtonFlag(NXAppBarType::NavigationButtonHint);
    }
    m_IsNavigationBarExpanded = false;
  }
}
