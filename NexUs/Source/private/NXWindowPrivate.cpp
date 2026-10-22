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
  if (_isNavigationBarFloat) {
    return;
  }
  auto currentDisplayMode = _navigationBar->getDisplayMode();
  if (currentDisplayMode == NXNavigationType::Minimal) {
    _isNavigationDisplayModeChanged = false;
    _resetWindowLayout(true);
    _navigationBar->setIsTransparent(false);
    _navigationBar->setDisplayMode(NXNavigationType::Maximal, false);
    _navigationBar->move(-_navigationBar->width(), _navigationBar->pos().y());
    _navigationBar->resize(_navigationBar->width(), _navigationCenterStackedWidget->height() + 1);
    QPropertyAnimation *navigationMoveAnimation = new QPropertyAnimation(_navigationBar, "pos");
    connect(navigationMoveAnimation, &QPropertyAnimation::finished, this, [=]() {
      _isNavigationBarExpanded = true;
    });
    navigationMoveAnimation->setEasingCurve(QEasingCurve::OutCubic);
    navigationMoveAnimation->setDuration(225);
    navigationMoveAnimation->setStartValue(_navigationBar->pos());
    navigationMoveAnimation->setEndValue(QPoint(0, 0));
    navigationMoveAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    _isNavigationBarFloat = true;
  } else {
    if (currentDisplayMode == NXNavigationType::Compact) {
      _navigationBar->setDisplayMode(NXNavigationType::Maximal);
    } else {
      _navigationBar->setDisplayMode(NXNavigationType::Compact);
    }
  }
}

void NXWindowPrivate::onWMWindowClickedEvent(const QVariantMap &data)
{
  NXAppBarType::WMMouseActionType actionType =
      data.value(QStringLiteral("WMClickType")).value<NXAppBarType::WMMouseActionType>();
  if (actionType == NXAppBarType::WMLBUTTONDBLCLK || actionType == NXAppBarType::WMLBUTTONUP) {
    if (NXApplication::containsCursorToItem(_navigationBar)) {
      return;
    }
    if (_isNavigationBarExpanded) {
      QPropertyAnimation *navigationMoveAnimation = new QPropertyAnimation(_navigationBar, "pos");
      connect(navigationMoveAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
        if (_isNavigationDisplayModeChanged) {
          _isNavigationBarFloat = false;
          _resetWindowLayout(false);
          navigationMoveAnimation->deleteLater();
        }
      });
      connect(navigationMoveAnimation, &QPropertyAnimation::finished, this, [=]() {
        if (!_isNavigationDisplayModeChanged) {
          _navigationBar->setDisplayMode(NXNavigationType::Minimal, false);
          _resetWindowLayout(false);
        }
        _isNavigationBarFloat = false;
      });
      navigationMoveAnimation->setEasingCurve(QEasingCurve::OutCubic);
      navigationMoveAnimation->setDuration(225);
      navigationMoveAnimation->setStartValue(_navigationBar->pos());
      navigationMoveAnimation->setEndValue(QPoint(-_navigationBar->width(), 0));
      navigationMoveAnimation->start(QAbstractAnimation::DeleteWhenStopped);
      _isNavigationBarExpanded = false;
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
    if (_pThemeChangeTime <= 0 || _pThemeChangeTime > 3000) {
      if (nxTheme->getThemeMode() == NXThemeType::Light) {
        nxTheme->setThemeMode(NXThemeType::Dark);
      } else {
        nxTheme->setThemeMode(NXThemeType::Light);
      }
      break;
    }
    _appBar->setIsOnlyAllowMinAndClose(true);
    if (!_animationWidget) {
      QPoint centerPos = q->mapFromGlobal(QCursor::pos());
      _animationWidget = new NXThemeAnimationWidget(q);
      connect(_animationWidget, &NXThemeAnimationWidget::animationFinished, this, [=]() {
        _appBar->setIsOnlyAllowMinAndClose(false);
        _animationWidget = nullptr;
      });
      _animationWidget->move(0, 0);
      _animationWidget->setOldWindowBackground(q->grab(q->rect()).toImage());
      if (nxTheme->getThemeMode() == NXThemeType::Light) {
        nxTheme->setThemeMode(NXThemeType::Dark);
      } else {
        nxTheme->setThemeMode(NXThemeType::Light);
      }

      _animationWidget->setCenter(centerPos);
      qreal topLeftDis     = _distance(centerPos, QPoint(0, 0));
      qreal topRightDis    = _distance(centerPos, QPoint(q->width(), 0));
      qreal bottomLeftDis  = _distance(centerPos, QPoint(0, q->height()));
      qreal bottomRightDis = _distance(centerPos, QPoint(q->width(), q->height()));
      QList<qreal> disList { topLeftDis, topRightDis, bottomLeftDis, bottomRightDis };
      std::sort(disList.begin(), disList.end());
      _animationWidget->setEndRadius(disList[3]);
      _animationWidget->resize(q->width(), q->height());
      _animationWidget->startAnimation(_pThemeChangeTime);
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
  _themeMode = themeMode;
  if (_pWindowPaintMode == NXWindowType::PaintMode::Movie) {
    if (_windowPaintMovie->state() == QMovie::Running) {
      _windowPaintMovie->stop();
    }
    _windowPaintMovie->setFileName(_themeMode == NXThemeType::Light ? _lightWindowMoviePath : _darkWindowMoviePath);
    _windowPaintMovie->start();
  }
  q->update();
}

void NXWindowPrivate::onWindowDisplayModeChanged()
{
  Q_Q(NXWindow);
  _windowDisplayMode = nxApp->getWindowDisplayMode();
  if (_windowPaintMovie->state() == QMovie::Running) {
    _windowPaintMovie->stop();
  }
  if (_windowDisplayMode == NXApplicationType::WindowDisplayMode::Normal && _pWindowPaintMode == NXWindowType::Movie) {
    _windowPaintMovie->setFileName(_themeMode == NXThemeType::Light ? _lightWindowMoviePath : _darkWindowMoviePath);
    _windowPaintMovie->start();
  }
  q->update();
}

void NXWindowPrivate::onNavigationNodeClicked(NXNavigationType::NavigationNodeType nodeType, const QString &nodeKey, bool isRouteBack)
{
  Q_Q(NXWindow);

  if (QWidget *page = _routeMap.value(nodeKey)) {
    int nodeIndex = _navigationCenterStackedWidget->getContainerStackedWidget()->indexOf(page);
    if (_navigationTargetIndex == nodeIndex ||
        _navigationCenterStackedWidget->getContainerStackedWidget()->count() <= nodeIndex) {
      return;
    }
    _navigationTargetIndex = nodeIndex;
    _navigationCenterStackedWidget->doWindowStackSwitch(_pStackSwitchMode, nodeIndex, isRouteBack);
  }
  Q_EMIT q->navigationNodeClicked(nodeType, nodeKey);
}

void NXWindowPrivate::onNavigationNodeAdded(NXNavigationType::NavigationNodeType nodeType, const QString &nodeKey, QWidget *page)
{
  if (nodeType == NXNavigationType::CategoryNode) {
    return;
  }
  _routeMap.insert(nodeKey, page);
  if (page) {
    _navigationCenterStackedWidget->getContainerStackedWidget()->addWidget(page);
  }
}

void NXWindowPrivate::onNavigationNodeRemoved(NXNavigationType::NavigationNodeType nodeType, const QString &nodeKey)
{
  Q_Q(NXWindow);
  auto it = _routeMap.find(nodeKey);
  if (it == _routeMap.end()) {
    return;
  }
  QWidget *page = it.value();
  _routeMap.remove(nodeKey);
  _pageMetaMap.remove(nodeKey);
  _navigationCenterStackedWidget->getContainerStackedWidget()->removeWidget(page);
  page->deleteLater();
  QWidget *currentWidget = _navigationCenterStackedWidget->getContainerStackedWidget()->currentWidget();
  if (currentWidget) {
    q->navigation(currentWidget->property("NXPageKey").toString());
  }
  Q_EMIT q->navigationNodeRemoved(nodeType, nodeKey);
}

void NXWindowPrivate::onNavigationRouterStateChanged(NXNavigationRouterType::RouteMode routeMode)
{
  switch (routeMode) {
  case NXNavigationRouterType::BackValid : {
    _appBar->setRouteBackButtonEnable(true);
    break;
  }
  case NXNavigationRouterType::BackInvalid : {
    _appBar->setRouteBackButtonEnable(false);
    break;
  }
  case NXNavigationRouterType::ForwardValid : {
    _appBar->setRouteForwardButtonEnable(true);
    break;
  }
  case NXNavigationRouterType::ForwardInvalid : {
    _appBar->setRouteForwardButtonEnable(false);
    break;
  }
  }
}

void NXWindowPrivate::onNavigationRoute(const QVariantMap &routeData)
{
  Q_Q(NXWindow);
  int routeIndex           = -1;
  _centralStackTargetIndex = routeIndex;
  bool isRouteBack         = routeData.value(QStringLiteral("NXRouteBackMode")).toBool();
  if (isRouteBack) {
    routeIndex = routeData.value(QStringLiteral("NXBackCentralStackIndex")).toUInt();
  } else {
    routeIndex = routeData.value(QStringLiteral("NXForwardCentralStackIndex")).toUInt();
  }
  if (routeIndex != _centerStackedWidget->getContainerStackedWidget()->currentIndex()) {
    _centerStackedWidget->doWindowStackSwitch(_pStackSwitchMode, routeIndex, isRouteBack);
  }
}

qreal NXWindowPrivate::_distance(QPoint point1, QPoint point2)
{
  return std::sqrt((point1.x() - point2.x()) * (point1.x() - point2.x()) + (point1.y() - point2.y()) * (point1.y() - point2.y()));
}

void NXWindowPrivate::_resetWindowLayout(bool isAnimation)
{
  if (isAnimation) {
    while (_centerLayout->count() > 0) {
      _centerLayout->takeAt(0);
    }
  } else {
    if (_centerLayout->count() == 0) {
      _navigationBar->setIsTransparent(true);
      _navigationBar->setDisplayMode(NXNavigationType::Minimal, false);
      _centerLayout->addWidget(_navigationBar);
      _centerLayout->addWidget(_navigationCenterStackedWidget);
    }
  }
}

void NXWindowPrivate::_doNavigationDisplayModeChange()
{
  Q_Q(NXWindow);
  if (!_isNavigationEnable || !_isInitFinished) {
    return;
  }
  if (_pNavigationBarDisplayMode == NXNavigationType::Minimal) {
    _resetWindowLayout(false);
  }
  if (_pNavigationBarDisplayMode == NXNavigationType::Auto) {
    _isNavigationDisplayModeChanged = true;
    _isNavigationBarFloat           = false;
    _resetWindowLayout(false);
    int width = q->centralWidget()->width();
    if (width >= 850 && _currentNavigationBarDisplayMode != NXNavigationType::Maximal) {
      _navigationBar->setDisplayMode(NXNavigationType::Maximal);
      _currentNavigationBarDisplayMode = NXNavigationType::Maximal;
    } else if (width >= 550 && width < 850 && _currentNavigationBarDisplayMode != NXNavigationType::Compact) {
      _navigationBar->setDisplayMode(NXNavigationType::Compact);
      _currentNavigationBarDisplayMode = NXNavigationType::Compact;
    } else if (width < 550 && _currentNavigationBarDisplayMode != NXNavigationType::Minimal) {
      _navigationBar->setDisplayMode(NXNavigationType::Minimal);
      _currentNavigationBarDisplayMode = NXNavigationType::Minimal;
      _appBar->setWindowButtonFlag(NXAppBarType::NavigationButtonHint);
    }
    _isNavigationBarExpanded = false;
  }
}
