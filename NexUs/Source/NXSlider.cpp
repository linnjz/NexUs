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
  d->m_SliderStyle = new NXSliderStyle(style());
  setStyle(d->m_SliderStyle);

  d->m_ValueToolTip = new NXToolTip(this);
  d->m_ValueToolTip->setToolTip(QString::number(this->value()));
  d->m_ValueToolTip->setIsMoveEnable(true);
  d->m_ValueToolTip->setOffSetX(-20);
  d->m_ValueToolTip->setOffSetY(-60);
  connect(this, &NXSlider::valueChanged, this, [=](int const value) {
    d->m_ValueToolTip->setToolTip(QString::number(value));
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
    d->m_MousePressed = true;
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    if (handleRect.contains(event->pos())) {
      d->m_ValueToolTip->show();
    } else {
      d->m_ValueToolTip->hide();
    }
  }
}

void NXSlider::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXSlider);
  QSlider::mouseMoveEvent(event);
  if (d->m_MousePressed) {
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    if (handleRect.contains(event->pos())) {
      d->m_ValueToolTip->show();
    } else {
      d->m_ValueToolTip->hide();
    }
  } else {
    d->m_ValueToolTip->hide();
  }
}

void NXSlider::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXSlider);
  QSlider::mouseReleaseEvent(event);
  d->m_ValueToolTip->hide();
  d->m_MousePressed = false;
}
