#include "NXDialog.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QPainter>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>

#include "NXApplication.h"
#include "NXTheme.h"
#include "private/NXDialogPrivate.h"
Q_TAKEOVER_NATIVEEVENT_CPP(NXDialog, d_func()->m_AppBar);

NXDialog::NXDialog(QWidget *parent)
    : QDialog { parent }
    , d_ptr(new NXDialogPrivate())
{
  Q_D(NXDialog);
  d->q_ptr = this;
  resize(500, 500); // 默认宽高
  setWindowTitle(QStringLiteral("NXDialog"));
  setObjectName("NXDialog");
#ifndef Q_OS_WIN
  setAttribute(Qt::WA_Hover);
#endif
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 3) || QT_VERSION > QT_VERSION_CHECK(6, 6, 1))
  setStyleSheet(QStringLiteral("#NXDialog{background-color:transparent;}"));
#endif
  // 自定义AppBar
  d->m_AppBar = new NXAppBar(this);
  d->m_AppBar->setIsStayTop(true);
  d->m_AppBar->setWindowButtonFlags(NXAppBarType::StayTopButtonHint | NXAppBarType::MinimizeButtonHint | NXAppBarType::MaximizeButtonHint | NXAppBarType::CloseButtonHint);
  connect(d->m_AppBar, &NXAppBar::routeBackButtonClicked, this, &NXDialog::routeBackButtonClicked);
  connect(d->m_AppBar, &NXAppBar::navigationButtonClicked, this, &NXDialog::navigationButtonClicked);
  connect(d->m_AppBar, &NXAppBar::themeChangeButtonClicked, this, &NXDialog::themeChangeButtonClicked);
  connect(d->m_AppBar, &NXAppBar::closeButtonClicked, this, &NXDialog::closeButtonClicked);

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

NXDialog::~NXDialog()
{
  nxApp->syncWindowDisplayMode(this, false);
}

void NXDialog::setIsStayTop(bool isStayTop)
{
  Q_D(NXDialog);
  d->m_AppBar->setIsStayTop(isStayTop);
}

bool NXDialog::getIsStayTop() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  return d_ptr->m_AppBar->getIsStayTop();
}

void NXDialog::setIsFixedSize(bool isFixedSize)
{
  Q_D(NXDialog);
  d->m_AppBar->setIsFixedSize(isFixedSize);
}

bool NXDialog::getIsFixedSize() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  return d_ptr->m_AppBar->getIsFixedSize();
}

void NXDialog::setIsDefaultClosed(bool isDefaultClosed)
{
  Q_D(NXDialog);
  d->m_AppBar->setIsDefaultClosed(isDefaultClosed);
  Q_EMIT signalIsDefaultClosedChanged();
}

bool NXDialog::getIsDefaultClosed() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXDialog);
  return d->m_AppBar->getIsDefaultClosed();
}

void NXDialog::setAppBarHeight(int appBarHeight)
{
  Q_D(NXDialog);
  d->m_AppBar->setAppBarHeight(appBarHeight);
  Q_EMIT signalAppBarHeightChanged();
}

int NXDialog::getAppBarHeight() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXDialog);
  return d->m_AppBar->getAppBarHeight();
}

void NXDialog::moveToCenter()
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

void NXDialog::setWindowButtonFlag(NXAppBarType::ButtonType buttonFlag, bool isEnable)
{
  Q_D(NXDialog);
  d->m_AppBar->setWindowButtonFlag(buttonFlag, isEnable);
}

void NXDialog::setWindowButtonFlags(NXAppBarType::ButtonFlags buttonFlags)
{
  Q_D(NXDialog);
  d->m_AppBar->setWindowButtonFlags(buttonFlags);
}

NXAppBarType::ButtonFlags NXDialog::getWindowButtonFlags() const
{
  return d_ptr->m_AppBar->getWindowButtonFlags();
}

void NXDialog::paintEvent(QPaintEvent *event)
{
  Q_D(NXDialog);
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
    painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogBase));
    painter.drawRect(rect());
    painter.restore();
  }
  QWidget::paintEvent(event);
}
