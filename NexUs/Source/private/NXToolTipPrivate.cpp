#include "NXToolTipPrivate.h"

#include <QCursor>
#include <QEvent>
#include <QPropertyAnimation>

#include "NXText.h"
#include "NXToolTip.h"

NXToolTipPrivate::NXToolTipPrivate(QObject *parent)
    : QObject(parent)
    , m_ShowTimer(new QTimer(this))
    , m_HideTimer(new QTimer(this))
    , m_AutoHideTimer(new QTimer(this))
{
  m_ShowTimer->setSingleShot(true);
  m_HideTimer->setSingleShot(true);
  m_AutoHideTimer->setSingleShot(true);

  connect(m_ShowTimer, &QTimer::timeout, this, &NXToolTipPrivate::onShowTimeout);
  connect(m_HideTimer, &QTimer::timeout, this, &NXToolTipPrivate::onHideTimeout);
  connect(m_AutoHideTimer, &QTimer::timeout, this, &NXToolTipPrivate::onAutoHideTimeout);
}

NXToolTipPrivate::~NXToolTipPrivate()
{
}

bool NXToolTipPrivate::eventFilter(QObject *watched, QEvent *event)
{
  Q_Q(NXToolTip);
  if (m_ToolTipText && (!m_ToolTipText->text().isEmpty() || m_CustomWidget)) {
    switch (event->type()) {
    case QEvent::Enter : {
      m_HideTimer->stop();
      m_AutoHideTimer->stop();
      if (!q->isVisible()) {
        m_ShowTimer->start(m_ShowDelayMsec);
      }
      break;
    }
    case QEvent::Leave : {
      m_ShowTimer->stop();
      m_AutoHideTimer->stop();
      m_HideTimer->start(m_HideDelayMsec);
      break;
    }
    case QEvent::HoverMove :
    case QEvent::MouseMove : {
      if (m_IsMoveEnable) {
        _updatePos();
      }
      break;
    }
    default : break;
    }
  }
  return QObject::eventFilter(watched, event);
}

void NXToolTipPrivate::onShowTimeout()
{
  _doShowAnimation();
}

void NXToolTipPrivate::onHideTimeout()
{
  Q_Q(NXToolTip);
  q->hide();
  _stopAllTimers();
}

void NXToolTipPrivate::onAutoHideTimeout()
{
  Q_Q(NXToolTip);
  q->hide();
  _stopAllTimers();
}

void NXToolTipPrivate::_doShowAnimation()
{
  Q_Q(NXToolTip);
  QPoint cursorPoint = QCursor::pos();
  q->move(cursorPoint.x() + m_OffSetX, cursorPoint.y() + m_OffSetY);
  q->show();
  QPropertyAnimation *showAnimation = new QPropertyAnimation(q, "windowOpacity");
  showAnimation->setEasingCurve(QEasingCurve::InOutSine);
  showAnimation->setDuration(250);
  showAnimation->setStartValue(0);
  showAnimation->setEndValue(1);
  showAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  if (m_DisplayMsec > 0) {
    m_AutoHideTimer->start(m_DisplayMsec);
  }
}

void NXToolTipPrivate::_updatePos()
{
  Q_Q(NXToolTip);
  QPoint cursorPoint = QCursor::pos();
  q->move(cursorPoint.x() + m_OffSetX, cursorPoint.y() + m_OffSetY);
}

void NXToolTipPrivate::_stopAllTimers()
{
  m_ShowTimer->stop();
  m_HideTimer->stop();
  m_AutoHideTimer->stop();
}
