#include "NXSplashScreenPrivate.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include "NXSplashScreen.h"

NXSplashScreenPrivate::NXSplashScreenPrivate(QObject *parent)
    : QObject { parent }
{
}

NXSplashScreenPrivate::~NXSplashScreenPrivate()
{
}

void NXSplashScreenPrivate::_doShowAnimation()
{
  Q_Q(NXSplashScreen);
  q->setWindowOpacity(0);
  QPropertyAnimation *opacityAnimation = new QPropertyAnimation(q, "windowOpacity");
  opacityAnimation->setDuration(400);
  opacityAnimation->setStartValue(0.0);
  opacityAnimation->setEndValue(1.0);
  opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
  opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXSplashScreenPrivate::_doCloseAnimation()
{
  Q_Q(NXSplashScreen);
  QPropertyAnimation *opacityAnimation = new QPropertyAnimation(q, "windowOpacity");
  opacityAnimation->setDuration(300);
  opacityAnimation->setStartValue(1.0);
  opacityAnimation->setEndValue(0.0);
  opacityAnimation->setEasingCurve(QEasingCurve::InOutCubic);
  connect(opacityAnimation, &QPropertyAnimation::finished, q, [=]() {
    q->QWidget::close();
    Q_EMIT q->closed();
  });
  opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}
