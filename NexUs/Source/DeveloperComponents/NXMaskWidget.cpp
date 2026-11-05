#include "NXMaskWidget.h"

#include <QPainter>
#include <QPropertyAnimation>

NXMaskWidget::NXMaskWidget(QWidget *parent)
    : QWidget { parent }
    , m_MaskAlpha { 0 }
{
  setObjectName("NXMaskWidget");
  setStyleSheet(QStringLiteral("#NXMaskWidget{background-color:transparent;}"));
}

NXMaskWidget::~NXMaskWidget()
{
}

void NXMaskWidget::doMaskAnimation(int endValue)
{
  QPropertyAnimation *opacityAnimation = new QPropertyAnimation(this, "MaskAlpha");
  connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
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
  opacityAnimation->setStartValue(m_MaskAlpha);
  opacityAnimation->setEndValue(endValue);
  opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXMaskWidget::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.save();
  painter.setPen(Qt::NoPen);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setBrush(QColor(0x00, 0x00, 0x00, m_MaskAlpha));
  painter.drawRect(rect());
  painter.restore();
}
