#include "NXAppBarPrivate.h"

#include "NXToolButton.h"
#ifdef Q_OS_WIN
#  include <Windows.h>
#endif
#include "NXAppBar.h"
#include "NXIconButton.h"
#include "NXNavigationBar.h"
#include "NXText.h"

#include <QApplication>
#include <QDebug>
#include <QGuiApplication>
#include <QLabel>
#include <QMenu>
#include <QPointer>
#include <QPropertyAnimation>
#include <QScreen>
#include <QVBoxLayout>
#include <QWidget>
#include <QWindow>
#include <QtMath>

NXAppBarPrivate::NXAppBarPrivate(QObject *parent)
    : QObject { parent }
{
}

NXAppBarPrivate::~NXAppBarPrivate()
{
}

void NXAppBarPrivate::onMinButtonClicked()
{
  Q_Q(NXAppBar);
  q->window()->showMinimized();
}

void NXAppBarPrivate::onMaxButtonClicked()
{
  Q_Q(NXAppBar);
  bool isMaximized = q->window()->isMaximized();
  isMaximized ? q->window()->showNormal() : q->window()->showMaximized();
#ifndef Q_OS_WIM
  _changeMaxButtonAwesome(!isMaximized);
#endif
}

void NXAppBarPrivate::onCloseButtonClicked()
{
  Q_Q(NXAppBar);
  if (m_IsDefaultClosed) {
    QPointer<QWidget> windowGuard = q->window();
    QPointer<QWindow> handleGuard = windowGuard ? windowGuard->windowHandle() : nullptr;
    if (windowGuard) {
      windowGuard->close();
    }
    QApplication::processEvents();
    if (handleGuard) {
      handleGuard->close();
    }
  } else {
    Q_EMIT q_ptr->closeButtonClicked();
  }
}

void NXAppBarPrivate::onStayTopButtonClicked()
{
#ifdef Q_OS_WIN
  HWND hwnd = (HWND) m_CurrentWinID;
  ::SetWindowPos(hwnd, m_IsStayTop ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#else
  Q_Q(NXAppBar);
  if (q->window()->isVisible()) {
    q->window()->windowHandle()->setFlag(Qt::WindowStaysOnTopHint, m_IsStayTop);
    q->window()->update();
  } else {
    q->window()->setWindowFlag(Qt::WindowStaysOnTopHint, m_IsStayTop);
  }
#endif
  m_StayTopButton->setIsSelected(m_IsStayTop);

  int currentRotate                   = m_StayTopButton->property("NXIconRotate").toInt();
  int targetRotate                    = m_IsStayTop ? 0 : 45;
  QPropertyAnimation *rotateAnimation = new QPropertyAnimation(m_StayTopButton, "NXIconRotate");
  rotateAnimation->setDuration(300);
  rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
  rotateAnimation->setStartValue(currentRotate);
  rotateAnimation->setEndValue(targetRotate);
  connect(rotateAnimation, &QPropertyAnimation::valueChanged, m_StayTopButton, [=]() {
    m_StayTopButton->update();
  });
  rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXAppBarPrivate::_changeMaxButtonAwesome(bool isMaximized)
{
  if (isMaximized) {
    m_MaxButton->setNXIcon(NXIconType::WindowRestore);
  } else {
    m_MaxButton->setNXIcon(NXIconType::Square);
  }
}

void NXAppBarPrivate::_showAppBarMenu(QPoint point)
{
  Q_Q(NXAppBar const);
  if (m_CustomMenu) {
    m_CustomMenu->exec(point);
  } else {
#ifdef Q_OS_WIN
    QScreen *screen = qApp->screenAt(QCursor::pos());
    if (!screen) {
      screen = QGuiApplication::primaryScreen();
    }
    if (!screen) {
      return;
    }
    QPoint const origin = screen->geometry().topLeft();
    auto nativePos      = QPointF(QPointF(point - origin) * screen->devicePixelRatio()).toPoint() + origin;
    HWND hwnd           = reinterpret_cast<HWND>(q->window()->winId());
    const HMENU hMenu   = ::GetSystemMenu(hwnd, FALSE);
    if (q->window()->isMaximized() || q->window()->isFullScreen()) {
      ::EnableMenuItem(hMenu, SC_MOVE, MFS_DISABLED);
      ::EnableMenuItem(hMenu, SC_RESTORE, MFS_ENABLED);
    } else {
      ::EnableMenuItem(hMenu, SC_MOVE, MFS_ENABLED);
      ::EnableMenuItem(hMenu, SC_RESTORE, MFS_DISABLED);
    }
    if (!m_IsFixedSize && !q->window()->isMaximized() && !q->window()->isFullScreen()) {
      ::EnableMenuItem(hMenu, SC_SIZE, MFS_ENABLED);
      ::EnableMenuItem(hMenu, SC_MAXIMIZE, MFS_ENABLED);
    } else {
      ::EnableMenuItem(hMenu, SC_SIZE, MFS_DISABLED);
      ::EnableMenuItem(hMenu, SC_MAXIMIZE, MFS_DISABLED);
    }
    int const result =
        ::TrackPopupMenu(hMenu, (TPM_RETURNCMD | (QGuiApplication::isRightToLeft() ? TPM_RIGHTALIGN : TPM_LEFTALIGN)), nativePos.x(), nativePos.y(), 0, hwnd, nullptr);
    if (result != FALSE) {
      ::PostMessageW(hwnd, WM_SYSCOMMAND, result, 0);
    }
#endif
  }
}

void NXAppBarPrivate::_updateCursor(int edges)
{
  Q_Q(NXAppBar const);
  switch (edges) {
  case 0 : {
    q->window()->setCursor(Qt::ArrowCursor);
    break;
  }
  case Qt::LeftEdge :
  case Qt::RightEdge : {
    q->window()->setCursor(Qt::SizeHorCursor);
    break;
  }
  case Qt::TopEdge :
  case Qt::BottomEdge : {
    q->window()->setCursor(Qt::SizeVerCursor);
    break;
  }
  case Qt::LeftEdge | Qt::TopEdge :
  case Qt::RightEdge | Qt::BottomEdge : {
    q->window()->setCursor(Qt::SizeFDiagCursor);
    break;
  }
  case Qt::RightEdge | Qt::TopEdge :
  case Qt::LeftEdge | Qt::BottomEdge : {
    q->window()->setCursor(Qt::SizeBDiagCursor);
    break;
  }
  default : {
    break;
  }
  }
}

bool NXAppBarPrivate::_containsCursorToItem(QWidget *item)
{
  Q_Q(NXAppBar const);
  if (!item || !item->isVisible()) {
    return false;
  }
  auto itemRect = QRect(item->mapToGlobal(QPoint(0, 0)), item->size());
  if (item == q) {
    for (int i = 0; i < m_ClientWidgetList.count(); i++) {
      if (_containsCursorToItem(m_ClientWidgetList[i])) {
        return false;
      }
    }
    for (int i = 0; i < m_CustomAreaWidgetList.count(); i++) {
      QWidget *customAreaWidget = m_CustomAreaWidgetList[i];
      if (_containsCursorToItem(customAreaWidget)) {
        QObject *customAreaHitTestObject = m_CustomAreaHitTestObjectList[i];
        if (customAreaHitTestObject) {
          bool isContainsInAppBar = false;
          QMetaObject::invokeMethod(customAreaHitTestObject, m_CustomAreaHitTestFunctionNameList[i].toLocal8Bit().constData(), Qt::AutoConnection, Q_RETURN_ARG(bool, isContainsInAppBar));
          return isContainsInAppBar;
        } else {
          return false;
        }
      }
    }
  } else if (item == m_MaxButton) {
    itemRect.adjust(0, 8, 0, 0);
  }
  if (itemRect.contains(QCursor::pos())) {
    return true;
  }
  return false;
}

void NXAppBarPrivate::_onThemeModeChange(NXThemeType::ThemeMode themeMode)
{
  if (themeMode == NXThemeType::Light) {
    m_ThemeChangeButton->setNXIcon(NXIconType::MoonStars);
  } else {
    m_ThemeChangeButton->setNXIcon(NXIconType::SunBright);
  }
}

int NXAppBarPrivate::_calculateMinimumWidth()
{
  Q_Q(NXAppBar);
  int appBarWidth = 0;
  if (m_TitleLabel->isVisible()) {
    appBarWidth += m_TitleLabel->width();
    appBarWidth += 10;
  }
  if (m_IconLabel->isVisible()) {
    appBarWidth += m_IconLabel->width();
    appBarWidth += 10;
  }
  for (int i = 0; i < m_CustomAreaWidgetList.count(); i++) {
    auto customAreaWidget = m_CustomAreaWidgetList[i];
    if (customAreaWidget->isVisible()) {
      appBarWidth += customAreaWidget->minimumWidth();
    }
  }
  for (auto const clientWidget : m_ClientWidgetList) {
    if (clientWidget->isVisible()) {
      appBarWidth += clientWidget->minimumWidth();
    }
  }
  auto windowMinWidth = q->parentWidget()->minimumWidth() + 20;
  return qMax(appBarWidth, windowMinWidth);
}

QVBoxLayout *NXAppBarPrivate::_createVLayout(QWidget *widget)
{
  if (!widget) {
    return nullptr;
  }
  QVBoxLayout *vLayout = new QVBoxLayout();
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->setSpacing(0);
  if (widget == m_IconLabel || widget == m_TitleLabel) {
    vLayout->addSpacing(6);
  }
  vLayout->addWidget(widget);
  vLayout->addStretch();
  return vLayout;
}
