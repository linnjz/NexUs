#include "NXToggleButton.h"

#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#include "NXApplication.h"
#include "NXTheme.h"
#include "private/NXToggleButtonPrivate.h"
Q_PROPERTY_CREATE_CPP(NXToggleButton, QPS_SCREF_PAEMIT(QString), Text)
Q_PROPERTY_CREATE_CPP(NXToggleButton, QPS_PAEMIT(int), BorderRadius)

NXToggleButton::NXToggleButton(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXToggleButtonPrivate())
{
  Q_D(NXToggleButton);
  d->q_ptr         = this;
  d->m_ToggleAlpha = {};
  d->m_ThemeMode   = nxTheme->getThemeMode();
  setMouseTracking(true);
  setFixedSize(80, 32);
  QFont font = this->font();
  font.setPixelSize(nxApp->getFontPixelSize() + 2);
  setFont(font);
  setObjectName("NXToggleButton");
  setStyleSheet(QStringLiteral("#NXToggleButton{background-color:transparent;}"));
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXToggleButton::NXToggleButton(QString const &text, QWidget *parent)
    : NXToggleButton(parent)
{
  Q_D(NXToggleButton);
  d->m_Text = text;
}

NXToggleButton::~NXToggleButton()
{
}

void NXToggleButton::setIsToggled(bool isToggled)
{
  Q_D(NXToggleButton);
  d->m_IsToggled                = isToggled;
  d->m_ToggleAlpha             = isToggled ? 255 : 0;
  d->m_IsAlphaAnimationFinished = true;
  update();
  Q_EMIT toggled(isToggled);
}

bool NXToggleButton::getIsToggled() const
{
  Q_CONST_D(NXToggleButton);
  return d->m_IsToggled;
}

void NXToggleButton::setNXIcon(NXIconType::IconName icon)
{
  Q_D(NXToggleButton);
  d->m_NXIcon = icon;
}

bool NXToggleButton::event(QEvent *event)
{
  switch (event->type()) {
  case QEvent::Enter :
  case QEvent::Leave : {
    update();
    break;
  }
  default : {
    break;
  }
  }
  return QWidget::event(event);
}

void NXToggleButton::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXToggleButton);
  d->m_IsPressed = true;
  update();
  QWidget::mouseReleaseEvent(event);
}

void NXToggleButton::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXToggleButton);
  d->m_IsPressed                      = false;
  d->m_IsAlphaAnimationFinished       = false;
  d->m_IsToggled                      = !d->m_IsToggled;
  QPropertyAnimation *alphaAnimation = new QPropertyAnimation(d, "ToggleAlpha");
  connect(alphaAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    update();
  });
  connect(alphaAnimation, &QPropertyAnimation::finished, this, [=]() {
    d->m_IsAlphaAnimationFinished = true;
  });
  alphaAnimation->setDuration(250);
  alphaAnimation->setStartValue(d->m_ToggleAlpha);
  if (d->m_IsToggled) {
    alphaAnimation->setEndValue(255);
  } else {
    alphaAnimation->setEndValue(0);
  }
  alphaAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  Q_EMIT this->toggled(d->m_IsToggled);
  QWidget::mouseReleaseEvent(event);
}

void NXToggleButton::paintEvent(QPaintEvent *event)
{
  Q_D(NXToggleButton);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  painter.save();
  QRect foregroundRect(1, 1, width() - 2, height() - 2);
  if (d->m_IsAlphaAnimationFinished) {
    if (d->m_IsToggled) {
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorder));
      painter.setBrush(isEnabled() ? d->m_IsPressed ? NXThemeColor(d->m_ThemeMode, PrimaryPress) : (underMouse() ? NXThemeColor(d->m_ThemeMode, PrimaryHover) : NXThemeColor(d->m_ThemeMode, PrimaryNormal)) : NXThemeColor(d->m_ThemeMode, BasicDisable));
    } else {
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorder));
      painter.setBrush(isEnabled() ? d->m_IsPressed ? NXThemeColor(d->m_ThemeMode, BasicPress) : (underMouse() ? NXThemeColor(d->m_ThemeMode, BasicHover) : NXThemeColor(d->m_ThemeMode, BasicBase)) : NXThemeColor(d->m_ThemeMode, BasicDisable));
    }
  } else {
    painter.setPen(Qt::NoPen);
    QColor toggleColor = NXThemeColor(d->m_ThemeMode, PrimaryNormal);
    toggleColor.setAlpha(d->m_ToggleAlpha);
    painter.setBrush(toggleColor);
  }
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  // 底边线绘制
  if (!d->m_IsPressed && !d->m_IsToggled) {
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBaseLine));
    painter.drawLine(foregroundRect.x() + d->m_BorderRadius, height() - 1, foregroundRect.x() + foregroundRect.width() - d->m_BorderRadius, height() - 1);
  }

  //文字绘制
  painter.setPen(isEnabled() ? d->m_IsToggled ? NXThemeColor(d->m_ThemeMode, BasicTextInvert) : NXThemeColor(d->m_ThemeMode, BasicText) : NXThemeColor(d->m_ThemeMode, BasicTextDisable));
  if (d->m_NXIcon != NXIconType::None) {
    painter.setFont(QFont(QStringLiteral("NXAwesome")));
    QFontMetrics fm(painter.font());
    QRect iconRect = foregroundRect.adjusted(0, 3, -foregroundRect.width() / 3 * 2 - 5, 0);
    painter.drawText(iconRect, Qt::AlignVCenter | Qt::AlignRight, QChar(d->m_NXIcon));
    painter.setFont(QFont(QStringLiteral("Microsoft YaHei")));
    QRect textRect = foregroundRect.adjusted(foregroundRect.width() / 3 + 5, 0, 0, 0);
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, d->m_Text);
  } else {
    painter.drawText(foregroundRect, Qt::AlignCenter, d->m_Text);
  }
  painter.restore();
}
