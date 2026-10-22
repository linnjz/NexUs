#include "NXSplashScreen.h"

#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>

#include "NXProgressBar.h"
#include "NXProgressRing.h"
#include "NXText.h"
#include "NXTheme.h"
#include "private/NXSplashScreenPrivate.h"
Q_PROPERTY_CREATE_CPP(NXSplashScreen, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSplashScreen, int, Minimum)
Q_PROPERTY_CREATE_CPP(NXSplashScreen, int, Maximum)
Q_PROPERTY_CREATE_CPP(NXSplashScreen, bool, IsShowProgressBar)
Q_PROPERTY_CREATE_CPP(NXSplashScreen, bool, IsShowProgressRing)
Q_PROPERTY_CREATE_CPP(NXSplashScreen, bool, IsClosable)

void NXSplashScreen::setValue(int value)
{
  Q_D(NXSplashScreen);
  d->_pValue = value;
  if (d->_progressBar) {
    d->_progressBar->setValue(value);
  }
  if (d->_progressRing) {
    d->_progressRing->setValue(value);
  }
}

int NXSplashScreen::getValue() const
{
  return d_ptr->_pValue;
}

NXSplashScreen::NXSplashScreen(QWidget *parent)
    : QWidget { nullptr }
    , d_ptr(new NXSplashScreenPrivate())
{
  Q_D(NXSplashScreen);
  d->q_ptr                = this;
  d->_pBorderRadius       = 12;
  d->_pMinimum            = 0;
  d->_pMaximum            = 100;
  d->_pValue              = 0;
  d->_pIsShowProgressBar  = true;
  d->_pIsShowProgressRing = false;
  d->_pIsClosable         = false;
  d->_themeMode           = nxTheme->getThemeMode();

  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
  setAttribute(Qt::WA_TranslucentBackground);
  setFixedSize(580, 400);

  d->_titleText = new NXText(this);
  d->_titleText->setTextStyle(NXTextType::TitleLarge);
  d->_titleText->setAlignment(Qt::AlignCenter);

  d->_subTitleText = new NXText(this);
  d->_subTitleText->setTextStyle(NXTextType::Subtitle);
  d->_subTitleText->setAlignment(Qt::AlignCenter);
  d->_subTitleText->setWordWrap(true);
  d->_subTitleText->setMinimumWidth(400);

  d->_statusText = new NXText(this);
  d->_statusText->setTextStyle(NXTextType::Body);
  d->_statusText->setAlignment(Qt::AlignCenter);

  d->_progressBar = new NXProgressBar(this);
  d->_progressBar->setMinimum(0);
  d->_progressBar->setMaximum(100);
  d->_progressBar->setFixedHeight(24);

  d->_progressRing = new NXProgressRing(this);
  d->_progressRing->setFixedSize(48, 48);
  d->_progressRing->setIsBusying(true);
  d->_progressRing->setVisible(false);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  mainLayout->setContentsMargins(30, 44, 30, 30);
#else
  mainLayout->setContentsMargins(36, 50, 36, 36);
#endif
  mainLayout->setSpacing(6);
  mainLayout->addStretch(2);
  mainLayout->addWidget(d->_titleText, 0, Qt::AlignCenter);
  mainLayout->addSpacing(4);
  mainLayout->addWidget(d->_subTitleText, 0, Qt::AlignCenter);
  mainLayout->addStretch(1);
  mainLayout->addWidget(d->_progressRing, 0, Qt::AlignCenter);
  mainLayout->addSpacing(8);
  mainLayout->addWidget(d->_statusText, 0, Qt::AlignCenter);
  mainLayout->addSpacing(12);
  mainLayout->addWidget(d->_progressBar);
  mainLayout->addStretch(1);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXSplashScreen::NXSplashScreen(const QPixmap &logo, QWidget *parent)
    : NXSplashScreen(parent)
{
  Q_D(NXSplashScreen);
  d->_logo = logo;
}

NXSplashScreen::~NXSplashScreen()
{
}

void NXSplashScreen::setLogo(const QPixmap &logo)
{
  Q_D(NXSplashScreen);
  d->_logo = logo;
  update();
}

void NXSplashScreen::setTitle(const QString &title)
{
  Q_D(NXSplashScreen);
  d->_titleText->setText(title);
}

void NXSplashScreen::setSubTitle(const QString &subTitle)
{
  Q_D(NXSplashScreen);
  d->_subTitleText->setText(subTitle);
}

void NXSplashScreen::setStatusText(const QString &text)
{
  Q_D(NXSplashScreen);
  d->_statusText->setText(text);
}

void NXSplashScreen::show()
{
  Q_D(NXSplashScreen);
  QScreen *screen = QGuiApplication::primaryScreen();
  if (screen) {
    QRect screenRect = screen->availableGeometry();
    move((screenRect.width() - width()) / 2, (screenRect.height() - height()) / 2);
  }
  d->_progressBar->setVisible(d->_pIsShowProgressBar);
  d->_progressRing->setVisible(d->_pIsShowProgressRing);
  QWidget::show();
  d->_doShowAnimation();
}

void NXSplashScreen::close()
{
  Q_D(NXSplashScreen);
  d->_doCloseAnimation();
}

void NXSplashScreen::finish(QWidget *mainWindow)
{
  Q_D(NXSplashScreen);
  if (mainWindow) {
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(this, "windowOpacity");
    opacityAnimation->setDuration(300);
    opacityAnimation->setStartValue(1.0);
    opacityAnimation->setEndValue(0.0);
    opacityAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    connect(opacityAnimation, &QPropertyAnimation::finished, this, [=]() {
      QWidget::close();
      Q_EMIT closed();
      mainWindow->show();
      mainWindow->raise();
      mainWindow->activateWindow();
    });
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }
}

void NXSplashScreen::paintEvent(QPaintEvent *event)
{
  Q_D(NXSplashScreen);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  QRect foregroundRect = rect();
#else
  nxTheme->drawEffectShadow(&painter, rect(), 6, d->_pBorderRadius);
  QRect foregroundRect = rect().adjusted(6, 6, -6, -6);
#endif
  QPainterPath path;
  path.addRoundedRect(foregroundRect, d->_pBorderRadius, d->_pBorderRadius);
  painter.setClipPath(path);
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->_themeMode, DialogBase));
  painter.drawRoundedRect(foregroundRect, d->_pBorderRadius, d->_pBorderRadius);

  if (!d->_logo.isNull()) {
    int logoSize = qMin(80, qMin(foregroundRect.width(), foregroundRect.height()) / 3);
    QRect logoRect(foregroundRect.x() + (foregroundRect.width() - logoSize) / 2, foregroundRect.y() + 30, logoSize, logoSize);
    painter.drawPixmap(logoRect, d->_logo.scaled(logoSize, logoSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }

  painter.setBrush(Qt::NoBrush);
  painter.setPen(QPen(NXThemeColor(d->_themeMode, PopupBorder), 1));
  painter.drawRoundedRect(foregroundRect, d->_pBorderRadius, d->_pBorderRadius);

  painter.restore();
}

void NXSplashScreen::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXSplashScreen);
  if (event->button() == Qt::LeftButton) {
    d->_isDragging = true;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    d->_dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
#else
    d->_dragStartPos = event->globalPos() - frameGeometry().topLeft();
#endif
    event->accept();
  }
}

void NXSplashScreen::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXSplashScreen);
  if (d->_isDragging) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    move(event->globalPosition().toPoint() - d->_dragStartPos);
#else
    move(event->globalPos() - d->_dragStartPos);
#endif
    event->accept();
  }
}

void NXSplashScreen::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXSplashScreen);
  d->_isDragging = false;
  QWidget::mouseReleaseEvent(event);
}
