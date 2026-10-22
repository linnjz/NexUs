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
Q_TAKEOVER_NATIVEEVENT_CPP(NXDialog, d_func()->_appBar);

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
  d->_appBar = new NXAppBar(this);
  d->_appBar->setIsStayTop(true);
  d->_appBar->setWindowButtonFlags(NXAppBarType::StayTopButtonHint | NXAppBarType::MinimizeButtonHint | NXAppBarType::MaximizeButtonHint | NXAppBarType::CloseButtonHint);
  connect(d->_appBar, &NXAppBar::routeBackButtonClicked, this, &NXDialog::routeBackButtonClicked);
  connect(d->_appBar, &NXAppBar::navigationButtonClicked, this, &NXDialog::navigationButtonClicked);
  connect(d->_appBar, &NXAppBar::themeChangeButtonClicked, this, &NXDialog::themeChangeButtonClicked);
  connect(d->_appBar, &NXAppBar::closeButtonClicked, this, &NXDialog::closeButtonClicked);

  // 主题
  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });

  d->_windowDisplayMode = nxApp->getWindowDisplayMode();
  connect(nxApp, &NXApplication::pWindowDisplayModeChanged, this, [=]() {
    d->_windowDisplayMode = nxApp->getWindowDisplayMode();
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
  d->_appBar->setIsStayTop(isStayTop);
}

bool NXDialog::getIsStayTop() const
{
  return d_ptr->_appBar->getIsStayTop();
}

void NXDialog::setIsFixedSize(bool isFixedSize)
{
  Q_D(NXDialog);
  d->_appBar->setIsFixedSize(isFixedSize);
}

bool NXDialog::getIsFixedSize() const
{
  return d_ptr->_appBar->getIsFixedSize();
}

void NXDialog::setIsDefaultClosed(bool isDefaultClosed)
{
  Q_D(NXDialog);
  d->_appBar->setIsDefaultClosed(isDefaultClosed);
  Q_EMIT pIsDefaultClosedChanged();
}

bool NXDialog::getIsDefaultClosed() const
{
  Q_D(const NXDialog);
  return d->_appBar->getIsDefaultClosed();
}

void NXDialog::setAppBarHeight(int appBarHeight)
{
  Q_D(NXDialog);
  d->_appBar->setAppBarHeight(appBarHeight);
  Q_EMIT pAppBarHeightChanged();
}

int NXDialog::getAppBarHeight() const
{
  Q_D(const NXDialog);
  return d->_appBar->getAppBarHeight();
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
  d->_appBar->setWindowButtonFlag(buttonFlag, isEnable);
}

void NXDialog::setWindowButtonFlags(NXAppBarType::ButtonFlags buttonFlags)
{
  Q_D(NXDialog);
  d->_appBar->setWindowButtonFlags(buttonFlags);
}

NXAppBarType::ButtonFlags NXDialog::getWindowButtonFlags() const
{
  return d_ptr->_appBar->getWindowButtonFlags();
}

void NXDialog::paintEvent(QPaintEvent *event)
{
  Q_D(NXDialog);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 3) && QT_VERSION <= QT_VERSION_CHECK(6, 6, 1))
  if (d->_windowDisplayMode != NXApplicationType::WindowDisplayMode::NXMica)
#else
  if (d->_windowDisplayMode == NXApplicationType::WindowDisplayMode::Normal)
#endif
  {
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->_themeMode, DialogBase));
    painter.drawRect(rect());
    painter.restore();
  }
  QWidget::paintEvent(event);
}
