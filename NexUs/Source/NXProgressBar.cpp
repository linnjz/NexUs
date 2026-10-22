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
  d->_style = new NXProgressBarStyle(style());
  setStyle(d->_style);
  d->_busyAnimation = new QPropertyAnimation(d->_style, "busyStartValue");
  connect(d->_busyAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
    d->_style->setProperty("busyEndValue", value.toInt() + 75);
    update();
  });
  d->_busyAnimation->setDuration(2000);
  d->_busyAnimation->setLoopCount(-1);
}

NXProgressBar::~NXProgressBar()
{
  Q_D(NXProgressBar);
  delete d->_style;
}

void NXProgressBar::setMinimum(int minimum)
{
  Q_D(NXProgressBar);
  if (d->_isBusyAnimation && !(maximum() == 0 && minimum == 0)) {
    d->_isBusyAnimation = false;
    d->_busyAnimation->stop();
  }
  QProgressBar::setMinimum(minimum);
}

void NXProgressBar::setMaximum(int maximum)
{
  Q_D(NXProgressBar);
  if (d->_isBusyAnimation && !(minimum() == 0 && maximum == 0)) {
    d->_isBusyAnimation = false;
    d->_busyAnimation->stop();
  }
  QProgressBar::setMaximum(maximum);
}

void NXProgressBar::paintEvent(QPaintEvent *event)
{
  Q_D(NXProgressBar);
  if (!d->_isBusyAnimation && minimum() == 0 && maximum() == 0) {
    QStyleOptionProgressBar option;
    option.initFrom(this);
    d->_isBusyAnimation = true;
    d->_busyAnimation->setStartValue(-75);
    if (orientation() == Qt::Horizontal) {
      d->_busyAnimation->setEndValue(this->width());
    } else {
      d->_busyAnimation->setEndValue(this->height());
    }
    d->_busyAnimation->start();
  }
  QProgressBar::paintEvent(event);
}

void NXProgressBar::resizeEvent(QResizeEvent *event)
{
  Q_D(NXProgressBar);
  if (orientation() == Qt::Horizontal) {
    d->_busyAnimation->setEndValue(this->width());
  } else {
    d->_busyAnimation->setEndValue(this->height());
  }
  QProgressBar::resizeEvent(event);
}
