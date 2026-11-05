#include "NXDockWidget.h"

#include <QEvent>
#include <QLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#include "DeveloperComponents/NXDockWidgetTitleBar.h"
#include "DeveloperComponents/NXWinShadowHelper.h"
#include "NXApplication.h"
#include "NXTheme.h"
#include "private/NXDockWidgetPrivate.h"

NXDockWidget::NXDockWidget(QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags)
    , d_ptr(new NXDockWidgetPrivate())
{
  Q_D(NXDockWidget);
  d->q_ptr = this;
  setObjectName("NXDockWidget");
  d->m_TitleBar = new NXDockWidgetTitleBar(this);
  setTitleBarWidget(d->m_TitleBar);

  // 主题变更动画
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXDockWidgetPrivate::onThemeModeChanged);

  d->m_WindowDisplayMode = nxApp->getWindowDisplayMode();
  connect(nxApp, &NXApplication::signalWindowDisplayModeChanged, this, [=]() {
    d->m_WindowDisplayMode = nxApp->getWindowDisplayMode();
    update();
  });
  connect(this, &NXDockWidget::topLevelChanged, this, [=](bool topLevel) {
    if (nxApp->getWindowDisplayMode() == NXApplicationType::WindowDisplayMode::NXMica) {
      nxApp->syncWindowDisplayMode(this, topLevel);
    }
  });
}

NXDockWidget::NXDockWidget(QString const &title, QWidget *parent, Qt::WindowFlags flags)
    : NXDockWidget(parent, flags)
{
  this->setWindowTitle(title);
}

NXDockWidget::~NXDockWidget()
{
}

void NXDockWidget::resizeEvent(QResizeEvent *event)
{
  Q_EMIT dockResized(this->size());
  QDockWidget::resizeEvent(event);
}

void NXDockWidget::closeEvent(QCloseEvent *event)
{
  Q_EMIT dockClosed();
  QDockWidget::closeEvent(event);
}

void NXDockWidget::paintEvent(QPaintEvent *event)
{
  Q_D(NXDockWidget);
  if (isFloating()) {
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing);
#ifdef Q_OS_WIN
    // 背景
    if (d->m_WindowDisplayMode != NXApplicationType::WindowDisplayMode::NXMica) {
      painter.setPen(Qt::NoPen);
      painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogBase));
      painter.drawRect(rect());
    }
#else
    // 背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(d->m_WindowDisplayMode == NXApplicationType::WindowDisplayMode::NXMica ? Qt::transparent : NXThemeColor(d->m_ThemeMode, DialogBase));
    QRect foregroundRect(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, width() - 2 * d->m_ShadowBorderWidth, height() - 2 * d->m_ShadowBorderWidth);
    painter.drawRect(rect());
#endif
    painter.restore();
  }
  QDockWidget::paintEvent(event);
}

#ifdef Q_OS_WIN
bool NXDockWidget::event(QEvent *event)
{
  Q_D(NXDockWidget);
  switch (event->type()) {
  case QEvent::Show :
  case QEvent::MouseButtonRelease : {
    HWND hwnd   = (HWND) d->m_CurrentWinID;
    DWORD style = ::GetWindowLongPtr(hwnd, GWL_STYLE);
    ::SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME);
    NXWinShadowHelper::getInstance()->setWindowShadow(d->m_CurrentWinID);
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 3) && QT_VERSION <= QT_VERSION_CHECK(6, 6, 1))
    bool hasCaption = (style & WS_CAPTION) == WS_CAPTION;
    if (!hasCaption) {
      ::SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_CAPTION);
    }
#  endif
    break;
  }
  default : {
    break;
  }
  }
  return QDockWidget::event(event);
}

#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool NXDockWidget::nativeEvent(QByteArray const &eventType, void *message, qintptr *result)
#  else
bool NXDockWidget::nativeEvent(QByteArray const &eventType, void *message, long *result)
#  endif
{
  Q_D(NXDockWidget);
  if ((eventType != QStringLiteral("windows_generic_MSG")) || !message) {
    return false;
  }
  auto const msg  = static_cast<const MSG *>(message);
  const HWND hwnd = msg->hwnd;
  if (!hwnd || !msg) {
    return false;
  }
  d->m_CurrentWinID    = (qint64) hwnd;
  const UINT uMsg     = msg->message;
  const WPARAM wParam = msg->wParam;
  const LPARAM lParam = msg->lParam;
  switch (uMsg) {
  case WM_WINDOWPOSCHANGING : {
    WINDOWPOS *wp = reinterpret_cast<WINDOWPOS *>(lParam);
    if (wp != nullptr && (wp->flags & SWP_NOSIZE) == 0) {
      wp->flags |= SWP_NOCOPYBITS;
      *result = ::DefWindowProcW(hwnd, uMsg, wParam, lParam);
      return true;
    }
    return false;
  }
  case WM_NCACTIVATE : {
    *result = TRUE;
    return true;
  }
  case WM_NCHITTEST : {
    POINT nativeLocalPos { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    ::ScreenToClient(hwnd, &nativeLocalPos);
    RECT clientRect { 0, 0, 0, 0 };
    ::GetClientRect(hwnd, &clientRect);
    auto clientWidth  = clientRect.right - clientRect.left;
    auto clientHeight = clientRect.bottom - clientRect.top;
    bool left         = nativeLocalPos.x < d->m_Margins;
    bool right        = nativeLocalPos.x > clientWidth - d->m_Margins;
    bool top          = nativeLocalPos.y < d->m_Margins;
    bool bottom       = nativeLocalPos.y > clientHeight - d->m_Margins;
    *result           = 0;
    if (!window()->isFullScreen() && !window()->isMaximized()) {
      if (left && top) {
        *result = HTTOPLEFT;
      } else if (left && bottom) {
        *result = HTBOTTOMLEFT;
      } else if (right && top) {
        *result = HTTOPRIGHT;
      } else if (right && bottom) {
        *result = HTBOTTOMRIGHT;
      } else if (left) {
        *result = HTLEFT;
      } else if (right) {
        *result = HTRIGHT;
      } else if (top) {
        *result = HTTOP;
      } else if (bottom) {
        *result = HTBOTTOM;
      }
    }
    if (0 != *result) {
      return true;
    }
    *result = HTCLIENT;
    return true;
  }
  case WM_NCCALCSIZE : {
    if (wParam == FALSE) {
      return false;
    }
    *result = 0;
    return true;
  }
  }
  return QDockWidget::nativeEvent(eventType, message, result);
}
#endif
