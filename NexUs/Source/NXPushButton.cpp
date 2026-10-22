#include "NXPushButton.h"

#include <QPainter>
#include <QPainterPath>

#include "NXApplication.h"
#include "NXIcon.h"
#include "NXTheme.h"
#include "private/NXPushButtonPrivate.h"
Q_PROPERTY_CREATE_CPP(NXPushButton, QS_SET_CREF(QColor), LightDefaultColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QS_SET_CREF(QColor), DarkDefaultColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QS_SET_CREF(QColor), LightHoverColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QS_SET_CREF(QColor), DarkHoverColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QS_SET_CREF(QColor), LightPressColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QS_SET_CREF(QColor), DarkPressColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QS_SET_CREF(QColor), LightTextColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QS_SET_CREF(QColor), DarkTextColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXPushButton, bool, IsHoverEnable)

NXPushButton::NXPushButton(QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXPushButtonPrivate())
{
  Q_D(NXPushButton);
  d->q_ptr               = this;
  d->_pBorderRadius      = 3;
  d->_pIsHoverEnable     = true;
  d->_themeMode          = nxTheme->getThemeMode();
  d->_pLightDefaultColor = NXThemeColor(NXThemeType::Light, BasicBase);
  d->_pDarkDefaultColor  = NXThemeColor(NXThemeType::Dark, BasicBase);
  d->_pLightHoverColor   = NXThemeColor(NXThemeType::Light, BasicHover);
  d->_pDarkHoverColor    = NXThemeColor(NXThemeType::Dark, BasicHover);
  d->_pLightPressColor   = NXThemeColor(NXThemeType::Light, BasicPress);
  d->_pDarkPressColor    = NXThemeColor(NXThemeType::Dark, BasicPress);
  d->_pLightTextColor    = NXThemeColor(NXThemeType::Light, BasicText);
  d->_pDarkTextColor     = NXThemeColor(NXThemeType::Dark, BasicText);
  setMouseTracking(true);
  setFixedHeight(30);
  QFont font = this->font();
  font.setPixelSize(nxApp->getFontPixelSize() + 2);
  setFont(font);
  setObjectName("NXPushButton");
  setStyleSheet(QStringLiteral("#NXPushButton{background-color:transparent;}"));
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXPushButton::NXPushButton(const QString &text, QWidget *parent)
    : NXPushButton(parent)
{
  setText(text);
}

NXPushButton::~NXPushButton()
{
}

void NXPushButton::setTextPixelSize(int size)
{
  QFont font = this->font();
  font.setPixelSize(size);
  setFont(font);
}

int NXPushButton::getTextPixelSize() const
{
  return this->font().pixelSize();
}

void NXPushButton::setTextPointSize(int size)
{
  QFont font = this->font();
  font.setPointSize(size);
  setFont(font);
}

int NXPushButton::getTextPointSize() const
{
  return this->font().pointSize();
}

void NXPushButton::setTextStyle(NXTextType::TextStyle textStyle)
{
  Q_D(NXPushButton);
  QFont textFont = font();
  d->_textStyle  = textStyle;
  switch (textStyle) {
  case NXTextType::NoStyle : {
    break;
  }
  case NXTextType::Caption : {
    textFont.setPixelSize(12);
    break;
  }
  case NXTextType::Body : {
    textFont.setPixelSize(13);
    break;
  }
  case NXTextType::BodyStrong : {
    textFont.setPixelSize(13);
    textFont.setWeight(QFont::DemiBold);
    break;
  }
  case NXTextType::Subtitle : {
    textFont.setPixelSize(20);
    textFont.setWeight(QFont::DemiBold);
    break;
  }
  case NXTextType::Title : {
    textFont.setPixelSize(28);
    textFont.setWeight(QFont::DemiBold);
    break;
  }
  case NXTextType::TitleLarge : {
    textFont.setPixelSize(40);
    textFont.setWeight(QFont::DemiBold);
    break;
  }
  case NXTextType::Display : {
    textFont.setPixelSize(48);
    textFont.setWeight(QFont::DemiBold);
    break;
  }
  default : {
    break;
  }
  }
  setFont(textFont);
}

NXTextType::TextStyle NXPushButton::getTextStyle() const
{
  Q_D(const NXPushButton);
  return d->_textStyle;
}

void NXPushButton::setNXIcon(NXIconType::IconName icon)
{
  Q_D(NXPushButton);
  d->_icon    = icon;
  d->_hasIcon = (icon != NXIconType::None);
  update();
}

void NXPushButton::setNXIcon(NXIconType::IconName icon, int iconSize)
{
  Q_D(NXPushButton);
  d->_icon     = icon;
  d->_iconSize = iconSize;
  d->_hasIcon  = (icon != NXIconType::None);
  update();
}

void NXPushButton::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXPushButton);
  d->_isPressed = true;
  QPushButton::mousePressEvent(event);
}

void NXPushButton::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXPushButton);
  d->_isPressed = false;
  QPushButton::mouseReleaseEvent(event);
}

void NXPushButton::paintEvent(QPaintEvent *event)
{
  Q_D(NXPushButton);
  QPainter painter(this);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);

  // 背景绘制
  painter.save();
  QRect foregroundRect = rect().adjusted(1, 1, -1, -1);
  if (d->_themeMode == NXThemeType::Light) {
    painter.setPen(NXThemeColor(NXThemeType::Light, BasicBorder));
    painter.setBrush(isEnabled() ? d->_isPressed ? d->_pLightPressColor : ((d->_pIsHoverEnable && underMouse()) ? d->_pLightHoverColor : d->_pLightDefaultColor) : NXThemeColor(d->_themeMode, BasicDisable));
  } else {
    painter.setPen(Qt::NoPen);
    painter.setBrush(isEnabled() ? d->_isPressed ? d->_pDarkPressColor : ((d->_pIsHoverEnable && underMouse()) ? d->_pDarkHoverColor : d->_pDarkDefaultColor) : NXThemeColor(d->_themeMode, BasicDisable));
  }
  painter.drawRoundedRect(foregroundRect, d->_pBorderRadius, d->_pBorderRadius);
  // 底边线绘制
  if (!d->_isPressed) {
    painter.setPen(NXThemeColor(d->_themeMode, BasicBaseLine));
    painter.drawLine(foregroundRect.x() + d->_pBorderRadius, height() - 1, foregroundRect.width() - d->_pBorderRadius, height() - 1);
  }
  painter.setPen(isEnabled() ? d->_themeMode == NXThemeType::Light ? d->_pLightTextColor : d->_pDarkTextColor : NXThemeColor(d->_themeMode, BasicTextDisable));
  if (d->_hasIcon) {
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(d->_iconSize);

    QFontMetrics iconFontMetrics(iconFont);
    int iconWidth = iconFontMetrics.horizontalAdvance(QChar((unsigned short) d->_icon));

    QFont textFont = this->font();
    QFontMetrics textFontMetrics(textFont);
    int textWidth = textFontMetrics.horizontalAdvance(text());

    int spacing    = text().isEmpty() ? 0 : 8;
    int totalWidth = iconWidth + spacing + textWidth;
    int startX     = foregroundRect.x() + (foregroundRect.width() - totalWidth) / 2;

    painter.setFont(iconFont);
    painter.drawText(QRect(startX, foregroundRect.y(), iconWidth, foregroundRect.height()), Qt::AlignLeft | Qt::AlignVCenter, QChar((unsigned short) d->_icon));

    if (!text().isEmpty()) {
      painter.setFont(textFont);
      painter.drawText(QRect(startX + iconWidth + spacing, foregroundRect.y(), textWidth, foregroundRect.height()), Qt::AlignLeft | Qt::AlignVCenter, text());
    }
  } else {
    painter.drawText(foregroundRect, Qt::AlignCenter, text());
  }
  painter.restore();
}
