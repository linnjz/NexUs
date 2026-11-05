#include "NXAppBar.h"

#include <QApplication>
#include <QDebug>

#include "NXText.h"
#include "NXToolButton.h"
#include "NXWinShadowHelper.h"

#ifndef Q_OS_WIN
#  include <QWindow>
#endif
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>

#include "NXDef.h"
#include "NXEventBus.h"
#include "NXIconButton.h"
#include "NXTheme.h"
#include "private/NXAppBarPrivate.h"
Q_PROPERTY_CREATE_CPP(NXAppBar, QPS_PAEMIT(bool), IsStayTop)
Q_PROPERTY_CREATE_CPP(NXAppBar, QPS_PAEMIT(bool), IsDefaultClosed)
Q_PROPERTY_CREATE_CPP(NXAppBar, QPS_PAEMIT(bool), IsOnlyAllowMinAndClose)

NXAppBar::NXAppBar(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXAppBarPrivate())
{
  Q_D(NXAppBar);
  d->m_ButtonFlags = NXAppBarType::RouteBackButtonHint | NXAppBarType::RouteForwardButtonHint |
                    NXAppBarType::StayTopButtonHint | NXAppBarType::ThemeChangeButtonHint |
                    NXAppBarType::MinimizeButtonHint | NXAppBarType::MaximizeButtonHint | NXAppBarType::CloseButtonHint;
  window()->setAttribute(Qt::WA_Mapped);
  d->m_AppBarHeight = 45;
  d->m_RibbonHeight = 0;
  setFixedHeight(d->m_AppBarHeight);
  window()->setContentsMargins(0, this->height(), 0, 0);
  d->q_ptr                    = this;
  d->m_IsStayTop              = false;
  d->m_IsFixedSize            = false;
  d->m_IsDefaultClosed        = true;
  d->m_IsOnlyAllowMinAndClose = false;
  d->m_CustomMenu             = nullptr;
  window()->installEventFilter(this);
#ifdef Q_OS_WIN
  if (!nxWinHelper->getIsWinVersionGreater10()) {
    d->m_Win7Margins = 8;
  }
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 3) && QT_VERSION <= QT_VERSION_CHECK(6, 6, 1))
  window()->setWindowFlags((window()->windowFlags()) | Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);
#  endif
#else
  window()->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint | Qt::WindowFullscreenButtonHint | Qt::WindowSystemMenuHint);
#endif
  setMouseTracking(true);
  setObjectName("NXAppBar");
  setStyleSheet(QStringLiteral("#NXAppBar{background-color:transparent;}"));
  d->m_RouteBackButton = new NXToolButton(this);
  d->m_RouteBackButton->setBorderRadius(0);
  d->m_RouteBackButton->setNXIcon(NXIconType::ArrowLeft);
  d->m_RouteBackButton->setFixedSize(35, 30);
  d->m_RouteBackButton->setEnabled(false);
  // 路由跳转
  connect(d->m_RouteBackButton, &NXIconButton::clicked, this, &NXAppBar::routeBackButtonClicked);

  d->m_RouteForwardButton = new NXToolButton(this);
  d->m_RouteForwardButton->setBorderRadius(0);
  d->m_RouteForwardButton->setNXIcon(NXIconType::ArrowRight);
  d->m_RouteForwardButton->setFixedSize(35, 30);
  d->m_RouteForwardButton->setEnabled(false);
  connect(d->m_RouteForwardButton, &NXToolButton::clicked, this, &NXAppBar::routeForwardButtonClicked);

  // 导航栏展开按钮
  d->m_NavigationButton = new NXToolButton(this);
  d->m_NavigationButton->setBorderRadius(0);
  d->m_NavigationButton->setNXIcon(NXIconType::Bars);
  d->m_NavigationButton->setFixedSize(40, 30);
  d->m_NavigationButton->setObjectName("NavigationButton");
  d->m_NavigationButton->setVisible(false);
  // 展开导航栏
  connect(d->m_NavigationButton, &NXToolButton::clicked, this, &NXAppBar::navigationButtonClicked);

  // 设置置顶
  d->m_StayTopButton = new NXToolButton(this);
  d->m_StayTopButton->setBorderRadius(0);
  d->m_StayTopButton->setNXIcon(NXIconType::Thumbtack, 45);
  d->m_StayTopButton->setFixedSize(40, 30);
  connect(d->m_StayTopButton, &NXToolButton::clicked, this, [=]() {
    this->setIsStayTop(!this->getIsStayTop());
  });
  connect(this, &NXAppBar::signalIsStayTopChanged, d, &NXAppBarPrivate::onStayTopButtonClicked);

  //图标
  d->m_IconLabel       = new QLabel(this);
  d->m_IconLabelLayout = d->_createVLayout(d->m_IconLabel);
  if (parent->windowIcon().isNull()) {
    d->m_IconLabel->setVisible(false);
  } else {
    d->m_IconLabel->setPixmap(parent->windowIcon().pixmap(18, 18));
    d->m_IconLabelLayout->setContentsMargins(10, 0, 0, 0);
  }
  connect(parent, &QWidget::windowIconChanged, this, [=](QIcon const &icon) {
    d->m_IconLabel->setPixmap(icon.pixmap(18, 18));
    d->m_IconLabel->setVisible(!icon.isNull());
    d->m_IconLabelLayout->setContentsMargins(icon.isNull() ? 0 : 10, 0, 0, 0);
  });

  //标题
  d->m_TitleLabel = new NXText(this);
  d->m_TitleLabel->setIsWrapAnywhere(false);
  d->m_TitleLabel->setTextPixelSize(13);
  d->m_TitleLabelLayout = d->_createVLayout(d->m_TitleLabel);
  if (parent->windowTitle().isEmpty()) {
    d->m_TitleLabel->setVisible(false);
  } else {
    d->m_TitleLabel->setText(parent->windowTitle());
    d->m_TitleLabelLayout->setContentsMargins(10, 0, 0, 0);
  }
  connect(parent, &QWidget::windowTitleChanged, this, [=](QString const &title) {
    d->m_TitleLabel->setText(title);
    d->m_TitleLabel->setVisible(!title.isEmpty());
    d->m_TitleLabelLayout->setContentsMargins(title.isEmpty() ? 0 : 10, 0, 0, 0);
  });

  // 主题变更
  d->m_ThemeChangeButton = new NXToolButton(this);
  d->m_ThemeChangeButton->setBorderRadius(0);
  d->m_ThemeChangeButton->setNXIcon(NXIconType::MoonStars);
  d->m_ThemeChangeButton->setFixedSize(40, 30);
  connect(d->m_ThemeChangeButton, &NXToolButton::clicked, this, &NXAppBar::themeChangeButtonClicked);
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_onThemeModeChange(themeMode);
  });

  d->m_MinButton = new NXToolButton(this);
  d->m_MinButton->setBorderRadius(0);
  d->m_MinButton->setNXIcon(NXIconType::Dash);
  d->m_MinButton->setFixedSize(40, 30);
  connect(d->m_MinButton, &NXToolButton::clicked, d, &NXAppBarPrivate::onMinButtonClicked);
  d->m_MaxButton = new NXToolButton(this);
  d->m_MaxButton->setBorderRadius(0);
  d->m_MaxButton->setIconSize(QSize(18, 18));
  d->m_MaxButton->setNXIcon(NXIconType::Square);
  d->m_MaxButton->setFixedSize(40, 30);
  connect(d->m_MaxButton, &NXToolButton::clicked, d, &NXAppBarPrivate::onMaxButtonClicked);
  d->m_CloseButton = new NXIconButton(NXIconType::Xmark, 18, 40, 30, this);
  d->m_CloseButton->setLightHoverColor(QColor(0xE8, 0x11, 0x23));
  d->m_CloseButton->setDarkHoverColor(QColor(0xE8, 0x11, 0x23));
  d->m_CloseButton->setLightHoverIconColor(Qt::white);
  d->m_CloseButton->setDarkHoverIconColor(Qt::white);
  connect(d->m_CloseButton, &NXIconButton::clicked, d, &NXAppBarPrivate::onCloseButtonClicked);

  d->m_MainLayout = new QHBoxLayout(this);
  d->m_MainLayout->setContentsMargins(0, 0, 0, 0);
  d->m_MainLayout->setSpacing(0);
  QHBoxLayout *leftLayout = new QHBoxLayout();
  leftLayout->setSpacing(0);
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->setAlignment(Qt::AlignLeft);
  leftLayout->addLayout(d->_createVLayout(d->m_RouteBackButton));
  leftLayout->addLayout(d->_createVLayout(d->m_RouteForwardButton));
  leftLayout->addLayout(d->_createVLayout(d->m_NavigationButton));
  leftLayout->addLayout(d->m_IconLabelLayout);
  leftLayout->addLayout(d->m_TitleLabelLayout);
  d->m_MainLayout->addLayout(leftLayout);

  auto leftAreaWidget = new QWidget(this);
  leftAreaWidget->setVisible(false);
  auto middleAreaWidget = new QWidget(this);
  middleAreaWidget->setVisible(false);
  auto rightAreaWidget = new QWidget(this);
  rightAreaWidget->setVisible(false);
  d->m_CustomAreaWidgetList[0] = leftAreaWidget;
  d->m_CustomAreaWidgetList[1] = middleAreaWidget;
  d->m_CustomAreaWidgetList[2] = rightAreaWidget;
  d->m_MainLayout->addWidget(leftAreaWidget);
  d->m_MainLayout->addStretch();
  d->m_MainLayout->addWidget(middleAreaWidget);
  d->m_MainLayout->addStretch();
  d->m_MainLayout->addWidget(rightAreaWidget);

  QHBoxLayout *rightLayout = new QHBoxLayout();
  rightLayout->setSpacing(0);
  rightLayout->setContentsMargins(0, 0, 0, 0);
  rightLayout->setAlignment(Qt::AlignRight);
  rightLayout->addLayout(d->_createVLayout(d->m_StayTopButton));
  rightLayout->addLayout(d->_createVLayout(d->m_ThemeChangeButton));
  rightLayout->addLayout(d->_createVLayout(d->m_MinButton));
  rightLayout->addLayout(d->_createVLayout(d->m_MaxButton));
  rightLayout->addLayout(d->_createVLayout(d->m_CloseButton));
  d->m_MainLayout->addLayout(rightLayout);

  d->m_ClientWidgetList.append(d->m_RouteBackButton);
  d->m_ClientWidgetList.append(d->m_RouteForwardButton);
  d->m_ClientWidgetList.append(d->m_NavigationButton);
  d->m_ClientWidgetList.append(d->m_StayTopButton);
  d->m_ClientWidgetList.append(d->m_ThemeChangeButton);
  d->m_ClientWidgetList.append(d->m_MinButton);
  d->m_ClientWidgetList.append(d->m_MaxButton);
  d->m_ClientWidgetList.append(d->m_CloseButton);

#ifdef Q_OS_WIN
  for (int i = 0; i < qApp->screens().count(); i++) {
    connect(qApp->screens().at(i), &QScreen::logicalDotsPerInchChanged, this, [=] {
      if (d->m_IsFixedSize) {
        HWND hwnd = (HWND) (d->m_CurrentWinID);
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_FRAMECHANGED);
      }
    });
  }
  //主屏幕变更处理
  connect(qApp, &QApplication::primaryScreenChanged, this, [=]() {
    HWND hwnd = (HWND) (d->m_CurrentWinID);
    ::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    ::RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
  });
  d->m_LastScreen = qApp->screenAt(window()->geometry().center());
#endif

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXAppBar::~NXAppBar()
{
}

void NXAppBar::setAppBarHeight(int height)
{
  Q_D(NXAppBar);
  d->m_AppBarHeight = height;
  setFixedHeight(d->m_AppBarHeight);
  window()->setContentsMargins(0, d->m_AppBarHeight + d->m_RibbonHeight, 0, 0);
  Q_EMIT signalAppBarHeightChanged();
}

int NXAppBar::getAppBarHeight() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXAppBar);
  return d->m_AppBarHeight;
}

void NXAppBar::setRibbonHeight(int height)
{
  Q_D(NXAppBar);
  d->m_RibbonHeight = height;
  window()->setContentsMargins(0, d->m_AppBarHeight + d->m_RibbonHeight, 0, 0);
  Q_EMIT signalAppBarHeightChanged();
}

int NXAppBar::getRibbonHeight() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXAppBar);
  return d->m_RibbonHeight;
}

void NXAppBar::setCustomWidget(NXAppBarType::CustomArea customArea, QWidget *widget, QObject *hitTestObject, QString const &hitTestFunctionName)
{
  Q_D(NXAppBar);
  if (!widget || widget == this) {
    return;
  }
  widget->setMinimumHeight(0);
  widget->setMaximumHeight(height());
  widget->setParent(this);
  int customAreaIndex = static_cast<int>(customArea);
  d->m_MainLayout->removeWidget(d->m_CustomAreaWidgetList[customAreaIndex]);
  d->m_MainLayout->insertWidget(2 * customAreaIndex + 1, widget);
  d->m_CustomAreaWidgetList[customAreaIndex]              = widget;
  d->m_CustomAreaHitTestObjectList[customAreaIndex]       = hitTestObject;
  d->m_CustomAreaHitTestFunctionNameList[customAreaIndex] = hitTestFunctionName;
  Q_EMIT customWidgetChanged();
}

QWidget *NXAppBar::getCustomWidget(NXAppBarType::CustomArea customArea) const
{
  Q_CONST_D(NXAppBar);
  int customAreaIndex = (int) customArea - 1;
  return d->m_CustomAreaWidgetList[customAreaIndex];
}

void NXAppBar::setCustomMenu(QMenu *customMenu)
{
  Q_D(NXAppBar);
  d->m_CustomMenu = customMenu;
  Q_EMIT customMenuChanged();
}

QMenu *NXAppBar::getCustomMenu() const
{
  Q_CONST_D(NXAppBar);
  return d->m_CustomMenu;
}

void NXAppBar::setIsFixedSize(bool isFixedSize)
{
  Q_D(NXAppBar);
  d->m_IsFixedSize = isFixedSize;
#ifdef Q_OS_WIN
  HWND hwnd   = (HWND) d->m_CurrentWinID;
  DWORD style = ::GetWindowLongPtr(hwnd, GWL_STYLE);
  if (d->m_IsFixedSize) {
    //切换DPI处理
    style &= ~WS_THICKFRAME;
    ::SetWindowLongPtr(hwnd, GWL_STYLE, style);
  } else {
    ::SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME);
  }
#else
  bool isVisible = window()->isVisible();
  window()->setWindowFlags((window()->windowFlags()) | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
  if (!isFixedSize) {
    window()->setWindowFlag(Qt::WindowMaximizeButtonHint);
  }
  if (isVisible) {
    window()->show();
  }
#endif
  Q_EMIT signalIsFixedSizeChanged();
}

bool NXAppBar::getIsFixedSize() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXAppBar);
  return d->m_IsFixedSize;
}

void NXAppBar::setWindowButtonFlag(NXAppBarType::ButtonType buttonFlag, bool isEnable)
{
  Q_D(NXAppBar);
  if (isEnable) {
    setWindowButtonFlags(d->m_ButtonFlags | buttonFlag);
  } else {
    setWindowButtonFlags(d->m_ButtonFlags & ~buttonFlag);
  }
}

void NXAppBar::setWindowButtonFlags(NXAppBarType::ButtonFlags buttonFlags)
{
  Q_D(NXAppBar);
  d->m_ButtonFlags = buttonFlags;
  if (d->m_ButtonFlags.testFlag(NXAppBarType::NoneButtonHint)) {
    d->m_RouteBackButton->setVisible(false);
    d->m_RouteForwardButton->setVisible(false);
    d->m_NavigationButton->setVisible(false);
    d->m_StayTopButton->setVisible(false);
    d->m_ThemeChangeButton->setVisible(false);
    d->m_MinButton->setVisible(false);
    d->m_MaxButton->setVisible(false);
    d->m_CloseButton->setVisible(false);
  } else {
    d->m_RouteBackButton->setVisible(d->m_ButtonFlags.testFlag(NXAppBarType::RouteBackButtonHint));
    d->m_RouteForwardButton->setVisible(d->m_ButtonFlags.testFlag(NXAppBarType::RouteForwardButtonHint));
    d->m_NavigationButton->setVisible(d->m_ButtonFlags.testFlag(NXAppBarType::NavigationButtonHint));
    d->m_StayTopButton->setVisible(d->m_ButtonFlags.testFlag(NXAppBarType::StayTopButtonHint));
    d->m_ThemeChangeButton->setVisible(d->m_ButtonFlags.testFlag(NXAppBarType::ThemeChangeButtonHint));
    d->m_MinButton->setVisible(d->m_ButtonFlags.testFlag(NXAppBarType::MinimizeButtonHint));
    d->m_MaxButton->setVisible(d->m_ButtonFlags.testFlag(NXAppBarType::MaximizeButtonHint));
    d->m_CloseButton->setVisible(d->m_ButtonFlags.testFlag(NXAppBarType::CloseButtonHint));
  }
}

NXAppBarType::ButtonFlags NXAppBar::getWindowButtonFlags() const
{
  return d_ptr->m_ButtonFlags;
}

void NXAppBar::setRouteBackButtonEnable(bool isEnable)
{
  Q_D(NXAppBar);
  d->m_RouteBackButton->setEnabled(isEnable);
}

void NXAppBar::setRouteForwardButtonEnable(bool isEnable)
{
  Q_D(NXAppBar);
  d->m_RouteForwardButton->setEnabled(isEnable);
}

#ifdef Q_OS_WIN
#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
int NXAppBar::takeOverNativeEvent(QByteArray const &eventType, void *message, qintptr *result)
#  else
int NXAppBar::takeOverNativeEvent(QByteArray const &eventType, void *message, long *result)
#  endif
{
  Q_D(NXAppBar);
  if ((eventType != QStringLiteral("windows_generic_MSG")) || !message) {
    return 0;
  }
  auto const msg  = static_cast<const MSG *>(message);
  const HWND hwnd = msg->hwnd;
  if (!hwnd || !msg) {
    return 0;
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
      return 1;
    }
    return 0;
  }
  case WM_NCPAINT : {
    if (!nxWinHelper->getIsCompositionEnabled()) {
      *result = FALSE;
      return 1;
    } else {
      return -1;
    }
  }
  case WM_NCACTIVATE : {
    if (nxWinHelper->getIsCompositionEnabled()) {
      *result = ::DefWindowProcW(hwnd, WM_NCACTIVATE, wParam, -1);
    } else {
      *result = TRUE;
    }
    return 1;
  }
  case WM_SIZE : {
    if (wParam == SIZE_RESTORED) {
      d->_changeMaxButtonAwesome(false);
    } else if (wParam == SIZE_MAXIMIZED) {
      d->_changeMaxButtonAwesome(true);
    }
    return 0;
  }
#  if QT_VERSION <= QT_VERSION_CHECK(6, 0, 0)
  case WM_SHOWWINDOW : {
    if (wParam == FALSE) {
      return 0;
    }
    RECT windowRect {};
    ::GetWindowRect(hwnd, &windowRect);
    int windowWidth      = windowRect.right - windowRect.left;
    int windowHeight     = windowRect.bottom - windowRect.top;
    static UINT swpFlags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE;
    ::SetWindowPos(hwnd, nullptr, 0, 0, windowWidth + 1, windowHeight, swpFlags);
    ::SetWindowPos(hwnd, nullptr, 0, 0, windowWidth, windowHeight, swpFlags);
    return -1;
  }
#  endif
  case WM_NCCALCSIZE : {
    if (wParam == FALSE) {
      return 0;
    }
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 3) && QT_VERSION <= QT_VERSION_CHECK(6, 6, 1))
    if (::IsZoomed(hwnd)) {
      this->move(7, 7);
      window()->setContentsMargins(8, 8 + height() + d->m_RibbonHeight, 8, 8);
    } else {
      this->move(0, 0);
      window()->setContentsMargins(0, height() + d->m_RibbonHeight, 0, 0);
    }
    *result = 0;
    return 1;
#  else
    RECT *clientRect = &((NCCALCSIZE_PARAMS *) (lParam))->rgrc[0];
    if (nxWinHelper->getIsWinVersionGreater10()) {
      const LONG originTop        = clientRect->top;
      const LRESULT hitTestResult = ::DefWindowProcW(hwnd, WM_NCCALCSIZE, wParam, lParam);
      if ((hitTestResult != HTERROR) && (hitTestResult != HTNOWHERE)) {
        *result = static_cast<long>(hitTestResult);
        return 1;
      }
      clientRect->top = originTop;
    }
    if (::IsZoomed(hwnd)) {
#    if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
      auto geometry = window()->screen()->geometry();
#    else
      QScreen *screen = qApp->screenAt(window()->geometry().center());
      QRect geometry;
      if (!screen) {
        screen = qApp->screenAt(QCursor::pos());
      }
      geometry = screen->geometry();
#    endif
      clientRect->top = geometry.top();
      if (!nxWinHelper->getIsWinVersionGreater10()) {
        quint32 borderThickness = nxWinHelper->getResizeBorderThickness(hwnd);
        clientRect->left        = geometry.left();
        clientRect->bottom -= borderThickness;
        clientRect->right -= borderThickness;
      }
    }
    *result = WVR_REDRAW;
    return 1;
#  endif
  }
  case WM_MOVE : {
    QScreen *currentScreen = qApp->screenAt(window()->geometry().center());
    if (currentScreen && currentScreen != d->m_LastScreen) {
      if (d->m_LastScreen) {
        ::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
        ::RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
      }
      d->m_LastScreen = currentScreen;
    }
    break;
  }
  case WM_NCHITTEST : {
    if (d->_containsCursorToItem(d->m_MaxButton)) {
      if (*result == HTNOWHERE) {
        if (!d->m_IsHoverMaxButton) {
          d->m_IsHoverMaxButton = true;
          d->m_MaxButton->setIsSelected(true);
          d->m_MaxButton->update();
        }
        *result = HTZOOM;
      }
      return 1;
    } else {
      if (d->m_IsHoverMaxButton) {
        d->m_IsHoverMaxButton = false;
        d->m_MaxButton->setIsSelected(false);
        d->m_MaxButton->update();
      }
    }
    POINT nativeLocalPos { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    ::ScreenToClient(hwnd, &nativeLocalPos);
    RECT clientRect { 0, 0, 0, 0 };
    ::GetClientRect(hwnd, &clientRect);
    auto clientWidth  = clientRect.right - clientRect.left;
    auto clientHeight = clientRect.bottom - clientRect.top;
    bool left         = nativeLocalPos.x < d->m_Win7Margins;
    bool right        = nativeLocalPos.x > clientWidth - d->m_Win7Margins;
    bool top          = nativeLocalPos.y < d->m_Margins;
    bool bottom       = nativeLocalPos.y > clientHeight - d->m_Win7Margins;
    *result           = HTNOWHERE;
    if (!d->m_IsOnlyAllowMinAndClose && !d->m_IsFixedSize && !window()->isFullScreen() && !window()->isMaximized()) {
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
      return 1;
    }
    if (d->_containsCursorToItem(this) && !window()->isFullScreen()) {
      *result = HTCAPTION;
      return 1;
    }
    *result = HTCLIENT;
    return 1;
  }
  case WM_GETMINMAXINFO : {
    MINMAXINFO *minmaxInfo = reinterpret_cast<MINMAXINFO *>(lParam);
    RECT rect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
    d->m_LastMinTrackWidth        = d->_calculateMinimumWidth();
    minmaxInfo->ptMinTrackSize.x = d->m_LastMinTrackWidth * qApp->devicePixelRatio();
    minmaxInfo->ptMinTrackSize.y = parentWidget()->minimumHeight() * qApp->devicePixelRatio();
    minmaxInfo->ptMaxPosition.x  = rect.left;
    minmaxInfo->ptMaxPosition.y  = rect.top;
    return 1;
  }
  case WM_LBUTTONDBLCLK : {
    QVariantMap postData;
    postData.insert(QStringLiteral("WMClickType"), NXAppBarType::WMLBUTTONDBLCLK);
    NXEventBus::getInstance()->post(QStringLiteral("WMWindowClicked"), postData);
    return 0;
  }
  case WM_LBUTTONDOWN : {
    QVariantMap postData;
    postData.insert(QStringLiteral("WMClickType"), NXAppBarType::WMLBUTTONDOWN);
    NXEventBus::getInstance()->post(QStringLiteral("WMWindowClicked"), postData);
    return 0;
  }
  case WM_LBUTTONUP : {
    QVariantMap postData;
    postData.insert(QStringLiteral("WMClickType"), NXAppBarType::WMLBUTTONUP);
    NXEventBus::getInstance()->post(QStringLiteral("WMWindowClicked"), postData);
    return 0;
  }
  case WM_NCLBUTTONDOWN : {
    QVariantMap postData;
    postData.insert(QStringLiteral("WMClickType"), NXAppBarType::WMNCLBUTTONDOWN);
    NXEventBus::getInstance()->post(QStringLiteral("WMWindowClicked"), postData);
    if (d->_containsCursorToItem(d->m_MaxButton) || d->m_IsOnlyAllowMinAndClose) {
      return 1;
    }
    break;
  }
  case WM_NCLBUTTONUP : {
    QVariantMap postData;
    postData.insert(QStringLiteral("WMClickType"), NXAppBarType::WMNCLBUTTONDOWN);
    NXEventBus::getInstance()->post(QStringLiteral("WMWindowClicked"), postData);
    if (d->_containsCursorToItem(d->m_MaxButton) && !d->m_IsOnlyAllowMinAndClose) {
      d->onMaxButtonClicked();
      return 1;
    }
    break;
  }
  case WM_NCLBUTTONDBLCLK : {
    if (!d->m_IsOnlyAllowMinAndClose && !d->m_IsFixedSize) {
      return 0;
    }
    return 1;
  }
  case WM_NCRBUTTONDOWN : {
    if (wParam == HTCAPTION && !d->m_IsOnlyAllowMinAndClose) {
      d->_showAppBarMenu(QCursor::pos());
    }
    break;
  }
  case WM_KEYDOWN :
  case WM_SYSKEYDOWN : {
    if ((GetAsyncKeyState(VK_MENU) & 0x80'00) && (GetAsyncKeyState(VK_SPACE) & 0x80'00) && !d->m_IsOnlyAllowMinAndClose) {
      auto pos = window()->geometry().topLeft();
      d->_showAppBarMenu(QPoint(pos.x(), pos.y() + this->height()));
    }
    break;
  }
  }
  return -1;
}
#endif

#ifdef Q_OS_MACOS
void NXAppBar::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXAppBar);
  if (event->button() == Qt::LeftButton) {
    if (d->_containsCursorToItem(this)) {
      d->m_IsDragging = true;
#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
      d->m_DragStartPos = event->globalPosition().toPoint() - window()->frameGeometry().topLeft();
#  else
      d->m_DragStartPos = event->globalPos() - window()->frameGeometry().topLeft();
#  endif
      grabMouse();
      event->accept();
      return;
    }
  }
  QWidget::mousePressEvent(event);
}

void NXAppBar::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXAppBar);
  if (d->m_IsDragging && !d->m_IsFixedSize && !window()->isMaximized() && !window()->isFullScreen()) {
#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    window()->move(event->globalPosition().toPoint() - d->m_DragStartPos);
#  else
    window()->move(event->globalPos() - d->m_DragStartPos);
#  endif
    event->accept();
    return;
  }
  QWidget::mouseMoveEvent(event);
}

void NXAppBar::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXAppBar);
  if (event->button() == Qt::LeftButton) {
    if (d->m_IsDragging) {
      releaseMouse();
    }
    d->m_IsDragging = false;
    event->accept();
    return;
  }
  QWidget::mouseReleaseEvent(event);
}

void NXAppBar::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_D(NXAppBar);
  if (event->button() == Qt::LeftButton && !d->m_IsFixedSize) {
    if (d->_containsCursorToItem(this)) {
      if (window()->isMaximized()) {
        window()->showNormal();
      } else {
        window()->showMaximized();
      }
      event->accept();
      return;
    }
  }
  QWidget::mouseDoubleClickEvent(event);
}
#endif

bool NXAppBar::eventFilter(QObject *obj, QEvent *event)
{
  Q_D(NXAppBar);
  switch (event->type()) {
  case QEvent::Resize : {
    QSize size = parentWidget()->size();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 3) && QT_VERSION <= QT_VERSION_CHECK(6, 6, 1))
    if (::IsZoomed((HWND) d->m_CurrentWinID)) {
      this->resize(size.width() - 14, this->height());
    } else {
      this->resize(size.width(), this->height());
    }
#else
    this->resize(size.width(), this->height());
#endif
    break;
  }
#ifdef Q_OS_WIN
  case QEvent::Show : {
    if (!d->m_IsFixedSize && !d->m_IsOnlyAllowMinAndClose) {
      HWND hwnd   = (HWND) d->m_CurrentWinID;
      DWORD style = ::GetWindowLongPtr(hwnd, GWL_STYLE);
      style &= ~WS_SYSMENU;
      ::SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME);
      if (!nxWinHelper->getIsWinVersionGreater10()) {
        SetClassLong(hwnd, GCL_STYLE, GetClassLong(hwnd, GCL_STYLE) | CS_DROPSHADOW);
      }
    }
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 3) && QT_VERSION <= QT_VERSION_CHECK(6, 6, 1))
    HWND hwnd = (HWND) d->m_CurrentWinID;
    NXWinShadowHelper::getInstance()->setWindowShadow(d->m_CurrentWinID);
    DWORD style     = ::GetWindowLongPtr(hwnd, GWL_STYLE);
    bool hasCaption = (style & WS_CAPTION) == WS_CAPTION;
    if (!hasCaption) {
      ::SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_CAPTION);
    }
#  endif
    break;
  }
#endif
  case QEvent::Close : {
    QCloseEvent *closeEvent = dynamic_cast<QCloseEvent *>(event);
    if (!d->m_IsDefaultClosed && closeEvent->spontaneous()) {
      event->ignore();
      if (window()->isMinimized()) {
        window()->showNormal();
      }
      d->onCloseButtonClicked();
      return true;
    } else {
      break;
    }
  }
#ifndef Q_OS_WIN
  case QEvent::MouseButtonPress : {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (mouseEvent->button() != Qt::LeftButton) {
      break;
    }

    if (d->m_Edges != 0) {
      d->_updateCursor(d->m_Edges);
      window()->windowHandle()->startSystemResize(Qt::Edges(d->m_Edges));
    } else {
      if (d->_containsCursorToItem(this) && !d->m_IsFixedSize) {
        window()->windowHandle()->startSystemMove();
      }
    }
    break;
  }
  case QEvent::MouseButtonDblClick : {
    if (d->_containsCursorToItem(this) && !d->m_IsFixedSize) {
      if (window()->isMaximized()) {
        window()->showNormal();
      } else {
        window()->showMaximized();
      }
    }
    break;
  }
  case QEvent::MouseButtonRelease : {
    d->m_Edges               = 0;
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (mouseEvent->button() == Qt::RightButton) {
      d->_showAppBarMenu(QCursor::pos());
    }
    break;
  }
  case QEvent::HoverMove : {
    if (window()->isMaximized() || window()->isFullScreen()) {
      break;
    }
    if (d->m_IsFixedSize) {
      break;
    }
    QHoverEvent *mouseEvent = static_cast<QHoverEvent *>(event);
    QPoint p =
#  if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        mouseEvent->pos();
#  else
        mouseEvent->position().toPoint();
#  endif
    if (p.x() >= d->m_Margins && p.x() <= (window()->width() - d->m_Margins) && p.y() >= d->m_Margins &&
        p.y() <= (window()->height() - d->m_Margins)) {
      if (d->m_Edges != 0) {
        d->m_Edges = 0;
        d->_updateCursor(d->m_Edges);
      }
      break;
    }
    d->m_Edges = 0;
    if (p.x() < d->m_Margins) {
      d->m_Edges |= Qt::LeftEdge;
    }
    if (p.x() > (window()->width() - d->m_Margins)) {
      d->m_Edges |= Qt::RightEdge;
    }
    if (p.y() < d->m_Margins) {
      d->m_Edges |= Qt::TopEdge;
    }
    if (p.y() > (window()->height() - d->m_Margins)) {
      d->m_Edges |= Qt::BottomEdge;
    }
    d->_updateCursor(d->m_Edges);
    break;
  }
#endif
  default : {
    break;
  }
  }
  return QObject::eventFilter(obj, event);
}

#ifdef Q_OS_WIN
void NXAppBar::paintEvent(QPaintEvent *event)
{
  if (nxWinHelper->getIsWinVersionGreater10() && !nxWinHelper->getIsWinVersionGreater11()) {
    Q_D(NXAppBar);
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing);
    auto borderWidth = nxWinHelper->getSystemMetricsForDpi((HWND) d->m_CurrentWinID, SM_CXBORDER);
    painter.setPen(QPen(window()->isActiveWindow() ? NXThemeColor(d->m_ThemeMode, Win10BorderActive) : NXThemeColor(d->m_ThemeMode, Win10BorderInactive), borderWidth));
    painter.drawLine(QPoint(0, 0), QPoint(window()->width(), 0));
    painter.restore();
  }
}
#endif
