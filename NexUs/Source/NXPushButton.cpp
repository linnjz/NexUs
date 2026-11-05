#include "NXPushButton.h"

#include <QPainter>
#include <QPainterPath>

#include "NXApplication.h"
#include "NXIcon.h"
#include "NXTheme.h"
#include "private/NXPushButtonPrivate.h"
Q_PROPERTY_CREATE_CPP(NXPushButton, QPS_SCREF_PAEMIT(QColor), LightDefaultColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QPS_SCREF_PAEMIT(QColor), DarkDefaultColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QPS_SCREF_PAEMIT(QColor), LightHoverColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QPS_SCREF_PAEMIT(QColor), DarkHoverColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QPS_SCREF_PAEMIT(QColor), LightPressColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QPS_SCREF_PAEMIT(QColor), DarkPressColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QPS_SCREF_PAEMIT(QColor), LightTextColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QPS_SCREF_PAEMIT(QColor), DarkTextColor)
Q_PROPERTY_CREATE_CPP(NXPushButton, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXPushButton, QPS_PAEMIT(bool), IsHoverEnable)

NXPushButton::NXPushButton(QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXPushButtonPrivate())
{
  Q_D(NXPushButton);
  d->q_ptr               = this;
  d->m_BorderRadius      = 3;
  d->m_IsHoverEnable     = true;
  d->m_ThemeMode          = nxTheme->getThemeMode();
  d->m_LightDefaultColor = NXThemeColor(NXThemeType::Light, BasicBase);
  d->m_DarkDefaultColor  = NXThemeColor(NXThemeType::Dark, BasicBase);
  d->m_LightHoverColor   = NXThemeColor(NXThemeType::Light, BasicHover);
  d->m_DarkHoverColor    = NXThemeColor(NXThemeType::Dark, BasicHover);
  d->m_LightPressColor   = NXThemeColor(NXThemeType::Light, BasicPress);
  d->m_DarkPressColor    = NXThemeColor(NXThemeType::Dark, BasicPress);
  d->m_LightTextColor    = NXThemeColor(NXThemeType::Light, BasicText);
  d->m_DarkTextColor     = NXThemeColor(NXThemeType::Dark, BasicText);
  setMouseTracking(true);
  setFixedHeight(30);
  QFont font = this->font();
  font.setPixelSize(nxApp->getFontPixelSize() + 2);
  setFont(font);
  setObjectName("NXPushButton");
  setStyleSheet(QStringLiteral("#NXPushButton{background-color:transparent;}"));
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXPushButton::NXPushButton(QString const &text, QWidget *parent)
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

int NXPushButton::getTextPixelSize() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  return this->font().pixelSize();
}

void NXPushButton::setTextPointSize(int size)
{
  QFont font = this->font();
  font.setPointSize(size);
  setFont(font);
}

int NXPushButton::getTextPointSize() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  return this->font().pointSize();
}

void NXPushButton::setTextStyle(NXTextType::TextStyle textStyle)
{
  Q_D(NXPushButton);
  QFont textFont = font();
  d->m_TextStyle  = textStyle;
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

NXTextType::TextStyle NXPushButton::getTextStyle() const noexcept(std::is_nothrow_copy_constructible_v<NXTextType::TextStyle>)
{
  Q_CONST_D(NXPushButton);
  return d->m_TextStyle;
}

void NXPushButton::setNXIcon(NXIconType::IconName icon)
{
  Q_D(NXPushButton);
  d->m_Icon    = icon;
  d->m_HasIcon = (icon != NXIconType::None);
  update();
}

void NXPushButton::setNXIcon(NXIconType::IconName icon, int iconSize)
{
  Q_D(NXPushButton);
  d->m_Icon     = icon;
  d->m_IconSize = iconSize;
  d->m_HasIcon  = (icon != NXIconType::None);
  update();
}

void NXPushButton::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXPushButton);
  d->m_IsPressed = true;
  QPushButton::mousePressEvent(event);
}

void NXPushButton::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXPushButton);
  d->m_IsPressed = false;
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
  if (d->m_ThemeMode == NXThemeType::Light) {
    painter.setPen(NXThemeColor(NXThemeType::Light, BasicBorder));
    painter.setBrush(isEnabled() ? d->m_IsPressed ? d->m_LightPressColor : ((d->m_IsHoverEnable && underMouse()) ? d->m_LightHoverColor : d->m_LightDefaultColor) : NXThemeColor(d->m_ThemeMode, BasicDisable));
  } else {
    painter.setPen(Qt::NoPen);
    painter.setBrush(isEnabled() ? d->m_IsPressed ? d->m_DarkPressColor : ((d->m_IsHoverEnable && underMouse()) ? d->m_DarkHoverColor : d->m_DarkDefaultColor) : NXThemeColor(d->m_ThemeMode, BasicDisable));
  }
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  // 底边线绘制
  if (!d->m_IsPressed) {
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBaseLine));
    painter.drawLine(foregroundRect.x() + d->m_BorderRadius, height() - 1, foregroundRect.width() - d->m_BorderRadius, height() - 1);
  }
  painter.setPen(isEnabled() ? d->m_ThemeMode == NXThemeType::Light ? d->m_LightTextColor : d->m_DarkTextColor : NXThemeColor(d->m_ThemeMode, BasicTextDisable));
  if (d->m_HasIcon) {
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(d->m_IconSize);

    QFontMetrics iconFontMetrics(iconFont);
    int iconWidth = iconFontMetrics.horizontalAdvance(QChar((unsigned short) d->m_Icon));

    QFont textFont = this->font();
    QFontMetrics textFontMetrics(textFont);
    int textWidth = textFontMetrics.horizontalAdvance(text());

    int spacing    = text().isEmpty() ? 0 : 8;
    int totalWidth = iconWidth + spacing + textWidth;
    int startX     = foregroundRect.x() + (foregroundRect.width() - totalWidth) / 2;

    painter.setFont(iconFont);
    painter.drawText(QRect(startX, foregroundRect.y(), iconWidth, foregroundRect.height()), Qt::AlignLeft | Qt::AlignVCenter, QChar((unsigned short) d->m_Icon));

    if (!text().isEmpty()) {
      painter.setFont(textFont);
      painter.drawText(QRect(startX + iconWidth + spacing, foregroundRect.y(), textWidth, foregroundRect.height()), Qt::AlignLeft | Qt::AlignVCenter, text());
    }
  } else {
    painter.drawText(foregroundRect, Qt::AlignCenter, text());
  }
  painter.restore();
}
