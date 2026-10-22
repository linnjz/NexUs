#include "NXMaskWidget.h"

#include <QPainter>
#include <QPropertyAnimation>

NXMaskWidget::NXMaskWidget(QWidget *parent)
    : QWidget { parent }
{
  setObjectName("NXMaskWidget");
  setStyleSheet(QStringLiteral("#NXMaskWidget{background-color:transparent;}"));
  _pMaskAlpha = 0;
}

NXMaskWidget::~NXMaskWidget()
{
}

void NXMaskWidget::doMaskAnimation(int endValue)
{
  QPropertyAnimation *opacityAnimation = new QPropertyAnimation(this, "pMaskAlpha");
  connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
    update();
  });
  connect(opacityAnimation, &QPropertyAnimation::finished, this, [=]() {
    if (endValue == 0) {
      setVisible(false);
    }
    Q_EMIT animationFinished();
  });
  opacityAnimation->setEasingCurve(QEasingCurve::InOutSine);
  opacityAnimation->setDuration(250);
  opacityAnimation->setStartValue(_pMaskAlpha);
  opacityAnimation->setEndValue(endValue);
  opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXMaskWidget::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.save();
  painter.setPen(Qt::NoPen);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setBrush(QColor(0x00, 0x00, 0x00, _pMaskAlpha));
  painter.drawRect(rect());
  painter.restore();
}
