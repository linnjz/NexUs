#include "NXIconButton.h"

#include <QEvent>
#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#include "NXApplication.h"
#include "NXTheme.h"
#include "private/NXIconButtonPrivate.h"
Q_PROPERTY_CREATE_CPP(NXIconButton, QS_SET_CREF(QColor), LightHoverColor)
Q_PROPERTY_CREATE_CPP(NXIconButton, QS_SET_CREF(QColor), DarkHoverColor)
Q_PROPERTY_CREATE_CPP(NXIconButton, QS_SET_CREF(QColor), LightIconColor)
Q_PROPERTY_CREATE_CPP(NXIconButton, QS_SET_CREF(QColor), DarkIconColor)
Q_PROPERTY_CREATE_CPP(NXIconButton, QS_SET_CREF(QColor), LightHoverIconColor)
Q_PROPERTY_CREATE_CPP(NXIconButton, QS_SET_CREF(QColor), DarkHoverIconColor)
Q_PROPERTY_CREATE_CPP(NXIconButton, qreal, Opacity)
Q_PROPERTY_CREATE_CPP(NXIconButton, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXIconButton, bool, IsSelected)

NXIconButton::NXIconButton(const QPixmap &pix, QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXIconButtonPrivate())
{
  Q_D(NXIconButton);
  d->q_ptr                 = this;
  d->_iconPix              = pix.copy();
  d->_pHoverAlpha          = 0;
  d->_pOpacity             = 1;
  d->_pLightHoverColor     = NXThemeColor(NXThemeType::Light, BasicHoverAlpha);
  d->_pDarkHoverColor      = NXThemeColor(NXThemeType::Dark, BasicHoverAlpha);
  d->_pLightIconColor      = NXThemeColor(NXThemeType::Light, BasicText);
  d->_pDarkIconColor       = NXThemeColor(NXThemeType::Dark, BasicText);
  d->_pLightHoverIconColor = NXThemeColor(NXThemeType::Light, BasicText);
  d->_pDarkHoverIconColor  = NXThemeColor(NXThemeType::Dark, BasicText);
  d->_pIsSelected          = false;
  d->_pBorderRadius        = 0;
  d->_themeMode            = nxTheme->getThemeMode();
  connect(this, &NXIconButton::pIsSelectedChanged, this, [=]() {
    update();
  });
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXIconButton::NXIconButton(NXIconType::IconName awesome, QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXIconButtonPrivate())
{
  Q_D(NXIconButton);
  d->q_ptr                 = this;
  d->_pHoverAlpha          = 0;
  d->_pOpacity             = 1;
  d->_pLightHoverColor     = NXThemeColor(NXThemeType::Light, BasicHoverAlpha);
  d->_pDarkHoverColor      = NXThemeColor(NXThemeType::Dark, BasicHoverAlpha);
  d->_pLightIconColor      = NXThemeColor(NXThemeType::Light, BasicText);
  d->_pDarkIconColor       = NXThemeColor(NXThemeType::Dark, BasicText);
  d->_pLightHoverIconColor = NXThemeColor(NXThemeType::Light, BasicText);
  d->_pDarkHoverIconColor  = NXThemeColor(NXThemeType::Dark, BasicText);
  d->_pIsSelected          = false;
  d->_pBorderRadius        = 0;
  d->_themeMode            = nxTheme->getThemeMode();
  QFont iconFont           = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(nxApp->getFontPixelSize() + 2);
  this->setFont(iconFont);
  d->_pAwesome = awesome;
  this->setText(QChar((unsigned short) awesome));
  connect(this, &NXIconButton::pIsSelectedChanged, this, [=]() {
    update();
  });
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXIconButton::NXIconButton(NXIconType::IconName awesome, int pixelSize, QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXIconButtonPrivate())
{
  Q_D(NXIconButton);
  d->q_ptr                 = this;
  d->_pHoverAlpha          = 0;
  d->_pOpacity             = 1;
  d->_pLightHoverColor     = NXThemeColor(NXThemeType::Light, BasicHoverAlpha);
  d->_pDarkHoverColor      = NXThemeColor(NXThemeType::Dark, BasicHoverAlpha);
  d->_pLightIconColor      = NXThemeColor(NXThemeType::Light, BasicText);
  d->_pDarkIconColor       = NXThemeColor(NXThemeType::Dark, BasicText);
  d->_pLightHoverIconColor = NXThemeColor(NXThemeType::Light, BasicText);
  d->_pDarkHoverIconColor  = NXThemeColor(NXThemeType::Dark, BasicText);
  d->_pIsSelected          = false;
  d->_pBorderRadius        = 0;
  d->_themeMode            = nxTheme->getThemeMode();
  QFont iconFont           = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(pixelSize);
  this->setFont(iconFont);
  d->_pAwesome = awesome;
  this->setText(QChar((unsigned short) awesome));
  connect(this, &NXIconButton::pIsSelectedChanged, this, [=]() {
    update();
  });
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXIconButton::NXIconButton(
    NXIconType::IconName awesome, int pixelSize, int fixedWidth, int fixedHeight, QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXIconButtonPrivate())
{
  Q_D(NXIconButton);
  d->q_ptr                 = this;
  d->_pHoverAlpha          = 0;
  d->_pOpacity             = 1;
  d->_pLightHoverColor     = NXThemeColor(NXThemeType::Light, BasicHoverAlpha);
  d->_pDarkHoverColor      = NXThemeColor(NXThemeType::Dark, BasicHoverAlpha);
  d->_pLightIconColor      = NXThemeColor(NXThemeType::Light, BasicText);
  d->_pDarkIconColor       = NXThemeColor(NXThemeType::Dark, BasicText);
  d->_pLightHoverIconColor = NXThemeColor(NXThemeType::Light, BasicText);
  d->_pDarkHoverIconColor  = NXThemeColor(NXThemeType::Dark, BasicText);
  d->_pIsSelected          = false;
  d->_pBorderRadius        = 0;
  d->_themeMode            = nxTheme->getThemeMode();
  QFont iconFont           = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(pixelSize);
  this->setFont(iconFont);
  d->_pAwesome = awesome;
  this->setText(QChar((unsigned short) awesome));
  this->setFixedSize(fixedWidth, fixedHeight);
  connect(this, &NXIconButton::pIsSelectedChanged, this, [=]() {
    update();
  });
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXIconButton::~NXIconButton()
{
}

void NXIconButton::setAwesome(NXIconType::IconName awesome)
{
  Q_D(NXIconButton);
  d->_pAwesome = awesome;
  this->setText(QChar((unsigned short) awesome));
}

NXIconType::IconName NXIconButton::getAwesome() const
{
  return this->d_ptr->_pAwesome;
}

void NXIconButton::setPixmap(const QPixmap &pix)
{
  Q_D(NXIconButton);
  d->_iconPix = pix.copy();
}

bool NXIconButton::event(QEvent *event)
{
  Q_D(NXIconButton);
  switch (event->type()) {
  case QEvent::Enter : {
    if (isEnabled() && !d->_pIsSelected) {
      d->_isAlphaAnimationFinished       = false;
      QPropertyAnimation *alphaAnimation = new QPropertyAnimation(d, "pHoverAlpha");
      connect(alphaAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
        update();
      });
      connect(alphaAnimation, &QPropertyAnimation::finished, this, [=]() {
        d->_isAlphaAnimationFinished = true;
      });
      alphaAnimation->setDuration(175);
      alphaAnimation->setStartValue(d->_pHoverAlpha);
      alphaAnimation->setEndValue(d->_themeMode == NXThemeType::Light ? d->_pLightHoverColor.alpha() : d->_pDarkHoverColor.alpha());
      alphaAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    break;
  }
  case QEvent::Leave : {
    if (isEnabled() && !d->_pIsSelected) {
      d->_isAlphaAnimationFinished       = false;
      QPropertyAnimation *alphaAnimation = new QPropertyAnimation(d, "pHoverAlpha");
      connect(alphaAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
        update();
      });
      connect(alphaAnimation, &QPropertyAnimation::finished, this, [=]() {
        d->_isAlphaAnimationFinished = true;
      });
      alphaAnimation->setDuration(175);
      alphaAnimation->setStartValue(d->_pHoverAlpha);
      alphaAnimation->setEndValue(0);
      alphaAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    break;
  }
  default : {
    break;
  }
  }
  return QPushButton::event(event);
}

void NXIconButton::paintEvent(QPaintEvent *event)
{
  Q_D(NXIconButton);
  QPainter painter(this);
  painter.save();
  painter.setOpacity(d->_pOpacity);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  if (d->_isAlphaAnimationFinished || d->_pIsSelected) {
    painter.setBrush(d->_pIsSelected ? d->_themeMode == NXThemeType::Light ? d->_pLightHoverColor : d->_pDarkHoverColor : isEnabled() ? underMouse() ? d->_themeMode == NXThemeType::Light ? d->_pLightHoverColor : d->_pDarkHoverColor : Qt::transparent
                                                                                                                                      : Qt::transparent);
  } else {
    QColor hoverColor = d->_themeMode == NXThemeType::Light ? d->_pLightHoverColor : d->_pDarkHoverColor;
    hoverColor.setAlpha(d->_pHoverAlpha);
    painter.setBrush(hoverColor);
  }
  painter.drawRoundedRect(rect(), d->_pBorderRadius, d->_pBorderRadius);
  // 图标绘制
  if (!d->_iconPix.isNull()) {
    QPainterPath path;
    path.addEllipse(rect());
    painter.setClipPath(path);
    painter.drawPixmap(rect(), d->_iconPix);
  } else {
    painter.setPen(isEnabled() ? d->_themeMode == NXThemeType::Light ? underMouse() ? d->_pLightHoverIconColor : d->_pLightIconColor : underMouse() ? d->_pDarkHoverIconColor
                                                                                                                                                    : d->_pDarkIconColor
                               : NXThemeColor(d->_themeMode, BasicTextDisable));
    painter.drawText(rect(), Qt::AlignCenter, QChar((unsigned short) d->_pAwesome));
  }
  painter.restore();
}
