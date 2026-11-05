#include "NXIconButton.h"

#include <QEvent>
#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#include "NXApplication.h"
#include "NXTheme.h"
#include "private/NXIconButtonPrivate.h"
Q_PROPERTY_CREATE_CPP(NXIconButton, QPS_SCREF_PAEMIT(QColor), LightHoverColor)
Q_PROPERTY_CREATE_CPP(NXIconButton, QPS_SCREF_PAEMIT(QColor), DarkHoverColor)
Q_PROPERTY_CREATE_CPP(NXIconButton, QPS_SCREF_PAEMIT(QColor), LightIconColor)
Q_PROPERTY_CREATE_CPP(NXIconButton, QPS_SCREF_PAEMIT(QColor), DarkIconColor)
Q_PROPERTY_CREATE_CPP(NXIconButton, QPS_SCREF_PAEMIT(QColor), LightHoverIconColor)
Q_PROPERTY_CREATE_CPP(NXIconButton, QPS_SCREF_PAEMIT(QColor), DarkHoverIconColor)
Q_PROPERTY_CREATE_CPP(NXIconButton, QPS_PAEMIT(qreal), Opacity)
Q_PROPERTY_CREATE_CPP(NXIconButton, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXIconButton, QPS_PAEMIT(bool), IsSelected)

NXIconButton::NXIconButton(QPixmap const &pix, QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXIconButtonPrivate())
{
  Q_D(NXIconButton);
  d->q_ptr                 = this;
  d->m_IconPix              = pix.copy();
  d->m_HoverAlpha          = 0;
  d->m_Opacity             = 1;
  d->m_LightHoverColor     = NXThemeColor(NXThemeType::Light, BasicHoverAlpha);
  d->m_DarkHoverColor      = NXThemeColor(NXThemeType::Dark, BasicHoverAlpha);
  d->m_LightIconColor      = NXThemeColor(NXThemeType::Light, BasicText);
  d->m_DarkIconColor       = NXThemeColor(NXThemeType::Dark, BasicText);
  d->m_LightHoverIconColor = NXThemeColor(NXThemeType::Light, BasicText);
  d->m_DarkHoverIconColor  = NXThemeColor(NXThemeType::Dark, BasicText);
  d->m_IsSelected          = false;
  d->m_BorderRadius        = 0;
  d->m_ThemeMode            = nxTheme->getThemeMode();
  connect(this, &NXIconButton::signalIsSelectedChanged, this, [=]() {
    update();
  });
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXIconButton::NXIconButton(NXIconType::IconName awesome, QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXIconButtonPrivate())
{
  Q_D(NXIconButton);
  d->q_ptr                 = this;
  d->m_HoverAlpha          = 0;
  d->m_Opacity             = 1;
  d->m_LightHoverColor     = NXThemeColor(NXThemeType::Light, BasicHoverAlpha);
  d->m_DarkHoverColor      = NXThemeColor(NXThemeType::Dark, BasicHoverAlpha);
  d->m_LightIconColor      = NXThemeColor(NXThemeType::Light, BasicText);
  d->m_DarkIconColor       = NXThemeColor(NXThemeType::Dark, BasicText);
  d->m_LightHoverIconColor = NXThemeColor(NXThemeType::Light, BasicText);
  d->m_DarkHoverIconColor  = NXThemeColor(NXThemeType::Dark, BasicText);
  d->m_IsSelected          = false;
  d->m_BorderRadius        = 0;
  d->m_ThemeMode            = nxTheme->getThemeMode();
  QFont iconFont           = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(nxApp->getFontPixelSize() + 2);
  this->setFont(iconFont);
  d->m_Awesome = awesome;
  this->setText(QChar((unsigned short) awesome));
  connect(this, &NXIconButton::signalIsSelectedChanged, this, [=]() {
    update();
  });
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXIconButton::NXIconButton(NXIconType::IconName awesome, int pixelSize, QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXIconButtonPrivate())
{
  Q_D(NXIconButton);
  d->q_ptr                 = this;
  d->m_HoverAlpha          = 0;
  d->m_Opacity             = 1;
  d->m_LightHoverColor     = NXThemeColor(NXThemeType::Light, BasicHoverAlpha);
  d->m_DarkHoverColor      = NXThemeColor(NXThemeType::Dark, BasicHoverAlpha);
  d->m_LightIconColor      = NXThemeColor(NXThemeType::Light, BasicText);
  d->m_DarkIconColor       = NXThemeColor(NXThemeType::Dark, BasicText);
  d->m_LightHoverIconColor = NXThemeColor(NXThemeType::Light, BasicText);
  d->m_DarkHoverIconColor  = NXThemeColor(NXThemeType::Dark, BasicText);
  d->m_IsSelected          = false;
  d->m_BorderRadius        = 0;
  d->m_ThemeMode            = nxTheme->getThemeMode();
  QFont iconFont           = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(pixelSize);
  this->setFont(iconFont);
  d->m_Awesome = awesome;
  this->setText(QChar((unsigned short) awesome));
  connect(this, &NXIconButton::signalIsSelectedChanged, this, [=]() {
    update();
  });
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXIconButton::NXIconButton(
    NXIconType::IconName awesome, int pixelSize, int fixedWidth, int fixedHeight, QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXIconButtonPrivate())
{
  Q_D(NXIconButton);
  d->q_ptr                 = this;
  d->m_HoverAlpha          = 0;
  d->m_Opacity             = 1;
  d->m_LightHoverColor     = NXThemeColor(NXThemeType::Light, BasicHoverAlpha);
  d->m_DarkHoverColor      = NXThemeColor(NXThemeType::Dark, BasicHoverAlpha);
  d->m_LightIconColor      = NXThemeColor(NXThemeType::Light, BasicText);
  d->m_DarkIconColor       = NXThemeColor(NXThemeType::Dark, BasicText);
  d->m_LightHoverIconColor = NXThemeColor(NXThemeType::Light, BasicText);
  d->m_DarkHoverIconColor  = NXThemeColor(NXThemeType::Dark, BasicText);
  d->m_IsSelected          = false;
  d->m_BorderRadius        = 0;
  d->m_ThemeMode            = nxTheme->getThemeMode();
  QFont iconFont           = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(pixelSize);
  this->setFont(iconFont);
  d->m_Awesome = awesome;
  this->setText(QChar((unsigned short) awesome));
  this->setFixedSize(fixedWidth, fixedHeight);
  connect(this, &NXIconButton::signalIsSelectedChanged, this, [=]() {
    update();
  });
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXIconButton::~NXIconButton()
{
}

void NXIconButton::setAwesome(NXIconType::IconName awesome)
{
  Q_D(NXIconButton);
  d->m_Awesome = awesome;
  this->setText(QChar((unsigned short) awesome));
}

NXIconType::IconName NXIconButton::getAwesome() const
{
  return this->d_ptr->m_Awesome;
}

void NXIconButton::setPixmap(QPixmap const &pix)
{
  Q_D(NXIconButton);
  d->m_IconPix = pix.copy();
}

bool NXIconButton::event(QEvent *event)
{
  Q_D(NXIconButton);
  switch (event->type()) {
  case QEvent::Enter : {
    if (isEnabled() && !d->m_IsSelected) {
      d->m_IsAlphaAnimationFinished       = false;
      QPropertyAnimation *alphaAnimation = new QPropertyAnimation(d, "HoverAlpha");
      connect(alphaAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
        update();
      });
      connect(alphaAnimation, &QPropertyAnimation::finished, this, [=]() {
        d->m_IsAlphaAnimationFinished = true;
      });
      alphaAnimation->setDuration(175);
      alphaAnimation->setStartValue(d->m_HoverAlpha);
      alphaAnimation->setEndValue(d->m_ThemeMode == NXThemeType::Light ? d->m_LightHoverColor.alpha() : d->m_DarkHoverColor.alpha());
      alphaAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    break;
  }
  case QEvent::Leave : {
    if (isEnabled() && !d->m_IsSelected) {
      d->m_IsAlphaAnimationFinished       = false;
      QPropertyAnimation *alphaAnimation = new QPropertyAnimation(d, "HoverAlpha");
      connect(alphaAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
        update();
      });
      connect(alphaAnimation, &QPropertyAnimation::finished, this, [=]() {
        d->m_IsAlphaAnimationFinished = true;
      });
      alphaAnimation->setDuration(175);
      alphaAnimation->setStartValue(d->m_HoverAlpha);
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
  painter.setOpacity(d->m_Opacity);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  if (d->m_IsAlphaAnimationFinished || d->m_IsSelected) {
    painter.setBrush(d->m_IsSelected ? d->m_ThemeMode == NXThemeType::Light ? d->m_LightHoverColor : d->m_DarkHoverColor : isEnabled() ? underMouse() ? d->m_ThemeMode == NXThemeType::Light ? d->m_LightHoverColor : d->m_DarkHoverColor : Qt::transparent
                                                                                                                                      : Qt::transparent);
  } else {
    QColor hoverColor = d->m_ThemeMode == NXThemeType::Light ? d->m_LightHoverColor : d->m_DarkHoverColor;
    hoverColor.setAlpha(d->m_HoverAlpha);
    painter.setBrush(hoverColor);
  }
  painter.drawRoundedRect(rect(), d->m_BorderRadius, d->m_BorderRadius);
  // 图标绘制
  if (!d->m_IconPix.isNull()) {
    QPainterPath path;
    path.addEllipse(rect());
    painter.setClipPath(path);
    painter.drawPixmap(rect(), d->m_IconPix);
  } else {
    painter.setPen(isEnabled() ? d->m_ThemeMode == NXThemeType::Light ? underMouse() ? d->m_LightHoverIconColor : d->m_LightIconColor : underMouse() ? d->m_DarkHoverIconColor
                                                                                                                                                    : d->m_DarkIconColor
                               : NXThemeColor(d->m_ThemeMode, BasicTextDisable));
    painter.drawText(rect(), Qt::AlignCenter, QChar((unsigned short) d->m_Awesome));
  }
  painter.restore();
}
