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
  connect(circleAnimation, &QPropertyAnimation::valueChanged, q, [=](const QVariant &value) {
    this->_circleCenterX = value.toReal();
    q->update();
  });
  circleAnimation->setStartValue(startX);
  circleAnimation->setEndValue(endX);
  circleAnimation->setEasingCurve(QEasingCurve::InOutSine);
  circleAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  _isToggled = isToggle;
  Q_EMIT q->toggled(isToggle);
}

void NXToggleSwitchPrivate::_startRadiusAnimation(qreal startRadius, qreal endRadius)
{
  Q_Q(NXToggleSwitch);
  QPropertyAnimation *circleRadiusAnimation = new QPropertyAnimation(q, "circleRadius");
  connect(circleRadiusAnimation, &QPropertyAnimation::valueChanged, q, [=](const QVariant &value) {
    this->_circleRadius = value.toReal();
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
  if (_circleCenterX > q->width() - q->height() / 2 - _margin * 2) {
    _circleCenterX = q->width() - q->height() / 2 - _margin * 2;
  }
  if (_circleCenterX < q->height() / 2) {
    _circleCenterX = q->height() / 2;
  }
}
