#include "NXSlider.h"

#include <QMouseEvent>
#include <QStyleOptionSlider>

#include "DeveloperComponents/NXSliderStyle.h"
#include "NXToolTip.h"
#include "private/NXSliderPrivate.h"

NXSlider::NXSlider(QWidget *parent)
    : QSlider(parent)
    , d_ptr(new NXSliderPrivate())
{
  Q_D(NXSlider);
  d->q_ptr = this;
#ifdef Q_OS_MACOS
  setAttribute(Qt::WA_Hover);
#endif
  setOrientation(Qt::Horizontal);
  setStyle(new NXSliderStyle(style()));
  d->_sliderStyle = new NXSliderStyle(style());
  setStyle(d->_sliderStyle);

  d->_valueToolTip = new NXToolTip(this);
  d->_valueToolTip->setToolTip(QString::number(this->value()));
  d->_valueToolTip->setIsMoveEnable(true);
  d->_valueToolTip->setOffSetX(-20);
  d->_valueToolTip->setOffSetY(-60);
  connect(this, &NXSlider::valueChanged, this, [=](const int value) {
    d->_valueToolTip->setToolTip(QString::number(value));
  });
}

NXSlider::NXSlider(Qt::Orientation orientation, QWidget *parent)
    : NXSlider(parent)
{
  setOrientation(orientation);
}

NXSlider::~NXSlider()
{
  delete this->style();
}

void NXSlider::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXSlider);
  QSlider::mousePressEvent(event);
  if (event->button() == Qt::LeftButton) {
    d->_mousePressed = true;
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    if (handleRect.contains(event->pos())) {
      d->_valueToolTip->show();
    } else {
      d->_valueToolTip->hide();
    }
  }
}

void NXSlider::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXSlider);
  QSlider::mouseMoveEvent(event);
  if (d->_mousePressed) {
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    if (handleRect.contains(event->pos())) {
      d->_valueToolTip->show();
    } else {
      d->_valueToolTip->hide();
    }
  } else {
    d->_valueToolTip->hide();
  }
}

void NXSlider::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXSlider);
  QSlider::mouseReleaseEvent(event);
  d->_valueToolTip->hide();
  d->_mousePressed = false;
}
