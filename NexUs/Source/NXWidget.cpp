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
Q_TAKEOVER_NATIVEEVENT_CPP(NXWidget, d_func()->_appBar);

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
  d->_appBar = new NXAppBar(this);
  d->_appBar->setIsStayTop(false);
  d->_appBar->setWindowButtonFlags(NXAppBarType::StayTopButtonHint | NXAppBarType::MinimizeButtonHint | NXAppBarType::MaximizeButtonHint | NXAppBarType::CloseButtonHint);
  connect(d->_appBar, &NXAppBar::routeBackButtonClicked, this, &NXWidget::routeBackButtonClicked);
  connect(d->_appBar, &NXAppBar::navigationButtonClicked, this, &NXWidget::navigationButtonClicked);
  connect(d->_appBar, &NXAppBar::themeChangeButtonClicked, this, &NXWidget::themeChangeButtonClicked);
  connect(d->_appBar, &NXAppBar::closeButtonClicked, this, &NXWidget::closeButtonClicked);

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

NXWidget::~NXWidget()
{
  nxApp->syncWindowDisplayMode(this, false);
}

void NXWidget::setIsStayTop(bool isStayTop)
{
  Q_D(NXWidget);
  d->_appBar->setIsStayTop(isStayTop);
}

bool NXWidget::getIsStayTop() const
{
  return d_ptr->_appBar->getIsStayTop();
}

void NXWidget::setIsFixedSize(bool isFixedSize)
{
  Q_D(NXWidget);
  d->_appBar->setIsFixedSize(isFixedSize);
}

bool NXWidget::getIsFixedSize() const
{
  return d_ptr->_appBar->getIsFixedSize();
}

void NXWidget::setIsDefaultClosed(bool isDefaultClosed)
{
  Q_D(NXWidget);
  d->_appBar->setIsDefaultClosed(isDefaultClosed);
  Q_EMIT pIsDefaultClosedChanged();
}

bool NXWidget::getIsDefaultClosed() const
{
  Q_D(const NXWidget);
  return d->_appBar->getIsDefaultClosed();
}

void NXWidget::setAppBarHeight(int appBarHeight)
{
  Q_D(NXWidget);
  d->_appBar->setAppBarHeight(appBarHeight);
  Q_EMIT pAppBarHeightChanged();
}

int NXWidget::getAppBarHeight() const
{
  Q_D(const NXWidget);
  return d->_appBar->getAppBarHeight();
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
  d->_appBar->setWindowButtonFlag(buttonFlag, isEnable);
}

void NXWidget::setWindowButtonFlags(NXAppBarType::ButtonFlags buttonFlags)
{
  Q_D(NXWidget);
  d->_appBar->setWindowButtonFlags(buttonFlags);
}

NXAppBarType::ButtonFlags NXWidget::getWindowButtonFlags() const
{
  return d_ptr->_appBar->getWindowButtonFlags();
}

NXAppBar * NXWidget::getAppBar() const
{
  Q_D(const NXWidget);
  return d->_appBar;
}

void NXWidget::paintEvent(QPaintEvent *event)
{
  Q_D(NXWidget);
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
    painter.setBrush(NXThemeColor(d->_themeMode, WindowBase));
    painter.drawRect(rect());
    painter.restore();
  }
  QWidget::paintEvent(event);
}
