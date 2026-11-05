#include "NXProgressBar.h"

#include <QPropertyAnimation>
#include <QStyleOption>

#include "DeveloperComponents/NXProgressBarStyle.h"
#include "private/NXProgressBarPrivate.h"

NXProgressBar::NXProgressBar(QWidget *parent)
    : QProgressBar(parent)
    , d_ptr(new NXProgressBarPrivate())
{
  Q_D(NXProgressBar);
  setObjectName("NXProgressBar");
  setStyleSheet(QStringLiteral("#NXProgressBar{background-color:transparent;}"));
  d->q_ptr  = this;
  d->m_Style = new NXProgressBarStyle(style());
  setStyle(d->m_Style);
  d->m_BusyAnimation = new QPropertyAnimation(d->m_Style, "busyStartValue");
  connect(d->m_BusyAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    d->m_Style->setProperty("busyEndValue", value.toInt() + 75);
    update();
  });
  d->m_BusyAnimation->setDuration(2000);
  d->m_BusyAnimation->setLoopCount(-1);
}

NXProgressBar::~NXProgressBar()
{
  Q_D(NXProgressBar);
  delete d->m_Style;
}

void NXProgressBar::setMinimum(int minimum)
{
  Q_D(NXProgressBar);
  if (d->m_IsBusyAnimation && !(maximum() == 0 && minimum == 0)) {
    d->m_IsBusyAnimation = false;
    d->m_BusyAnimation->stop();
  }
  QProgressBar::setMinimum(minimum);
}

void NXProgressBar::setMaximum(int maximum)
{
  Q_D(NXProgressBar);
  if (d->m_IsBusyAnimation && !(minimum() == 0 && maximum == 0)) {
    d->m_IsBusyAnimation = false;
    d->m_BusyAnimation->stop();
  }
  QProgressBar::setMaximum(maximum);
}

void NXProgressBar::paintEvent(QPaintEvent *event)
{
  Q_D(NXProgressBar);
  if (!d->m_IsBusyAnimation && minimum() == 0 && maximum() == 0) {
    QStyleOptionProgressBar option;
    option.initFrom(this);
    d->m_IsBusyAnimation = true;
    d->m_BusyAnimation->setStartValue(-75);
    if (orientation() == Qt::Horizontal) {
      d->m_BusyAnimation->setEndValue(this->width());
    } else {
      d->m_BusyAnimation->setEndValue(this->height());
    }
    d->m_BusyAnimation->start();
  }
  QProgressBar::paintEvent(event);
}

void NXProgressBar::resizeEvent(QResizeEvent *event)
{
  Q_D(NXProgressBar);
  if (orientation() == Qt::Horizontal) {
    d->m_BusyAnimation->setEndValue(this->width());
  } else {
    d->m_BusyAnimation->setEndValue(this->height());
  }
  QProgressBar::resizeEvent(event);
}
