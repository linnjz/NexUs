#include "NXToggleSwitchPrivate.h"

#include <QPropertyAnimation>

#include "NXToggleSwitch.h"

NXToggleSwitchPrivate::NXToggleSwitchPrivate(QObject *parent)
    : QObject { parent }
{
}

NXToggleSwitchPrivate::~NXToggleSwitchPrivate()
{
}

void NXToggleSwitchPrivate::_startPosAnimation(qreal startX, qreal endX, bool isToggle)
{
  Q_Q(NXToggleSwitch);
  QPropertyAnimation *circleAnimation = new QPropertyAnimation(q, "circleCenterX");
  connect(circleAnimation, &QPropertyAnimation::valueChanged, q, [=](QVariant const &value) {
    this->m_CircleCenterX = value.toReal();
    q->update();
  });
  circleAnimation->setStartValue(startX);
  circleAnimation->setEndValue(endX);
  circleAnimation->setEasingCurve(QEasingCurve::InOutSine);
  circleAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  m_IsToggled = isToggle;
  Q_EMIT q->toggled(isToggle);
}

void NXToggleSwitchPrivate::_startRadiusAnimation(qreal startRadius, qreal endRadius)
{
  Q_Q(NXToggleSwitch);
  QPropertyAnimation *circleRadiusAnimation = new QPropertyAnimation(q, "circleRadius");
  connect(circleRadiusAnimation, &QPropertyAnimation::valueChanged, q, [=](QVariant const &value) {
    this->m_CircleRadius = value.toReal();
    q->update();
  });
  circleRadiusAnimation->setEasingCurve(QEasingCurve::InOutSine);
  circleRadiusAnimation->setStartValue(startRadius);
  circleRadiusAnimation->setEndValue(endRadius);
  circleRadiusAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXToggleSwitchPrivate::_adjustCircleCenterX()
{
  Q_Q(NXToggleSwitch);
  if (m_CircleCenterX > q->width() - q->height() / 2 - m_Margin * 2) {
    m_CircleCenterX = q->width() - q->height() / 2 - m_Margin * 2;
  }
  if (m_CircleCenterX < q->height() / 2) {
    m_CircleCenterX = q->height() / 2;
  }
}
