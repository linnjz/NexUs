#include "NXWidget.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QPainter>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>

#include "NXApplication.h"
#include "NXTheme.h"
#include "private/NXWidgetPrivate.h"
Q_TAKEOVER_NATIVEEVENT_CPP(NXWidget, d_func()->m_AppBar);

NXWidget::NXWidget(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXWidgetPrivate())
{
  Q_D(NXWidget);
  d->q_ptr = this;
  resize(500, 500); // 默认宽高
  setWindowTitle(QStringLiteral("NXWidget"));
  setObjectName("NXWidget");
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 3) || QT_VERSION > QT_VERSION_CHECK(6, 6, 1))
  setStyleSheet(QStringLiteral("#NXWidget{background-color:transparent;}"));
#endif
  // 自定义AppBar
  d->m_AppBar = new NXAppBar(this);
  d->m_AppBar->setIsStayTop(false);
  d->m_AppBar->setWindowButtonFlags(NXAppBarType::StayTopButtonHint | NXAppBarType::MinimizeButtonHint | NXAppBarType::MaximizeButtonHint | NXAppBarType::CloseButtonHint);
  connect(d->m_AppBar, &NXAppBar::routeBackButtonClicked, this, &NXWidget::routeBackButtonClicked);
  connect(d->m_AppBar, &NXAppBar::navigationButtonClicked, this, &NXWidget::navigationButtonClicked);
  connect(d->m_AppBar, &NXAppBar::themeChangeButtonClicked, this, &NXWidget::themeChangeButtonClicked);
  connect(d->m_AppBar, &NXAppBar::closeButtonClicked, this, &NXWidget::closeButtonClicked);

  // 主题
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });

  d->m_WindowDisplayMode = nxApp->getWindowDisplayMode();
  connect(nxApp, &NXApplication::signalWindowDisplayModeChanged, this, [=]() {
    d->m_WindowDisplayMode = nxApp->getWindowDisplayMode();
    update();
  });
  nxApp->syncWindowDisplayMode(this);
}

NXWidget::~NXWidget()
{
  nxApp->syncWindowDisplayMode(this, false);
}

void NXWidget::setIsStayTop(bool isStayTop)
{
  Q_D(NXWidget);
  d->m_AppBar->setIsStayTop(isStayTop);
}

bool NXWidget::getIsStayTop() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  return d_ptr->m_AppBar->getIsStayTop();
}

void NXWidget::setIsFixedSize(bool isFixedSize)
{
  Q_D(NXWidget);
  d->m_AppBar->setIsFixedSize(isFixedSize);
}

bool NXWidget::getIsFixedSize() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  return d_ptr->m_AppBar->getIsFixedSize();
}

void NXWidget::setIsDefaultClosed(bool isDefaultClosed)
{
  Q_D(NXWidget);
  d->m_AppBar->setIsDefaultClosed(isDefaultClosed);
  Q_EMIT signalIsDefaultClosedChanged();
}

bool NXWidget::getIsDefaultClosed() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXWidget);
  return d->m_AppBar->getIsDefaultClosed();
}

void NXWidget::setAppBarHeight(int appBarHeight)
{
  Q_D(NXWidget);
  d->m_AppBar->setAppBarHeight(appBarHeight);
  Q_EMIT signalAppBarHeightChanged();
}

int NXWidget::getAppBarHeight() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXWidget);
  return d->m_AppBar->getAppBarHeight();
}

void NXWidget::moveToCenter()
{
  if (isMaximized() || isFullScreen()) {
    return;
  }
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
  auto geometry = screen()->availableGeometry();
#else
  auto geometry = qApp->screenAt(this->geometry().center())->geometry();
#endif
  setGeometry((geometry.left() + geometry.right() - width()) / 2, (geometry.top() + geometry.bottom() - height()) / 2, width(), height());
}

void NXWidget::setWindowButtonFlag(NXAppBarType::ButtonType buttonFlag, bool isEnable)
{
  Q_D(NXWidget);
  d->m_AppBar->setWindowButtonFlag(buttonFlag, isEnable);
}

void NXWidget::setWindowButtonFlags(NXAppBarType::ButtonFlags buttonFlags)
{
  Q_D(NXWidget);
  d->m_AppBar->setWindowButtonFlags(buttonFlags);
}

NXAppBarType::ButtonFlags NXWidget::getWindowButtonFlags() const
{
  return d_ptr->m_AppBar->getWindowButtonFlags();
}

NXAppBar *NXWidget::getAppBar() const
{
  Q_CONST_D(NXWidget);
  return d->m_AppBar;
}

void NXWidget::paintEvent(QPaintEvent *event)
{
  Q_D(NXWidget);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 3) && QT_VERSION <= QT_VERSION_CHECK(6, 6, 1))
  if (d->m_WindowDisplayMode != NXApplicationType::WindowDisplayMode::NXMica)
#else
  if (d->m_WindowDisplayMode == NXApplicationType::WindowDisplayMode::Normal)
#endif
  {
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->m_ThemeMode, WindowBase));
    painter.drawRect(rect());
    painter.restore();
  }
  QWidget::paintEvent(event);
}
