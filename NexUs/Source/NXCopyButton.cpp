#include "NXCopyButton.h"

#include <QApplication>
#include <QClipboard>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QTimer>

#include "NXTheme.h"
#include "private/NXCopyButtonPrivate.h"

Q_PROPERTY_CREATE_CPP(NXCopyButton, QPS_SCREF_PAEMIT(QString), CopyText)
Q_PROPERTY_CREATE_CPP(NXCopyButton, QPS_SCREF_PAEMIT(QString), SuccessText)
Q_PROPERTY_CREATE_CPP(NXCopyButton, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXCopyButton, QPS_PAEMIT(int), SuccessDuration)

NXCopyButton::NXCopyButton(QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXCopyButtonPrivate())
{
  Q_D(NXCopyButton);
  d->q_ptr             = this;
  d->m_BorderRadius    = 3;
  d->m_SuccessDuration = 1500;
  d->m_ThemeMode        = nxTheme->getThemeMode();
  setMouseTracking(true);
  setFixedSize(30, 30);
  QFont font = this->font();
  font.setPixelSize(15);
  setFont(font);
  setObjectName("NXCopyButton");
  setStyleSheet(QStringLiteral("#NXCopyButton{background-color:transparent;}"));

  d->m_ResetTimer = new QTimer(this);
  d->m_ResetTimer->setSingleShot(true);
  connect(d->m_ResetTimer, &QTimer::timeout, this, [=]() {
    d->m_IsSuccess                     = false;
    QPropertyAnimation *fadeAnimation = new QPropertyAnimation(this, "");
    connect(fadeAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      d->m_IconOpacity = value.toReal();
      update();
    });
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->setDuration(200);
    fadeAnimation->setEasingCurve(QEasingCurve::InOutSine);
    fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  });

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXCopyButton::NXCopyButton(QString const &text, QWidget *parent)
    : NXCopyButton(parent)
{
  d_ptr->m_CopyText = text;
}

NXCopyButton::~NXCopyButton()
{
}

void NXCopyButton::setText(QString const &text)
{
  QPushButton::setText(text);
  if (text.isEmpty()) {
    setFixedSize(30, 30);
  } else {
    Q_D(NXCopyButton);
    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(17);
    QFontMetrics iconFm(iconFont);
    int iconWidth = iconFm.horizontalAdvance(QChar((unsigned short) d->m_NormalIcon));
    QFontMetrics textFm(font());
    int textWidth = qMax(textFm.horizontalAdvance(text), textFm.horizontalAdvance(d->m_SuccessText));
    int padding   = 2 * d->m_ShadowBorderWidth + 6 + 16;
    setFixedSize(iconWidth + textWidth + padding, 38);
  }
}

void NXCopyButton::setNXIcon(NXIconType::IconName icon)
{
  Q_D(NXCopyButton);
  d->m_NormalIcon = icon;
  update();
}

void NXCopyButton::setSuccessIcon(NXIconType::IconName icon)
{
  Q_D(NXCopyButton);
  d->m_SuccessIcon = icon;
  update();
}

void NXCopyButton::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXCopyButton);
  d->m_IsPressed = true;
  QPushButton::mousePressEvent(event);
}

void NXCopyButton::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXCopyButton);
  d->m_IsPressed = false;
  if (rect().contains(event->pos()) && !d->m_IsSuccess) {
    QApplication::clipboard()->setText(d->m_CopyText);
    d->m_IsSuccess = true;
    d->m_ResetTimer->start(d->m_SuccessDuration);

    QPropertyAnimation *fadeAnimation = new QPropertyAnimation(this, "");
    connect(fadeAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      d->m_IconOpacity = value.toReal();
      update();
    });
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->setDuration(200);
    fadeAnimation->setEasingCurve(QEasingCurve::InOutSine);
    fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    Q_EMIT copyCompleted(d->m_CopyText);
  }
  QPushButton::mouseReleaseEvent(event);
}

void NXCopyButton::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  Q_D(NXCopyButton);
  QPainter painter(this);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  nxTheme->drawEffectShadow(&painter, rect(), d->m_ShadowBorderWidth, d->m_BorderRadius);

  painter.save();
  QRect foregroundRect(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, width() - 2 * d->m_ShadowBorderWidth, height() - 2 * d->m_ShadowBorderWidth);

  if (d->m_ThemeMode == NXThemeType::Light) {
    painter.setPen(NXThemeColor(NXThemeType::Light, BasicBorder));
    if (d->m_IsSuccess) {
      painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBase));
    } else if (d->m_IsPressed) {
      painter.setBrush(NXThemeColor(NXThemeType::Light, BasicPress));
    } else if (underMouse()) {
      painter.setBrush(NXThemeColor(NXThemeType::Light, BasicHover));
    } else {
      painter.setBrush(NXThemeColor(NXThemeType::Light, BasicBase));
    }
  } else {
    painter.setPen(Qt::NoPen);
    if (d->m_IsSuccess) {
      painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBase));
    } else if (d->m_IsPressed) {
      painter.setBrush(NXThemeColor(NXThemeType::Dark, BasicPress));
    } else if (underMouse()) {
      painter.setBrush(NXThemeColor(NXThemeType::Dark, BasicHover));
    } else {
      painter.setBrush(NXThemeColor(NXThemeType::Dark, BasicBase));
    }
  }
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);

  if (!d->m_IsPressed) {
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBaseLine));
    painter.drawLine(foregroundRect.x() + d->m_BorderRadius, height() - d->m_ShadowBorderWidth, foregroundRect.width(), height() - d->m_ShadowBorderWidth);
  }

  NXIconType::IconName currentIcon = d->m_IsSuccess ? d->m_SuccessIcon : d->m_NormalIcon;
  QColor iconColor                 = d->m_IsSuccess ? NXThemeColor(d->m_ThemeMode, PrimaryNormal)
                                                   : (d->m_ThemeMode == NXThemeType::Light ? NXThemeColor(NXThemeType::Light, BasicText)
                                                                                          : NXThemeColor(NXThemeType::Dark, BasicText));
  iconColor.setAlphaF(d->m_IconOpacity);
  painter.setPen(iconColor);

  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(17);

  QString btnText = d->m_IsSuccess ? d->m_SuccessText : text();
  if (btnText.isEmpty()) {
    painter.setFont(iconFont);
    painter.drawText(foregroundRect, Qt::AlignCenter, QChar((unsigned short) currentIcon));
  } else {
    QFontMetrics iconFm(iconFont);
    int iconWidth = iconFm.horizontalAdvance(QChar((unsigned short) currentIcon));

    QFont textFont   = this->font();
    QColor textColor = d->m_IsSuccess
                         ? NXThemeColor(d->m_ThemeMode, PrimaryNormal)
                         : (d->m_ThemeMode == NXThemeType::Light ? NXThemeColor(NXThemeType::Light, BasicText)
                                                                : NXThemeColor(NXThemeType::Dark, BasicText));
    textColor.setAlphaF(d->m_IconOpacity);
    QFontMetrics textFm(textFont);
    int textWidth = textFm.horizontalAdvance(btnText);

    int spacing    = 6;
    int totalWidth = iconWidth + spacing + textWidth;
    int startX     = foregroundRect.x() + (foregroundRect.width() - totalWidth) / 2;

    painter.setFont(iconFont);
    painter.drawText(QRect(startX, foregroundRect.y(), iconWidth, foregroundRect.height()), Qt::AlignCenter, QChar((unsigned short) currentIcon));

    painter.setPen(textColor);
    painter.setFont(textFont);
    painter.drawText(QRect(startX + iconWidth + spacing, foregroundRect.y(), textWidth, foregroundRect.height()), Qt::AlignCenter, btnText);
  }

  painter.restore();
}
