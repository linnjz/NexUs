#include "NXToast.h"

#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QScreen>
#include <QSequentialAnimationGroup>
#include <QTimer>

#include "NXTheme.h"
#include "private/NXToastPrivate.h"

Q_PROPERTY_CREATE_CPP(NXToast, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXToast, QPS_PAEMIT(int), DisplayMsec)

NXToast::NXToast(ToastType type, QString const &text, int displayMsec, QWidget *parent)
    : QWidget { nullptr }
    , d_ptr(new NXToastPrivate())
{
  Q_D(NXToast);
  d->q_ptr          = this;
  d->m_BorderRadius = 8;
  d->m_DisplayMsec  = displayMsec;
  d->m_ToastType     = type;
  d->m_Text          = text;
  setObjectName("NXToast");
  setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_DeleteOnClose);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });

  // Calculate size
  QFont textFont = font();
  textFont.setPixelSize(14);
  QFontMetrics fm(textFont);
  int textWidth  = fm.horizontalAdvance(text);
  int totalWidth = textWidth + 80; // icon + padding
  totalWidth     = qBound(200, totalWidth, 400);
  setFixedHeight(48);
  setFixedWidth(totalWidth + d->m_ShadowBorderWidth * 2);

  // Position: centered at top of parent window or screen
  QPoint targetPos;
  if (parent) {
    QRect parentRect = parent->geometry();
    int x            = parentRect.x() + (parentRect.width() - width()) / 2;
    int y            = parentRect.y() + 60;
    targetPos        = QPoint(x, y);
  } else {
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
      QRect screenRect = screen->availableGeometry();
      int x            = screenRect.x() + (screenRect.width() - width()) / 2;
      int y            = screenRect.y() + 60;
      targetPos        = QPoint(x, y);
    }
  }
  move(targetPos);

  // Opacity effect and animation
  QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
  opacityEffect->setOpacity(0);
  setGraphicsEffect(opacityEffect);

  // Fade in
  QPropertyAnimation *fadeIn = new QPropertyAnimation(opacityEffect, "opacity", this);
  fadeIn->setDuration(200);
  fadeIn->setStartValue(0.0);
  fadeIn->setEndValue(1.0);
  fadeIn->setEasingCurve(QEasingCurve::OutCubic);

  // Fade out
  QPropertyAnimation *fadeOut = new QPropertyAnimation(opacityEffect, "opacity", this);
  fadeOut->setDuration(300);
  fadeOut->setStartValue(1.0);
  fadeOut->setEndValue(0.0);
  fadeOut->setEasingCurve(QEasingCurve::InCubic);

  connect(fadeIn, &QPropertyAnimation::finished, this, [=]() {
    QTimer::singleShot(displayMsec, this, [fadeOut]() {
      fadeOut->start();
    });
  });

  connect(fadeOut, &QPropertyAnimation::finished, this, [=]() {
    close();
    deleteLater();
  });

  show();
  fadeIn->start();
}

NXToast::~NXToast()
{
}

void NXToast::success(QString const &text, int displayMsec, QWidget *parent)
{
  NXToast *toast = new NXToast(Success, text, displayMsec, parent);
  Q_UNUSED(toast);
}

void NXToast::info(QString const &text, int displayMsec, QWidget *parent)
{
  NXToast *toast = new NXToast(Info, text, displayMsec, parent);
  Q_UNUSED(toast);
}

void NXToast::warning(QString const &text, int displayMsec, QWidget *parent)
{
  NXToast *toast = new NXToast(Warning, text, displayMsec, parent);
  Q_UNUSED(toast);
}

void NXToast::error(QString const &text, int displayMsec, QWidget *parent)
{
  NXToast *toast = new NXToast(Error, text, displayMsec, parent);
  Q_UNUSED(toast);
}

void NXToast::paintEvent(QPaintEvent *event)
{
  Q_D(NXToast);
  QPainter painter(this);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);

  // Shadow
  nxTheme->drawEffectShadow(&painter, rect(), d->m_ShadowBorderWidth, d->m_BorderRadius);

  // Foreground rect
  QRect foregroundRect(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, width() - 2 * d->m_ShadowBorderWidth, height() - 2 * d->m_ShadowBorderWidth);

  // Background
  painter.save();
  painter.setPen(NXThemeColor(d->m_ThemeMode, PopupBorder));
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PopupBase));
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  painter.restore();

  // Left indicator bar (4px wide)
  QColor indicatorColor;
  NXIconType::IconName iconEnum;
  QColor iconColor;
  switch (d->m_ToastType) {
  case Success :
    indicatorColor = QColor(0x0F, 0x7B, 0x0F);
    iconEnum       = NXIconType::Check;
    iconColor      = QColor(0x0F, 0x7B, 0x0F);
    break;
  case Info :
    indicatorColor = NXThemeColor(d->m_ThemeMode, PrimaryNormal);
    iconEnum       = NXIconType::CircleInfo;
    iconColor      = NXThemeColor(d->m_ThemeMode, PrimaryNormal);
    break;
  case Warning :
    indicatorColor = QColor(0xF7, 0x93, 0x0E);
    iconEnum       = NXIconType::CircleExclamation;
    iconColor      = QColor(0xF7, 0x93, 0x0E);
    break;
  case Error :
    indicatorColor = NXThemeColor(d->m_ThemeMode, StatusDanger);
    iconEnum       = NXIconType::CircleXmark;
    iconColor      = NXThemeColor(d->m_ThemeMode, StatusDanger);
    break;
  }

  // Draw indicator bar
  painter.save();
  painter.setPen(Qt::NoPen);
  painter.setBrush(indicatorColor);
  QPainterPath clipPath;
  clipPath.addRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  painter.setClipPath(clipPath);
  painter.drawRect(QRect(foregroundRect.x(), foregroundRect.y(), 4, foregroundRect.height()));
  painter.restore();

  // Draw icon
  painter.save();
  painter.setPen(iconColor);
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(16);
  painter.setFont(iconFont);
  QRect iconRect(foregroundRect.x() + 14, foregroundRect.y(), 20, foregroundRect.height());
  painter.drawText(iconRect, Qt::AlignCenter, QChar((unsigned short) iconEnum));
  painter.restore();

  // Draw text
  painter.save();
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  QFont textFont = font();
  textFont.setPixelSize(14);
  painter.setFont(textFont);
  QRect textRect(foregroundRect.x() + 42, foregroundRect.y(), foregroundRect.width() - 52, foregroundRect.height());
  painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, d->m_Text);
  painter.restore();
}

NXToastPrivate::NXToastPrivate(QObject *parent)
    : QObject { parent }
{
}

NXToastPrivate::~NXToastPrivate()
{
}
