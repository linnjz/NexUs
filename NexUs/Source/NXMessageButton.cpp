#include "NXMessageButton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "NXApplication.h"
#include "NXMessageBar.h"
#include "NXTheme.h"
#include "private/NXMessageButtonPrivate.h"
Q_PROPERTY_CREATE_CPP(NXMessageButton, QPS_SCREF_PAEMIT(QString), BarTitle)
Q_PROPERTY_CREATE_CPP(NXMessageButton, QPS_SCREF_PAEMIT(QString), BarText)
Q_PROPERTY_CREATE_CPP(NXMessageButton, QPS_PAEMIT(QWidget *), MessageTargetWidget)
Q_PROPERTY_CREATE_CPP(NXMessageButton, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXMessageButton, QPS_PAEMIT(int), DisplayMsec)
Q_PROPERTY_CREATE_CPP(NXMessageButton, QPS_PAEMIT(NXMessageBarType::MessageMode), MessageMode)
Q_PROPERTY_CREATE_CPP(NXMessageButton, QPS_PAEMIT(NXMessageBarType::PositionPolicy), PositionPolicy)

NXMessageButton::NXMessageButton(QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXMessageButtonPrivate())
{
  Q_D(NXMessageButton);
  d->q_ptr          = this;
  d->m_BorderRadius = 3;
  setMouseTracking(true);
  setFixedSize(80, 38);
  QFont font = this->font();
  font.setPixelSize(nxApp->getFontPixelSize() + 2);
  setFont(font);
  setText(QStringLiteral("Message"));
  setObjectName("NXMessageButton");
  setStyleSheet(QStringLiteral("#NXMessageButton{background-color:transparent;}"));
  d->m_DisplayMsec         = 2000;
  d->m_MessageMode         = NXMessageBarType::Success;
  d->m_PositionPolicy      = NXMessageBarType::TopRight;
  d->m_ThemeMode            = nxTheme->getThemeMode();
  d->m_MessageTargetWidget = nullptr;
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
  connect(this, &NXMessageButton::clicked, this, [=]() {
    switch (d->m_MessageMode) {
    case NXMessageBarType::Success : {
      NXMessageBar::success(d->m_PositionPolicy, d->m_BarTitle, d->m_BarText, d->m_DisplayMsec, d->m_MessageTargetWidget);
      break;
    }
    case NXMessageBarType::Warning : {
      NXMessageBar::warning(d->m_PositionPolicy, d->m_BarTitle, d->m_BarText, d->m_DisplayMsec, d->m_MessageTargetWidget);
      break;
    }
    case NXMessageBarType::Information : {
      NXMessageBar::information(d->m_PositionPolicy, d->m_BarTitle, d->m_BarText, d->m_DisplayMsec, d->m_MessageTargetWidget);
      break;
    }
    case NXMessageBarType::Error : {
      NXMessageBar::error(d->m_PositionPolicy, d->m_BarTitle, d->m_BarText, d->m_DisplayMsec, d->m_MessageTargetWidget);
      break;
    }
    }
  });
}

NXMessageButton::NXMessageButton(QString const &text, QWidget *parent)
    : NXMessageButton(parent)
{
  setText(text);
}

NXMessageButton::~NXMessageButton()
{
}

void NXMessageButton::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXMessageButton);
  if (event->button() == Qt::LeftButton) {
    d->m_IsLeftButtonPress = true;
  }
  QPushButton::mousePressEvent(event);
}

void NXMessageButton::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXMessageButton);
  d->m_IsLeftButtonPress = false;
  QPushButton::mouseReleaseEvent(event);
}

void NXMessageButton::paintEvent(QPaintEvent *event)
{
  Q_D(NXMessageButton);
  QPainter painter(this);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  // 高性能阴影
  nxTheme->drawEffectShadow(&painter, rect(), d->m_ShadowBorderWidth, d->m_BorderRadius);

  // 背景绘制
  painter.save();
  QRect foregroundRect(d->m_PenBorderWidth + d->m_ShadowBorderWidth, d->m_PenBorderWidth + d->m_ShadowBorderWidth, width() - 2 * (d->m_PenBorderWidth + d->m_ShadowBorderWidth), height() - 2 * (d->m_PenBorderWidth + d->m_ShadowBorderWidth));
  painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), d->m_PenBorderWidth));
  painter.setBrush(isEnabled() ? underMouse() ? NXThemeColor(d->m_ThemeMode, BasicHover) : NXThemeColor(d->m_ThemeMode, BasicBase) : NXThemeColor(d->m_ThemeMode, BasicDisable));
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);

  //文字绘制
  painter.setPen(isEnabled() ? d->m_IsLeftButtonPress ? NXThemeColor(d->m_ThemeMode, BasicTextPress) : NXThemeColor(d->m_ThemeMode, BasicText) : NXThemeColor(d->m_ThemeMode, BasicTextDisable));
  painter.drawText(rect(), Qt::AlignCenter, text());
  painter.restore();
}
