#include "NXToggleSwitch.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

#include "NXTheme.h"
#include "private/NXToggleSwitchPrivate.h"

NXToggleSwitch::NXToggleSwitch(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXToggleSwitchPrivate())
{
  Q_D(NXToggleSwitch);
  d->q_ptr = this;
  setObjectName("NXToggleSwitch");
  setMouseTracking(true);
  setFixedSize(44, 22);
  d->m_CircleCenterX = -1;
  d->m_IsToggled     = false;
  d->m_ThemeMode     = nxTheme->getThemeMode();
  setProperty("circleCenterX", 0.01);
  setProperty("circleRadius", 0.01);
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXToggleSwitch::~NXToggleSwitch()
{
}

void NXToggleSwitch::setIsToggled(bool isToggled)
{
  Q_D(NXToggleSwitch);
  if (d->m_IsToggled == isToggled) {
    return;
  }
  if (d->m_IsToggled) {
    d->_startPosAnimation(width() - height() / 2 - d->m_Margin * 2, height() / 2, isToggled);
  } else {
    d->_startPosAnimation(height() / 2, width() - height() / 2 - d->m_Margin * 2, isToggled);
  }
}

bool NXToggleSwitch::getIsToggled() const
{
  return d_ptr->m_IsToggled;
}

bool NXToggleSwitch::event(QEvent *event)
{
  Q_D(NXToggleSwitch);
  switch (event->type()) {
  case QEvent::Enter : {
    if (isEnabled()) {
      d->_startRadiusAnimation(height() * 0.3, height() * 0.35);
    }
    break;
  }
  case QEvent::Leave : {
    if (isEnabled()) {
      d->_startRadiusAnimation(height() * 0.35, height() * 0.3);
    }
    break;
  }
  case QEvent::MouseMove : {
    update();
    break;
  }
  default : {
    break;
  }
  }
  return QWidget::event(event);
}

void NXToggleSwitch::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXToggleSwitch);
  d->_adjustCircleCenterX();
  d->m_IsLeftButtonPress = true;
  d->m_LastMouseX        = event->pos().x();
  d->m_PressStartX       = event->pos().x();
  d->m_IsMousePressMove  = false;
  d->_startRadiusAnimation(d->m_CircleRadius, height() * 0.25);
  QWidget::mousePressEvent(event);
}

void NXToggleSwitch::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXToggleSwitch);
  d->m_IsLeftButtonPress = false;
  QWidget::mouseReleaseEvent(event);
  if (d->m_IsMousePressMove) {
    d->m_IsMousePressMove = false;
    if (d->m_CircleCenterX > width() / 2) {
      d->_startPosAnimation(d->m_CircleCenterX, width() - height() / 2 - d->m_Margin * 2, true);
    } else {
      d->_startPosAnimation(d->m_CircleCenterX, height() / 2, false);
    }
  } else {
    if (d->m_IsToggled) {
      d->_startPosAnimation(d->m_CircleCenterX, height() / 2, false);
    } else {
      d->_startPosAnimation(d->m_CircleCenterX, width() - height() / 2 - d->m_Margin * 2, true);
    }
  }
  d->_startRadiusAnimation(height() * 0.25, height() * 0.35);
}

void NXToggleSwitch::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXToggleSwitch);
  if (d->m_IsLeftButtonPress) {
    // 添加移动阈值判断，避免 macOS 上的微小抖动被识别为拖动
    int totalMoveX = qAbs(event->pos().x() - d->m_PressStartX);
    if (totalMoveX > 3) {
      d->m_IsMousePressMove = true;
    }

    if (d->m_IsMousePressMove) {
      int moveX      = event->pos().x() - d->m_LastMouseX;
      d->m_LastMouseX = event->pos().x();
      d->m_CircleCenterX += moveX;
      d->_adjustCircleCenterX();
    }
  }
  QWidget::mouseMoveEvent(event);
}

void NXToggleSwitch::paintEvent(QPaintEvent *event)
{
  Q_D(NXToggleSwitch);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  // 背景绘制
  painter.setPen(d->m_IsToggled ? QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1.5) : QPen(NXThemeColor(d->m_ThemeMode, BasicBorderDeep), 1.5));
  painter.setBrush(isEnabled() ? d->m_IsToggled ? NXThemeColor(d->m_ThemeMode, PrimaryNormal) : (underMouse() ? NXThemeColor(d->m_ThemeMode, BasicHover) : NXThemeColor(d->m_ThemeMode, BasicBase)) : NXThemeColor(d->m_ThemeMode, BasicDisable));
  QPainterPath path;
  path.moveTo(width() - height() - d->m_Margin, height() - d->m_Margin);
  path.arcTo(QRectF(QPointF(width() - height() - d->m_Margin, d->m_Margin), QSize(height() - d->m_Margin * 2, height() - d->m_Margin * 2)), -90, 180);
  path.lineTo(height() / 2 + d->m_Margin, d->m_Margin);
  path.arcTo(QRectF(QPointF(d->m_Margin, d->m_Margin), QSize(height() - d->m_Margin * 2, height() - d->m_Margin * 2)), 90, 180);
  path.lineTo(width() - height() - d->m_Margin, height() - d->m_Margin);
  path.closeSubpath();
  painter.drawPath(path);

  // 圆心绘制
  painter.setPen(Qt::NoPen);
  painter.setBrush(isEnabled() ? d->m_IsToggled ? NXThemeColor(d->m_ThemeMode, BasicTextInvert) : NXThemeColor(d->m_ThemeMode, ToggleSwitchNoToggledCenter) : NXThemeColor(d->m_ThemeMode, BasicTextDisable));
  if (d->m_CircleRadius == 0) {
    d->m_CircleRadius = this->isEnabled() ? (underMouse() ? height() * 0.35 : height() * 0.3) : height() * 0.3;
  }
  if (d->m_IsLeftButtonPress) {
    painter.drawEllipse(QPointF(d->m_CircleCenterX, height() / 2), d->m_CircleRadius, d->m_CircleRadius);
  } else {
    if (d->m_CircleCenterX == -1) {
      d->m_CircleCenterX = d->m_IsToggled ? width() - height() / 2 - d->m_Margin * 2 : height() / 2;
    }
    painter.drawEllipse(QPointF(d->m_CircleCenterX, height() / 2), d->m_CircleRadius, d->m_CircleRadius);
  }
  painter.restore();
}
