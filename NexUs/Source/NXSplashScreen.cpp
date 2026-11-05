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
Q_PROPERTY_CREATE_CPP(NXSplashScreen, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSplashScreen, QPS_PAEMIT(int), Minimum)
Q_PROPERTY_CREATE_CPP(NXSplashScreen, QPS_PAEMIT(int), Maximum)
Q_PROPERTY_CREATE_CPP(NXSplashScreen, QPS_PAEMIT(bool), IsShowProgressBar)
Q_PROPERTY_CREATE_CPP(NXSplashScreen, QPS_PAEMIT(bool), IsShowProgressRing)
Q_PROPERTY_CREATE_CPP(NXSplashScreen, QPS_PAEMIT(bool), IsClosable)

void NXSplashScreen::setValue(int value)
{
  Q_D(NXSplashScreen);
  d->m_Value = value;
  if (d->m_ProgressBar) {
    d->m_ProgressBar->setValue(value);
  }
  if (d->m_ProgressRing) {
    d->m_ProgressRing->setValue(value);
  }
}

int NXSplashScreen::getValue() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  return d_ptr->m_Value;
}

NXSplashScreen::NXSplashScreen(QWidget *parent)
    : QWidget { nullptr }
    , d_ptr(new NXSplashScreenPrivate())
{
  Q_D(NXSplashScreen);
  d->q_ptr                = this;
  d->m_BorderRadius       = 12;
  d->m_Minimum            = 0;
  d->m_Maximum            = 100;
  d->m_Value              = 0;
  d->m_IsShowProgressBar  = true;
  d->m_IsShowProgressRing = false;
  d->m_IsClosable         = false;
  d->m_ThemeMode           = nxTheme->getThemeMode();

  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
  setAttribute(Qt::WA_TranslucentBackground);
  setFixedSize(580, 400);

  d->m_TitleText = new NXText(this);
  d->m_TitleText->setTextStyle(NXTextType::TitleLarge);
  d->m_TitleText->setAlignment(Qt::AlignCenter);

  d->m_SubTitleText = new NXText(this);
  d->m_SubTitleText->setTextStyle(NXTextType::Subtitle);
  d->m_SubTitleText->setAlignment(Qt::AlignCenter);
  d->m_SubTitleText->setWordWrap(true);
  d->m_SubTitleText->setMinimumWidth(400);

  d->m_StatusText = new NXText(this);
  d->m_StatusText->setTextStyle(NXTextType::Body);
  d->m_StatusText->setAlignment(Qt::AlignCenter);

  d->m_ProgressBar = new NXProgressBar(this);
  d->m_ProgressBar->setMinimum(0);
  d->m_ProgressBar->setMaximum(100);
  d->m_ProgressBar->setFixedHeight(24);

  d->m_ProgressRing = new NXProgressRing(this);
  d->m_ProgressRing->setFixedSize(48, 48);
  d->m_ProgressRing->setIsBusying(true);
  d->m_ProgressRing->setVisible(false);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  mainLayout->setContentsMargins(30, 44, 30, 30);
#else
  mainLayout->setContentsMargins(36, 50, 36, 36);
#endif
  mainLayout->setSpacing(6);
  mainLayout->addStretch(2);
  mainLayout->addWidget(d->m_TitleText, 0, Qt::AlignCenter);
  mainLayout->addSpacing(4);
  mainLayout->addWidget(d->m_SubTitleText, 0, Qt::AlignCenter);
  mainLayout->addStretch(1);
  mainLayout->addWidget(d->m_ProgressRing, 0, Qt::AlignCenter);
  mainLayout->addSpacing(8);
  mainLayout->addWidget(d->m_StatusText, 0, Qt::AlignCenter);
  mainLayout->addSpacing(12);
  mainLayout->addWidget(d->m_ProgressBar);
  mainLayout->addStretch(1);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXSplashScreen::NXSplashScreen(QPixmap const &logo, QWidget *parent)
    : NXSplashScreen(parent)
{
  Q_D(NXSplashScreen);
  d->m_Logo = logo;
}

NXSplashScreen::~NXSplashScreen()
{
}

void NXSplashScreen::setLogo(QPixmap const &logo)
{
  Q_D(NXSplashScreen);
  d->m_Logo = logo;
  update();
}

void NXSplashScreen::setTitle(QString const &title)
{
  Q_D(NXSplashScreen);
  d->m_TitleText->setText(title);
}

void NXSplashScreen::setSubTitle(QString const &subTitle)
{
  Q_D(NXSplashScreen);
  d->m_SubTitleText->setText(subTitle);
}

void NXSplashScreen::setStatusText(QString const &text)
{
  Q_D(NXSplashScreen);
  d->m_StatusText->setText(text);
}

void NXSplashScreen::show()
{
  Q_D(NXSplashScreen);
  QScreen *screen = QGuiApplication::primaryScreen();
  if (screen) {
    QRect screenRect = screen->availableGeometry();
    move((screenRect.width() - width()) / 2, (screenRect.height() - height()) / 2);
  }
  d->m_ProgressBar->setVisible(d->m_IsShowProgressBar);
  d->m_ProgressRing->setVisible(d->m_IsShowProgressRing);
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
  nxTheme->drawEffectShadow(&painter, rect(), 6, d->m_BorderRadius);
  QRect foregroundRect = rect().adjusted(6, 6, -6, -6);
#endif
  QPainterPath path;
  path.addRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  painter.setClipPath(path);
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogBase));
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);

  if (!d->m_Logo.isNull()) {
    int logoSize = qMin(80, qMin(foregroundRect.width(), foregroundRect.height()) / 3);
    QRect logoRect(foregroundRect.x() + (foregroundRect.width() - logoSize) / 2, foregroundRect.y() + 30, logoSize, logoSize);
    painter.drawPixmap(logoRect, d->m_Logo.scaled(logoSize, logoSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }

  painter.setBrush(Qt::NoBrush);
  painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, PopupBorder), 1));
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);

  painter.restore();
}

void NXSplashScreen::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXSplashScreen);
  if (event->button() == Qt::LeftButton) {
    d->m_IsDragging = true;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    d->m_DragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
#else
    d->m_DragStartPos = event->globalPos() - frameGeometry().topLeft();
#endif
    event->accept();
  }
}

void NXSplashScreen::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXSplashScreen);
  if (d->m_IsDragging) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    move(event->globalPosition().toPoint() - d->m_DragStartPos);
#else
    move(event->globalPos() - d->m_DragStartPos);
#endif
    event->accept();
  }
}

void NXSplashScreen::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXSplashScreen);
  d->m_IsDragging = false;
  QWidget::mouseReleaseEvent(event);
}
