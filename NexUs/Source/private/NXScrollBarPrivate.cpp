#include "NXScrollBarPrivate.h"

#include <QApplication>
#include <QPropertyAnimation>
#include <QStyleOption>

#include "NXScrollBar.h"

NXScrollBarPrivate::NXScrollBarPrivate(QObject *parent)
    : QObject { parent }
{
}

NXScrollBarPrivate::~NXScrollBarPrivate()
{
}

void NXScrollBarPrivate::onRangeChanged(int min, int max)
{
  Q_Q(NXScrollBar);
  if (q->isVisible() && m_IsAnimation && max != 0) {
    QPropertyAnimation *rangeSmoothAnimation = new QPropertyAnimation(this, "TargetMaximum");
    connect(rangeSmoothAnimation, &QPropertyAnimation::finished, this, [=]() {
      Q_EMIT q->rangeAnimationFinished();
    });
    connect(rangeSmoothAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      q->blockSignals(true);
      q->setMaximum(value.toUInt());
      q->blockSignals(false);
      q->update();
    });
    rangeSmoothAnimation->setEasingCurve(QEasingCurve::OutSine);
    rangeSmoothAnimation->setDuration(250);
    rangeSmoothAnimation->setStartValue(m_TargetMaximum);
    rangeSmoothAnimation->setEndValue(max);
    rangeSmoothAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  } else {
    if (max == 0) {
      m_ScrollValue = -1;
    }
    m_TargetMaximum = max;
  }
}

void NXScrollBarPrivate::_scroll(Qt::KeyboardModifiers modifiers, int delta)
{
  Q_Q(NXScrollBar);
  int stepsToScroll = 0;
  qreal offset      = qreal(delta) / 120;
  int pageStep      = 10;
  int singleStep    = q->singleStep();
  if ((modifiers & Qt::ControlModifier) || (modifiers & Qt::ShiftModifier)) {
    stepsToScroll = qBound(-pageStep, int(offset * pageStep), pageStep);
  } else {
    stepsToScroll = QApplication::wheelScrollLines() * offset * singleStep;
  }
  if (abs(m_ScrollValue - q->value()) > abs(stepsToScroll * m_SpeedLimit)) {
    m_ScrollValue = q->value();
  }
  m_ScrollValue -= stepsToScroll;
  m_SlideSmoothAnimation->stop();
  m_SlideSmoothAnimation->setStartValue(q->value());
  m_SlideSmoothAnimation->setEndValue(m_ScrollValue);
  m_SlideSmoothAnimation->start();
}

int NXScrollBarPrivate::_pixelPosToRangeValue(int pos) const
{
  Q_Q(NXScrollBar const);
  QStyleOptionSlider opt;
  q->initStyleOption(&opt);
  QRect gr = q->style()->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarGroove, q);
  QRect sr = q->style()->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSlider, q);
  int sliderMin, sliderMax, sliderLength;
  if (q->orientation() == Qt::Horizontal) {
    sliderLength = sr.width();
    sliderMin    = gr.x();
    sliderMax    = gr.right() - sliderLength + 1;
    if (q->layoutDirection() == Qt::RightToLeft) {
      opt.upsideDown = !opt.upsideDown;
    }
  } else {
    sliderLength = sr.height();
    sliderMin    = gr.y();
    sliderMax    = gr.bottom() - sliderLength + 1;
  }
  return QStyle::sliderValueFromPosition(q->minimum(), q->maximum(), pos - sliderMin, sliderMax - sliderMin, opt.upsideDown);
}

void NXScrollBarPrivate::_initAllConfig()
{
  Q_Q(NXScrollBar);
  _handleScrollBarRangeChanged(m_OriginScrollBar->minimum(), m_OriginScrollBar->maximum());
  q->setSingleStep(m_OriginScrollBar->singleStep());
  q->setPageStep(m_OriginScrollBar->pageStep());
}

void NXScrollBarPrivate::_handleScrollBarValueChanged(QScrollBar *scrollBar, int value)
{
  scrollBar->setValue(value);
}

void NXScrollBarPrivate::_handleScrollBarRangeChanged(int min, int max)
{
  Q_Q(NXScrollBar);
  q->setRange(min, max);
  if (max <= 0) {
    q->setVisible(false);
  } else {
    q->setVisible(true);
  }
}

void NXScrollBarPrivate::_handleScrollBarGeometry()
{
  Q_Q(NXScrollBar);
  q->raise();
  q->setSingleStep(m_OriginScrollBar->singleStep());
  q->setPageStep(m_OriginScrollBar->pageStep());

  // 从样式中获取滚动条宽度，而不是硬编码
  int scrollBarExtent = q->style()->pixelMetric(QStyle::PM_ScrollBarExtent, nullptr, q);

  if (q->orientation() == Qt::Horizontal) {
    q->setGeometry(0, m_OriginScrollArea->height() - scrollBarExtent, m_OriginScrollArea->width(), scrollBarExtent);
  } else {
    q->setGeometry(m_OriginScrollArea->width() - scrollBarExtent, 0, scrollBarExtent, m_OriginScrollArea->height());
  }
}
