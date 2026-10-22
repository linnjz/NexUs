#include "NXToolTipPrivate.h"

#include <QCursor>
#include <QEvent>
#include <QPropertyAnimation>

#include "NXText.h"
#include "NXToolTip.h"

NXToolTipPrivate::NXToolTipPrivate(QObject *parent)
    : QObject(parent)
    , _showTimer(new QTimer(this))
    , _hideTimer(new QTimer(this))
    , _autoHideTimer(new QTimer(this))
{
  _showTimer->setSingleShot(true);
  _hideTimer->setSingleShot(true);
  _autoHideTimer->setSingleShot(true);

  connect(_showTimer, &QTimer::timeout, this, &NXToolTipPrivate::onShowTimeout);
  connect(_hideTimer, &QTimer::timeout, this, &NXToolTipPrivate::onHideTimeout);
  connect(_autoHideTimer, &QTimer::timeout, this, &NXToolTipPrivate::onAutoHideTimeout);
}

NXToolTipPrivate::~NXToolTipPrivate()
{
}

bool NXToolTipPrivate::eventFilter(QObject *watched, QEvent *event)
{
  Q_Q(NXToolTip);
  if (_toolTipText && (!_toolTipText->text().isEmpty() || _pCustomWidget)) {
    switch (event->type()) {
    case QEvent::Enter : {
      _hideTimer->stop();
      _autoHideTimer->stop();
      if (!q->isVisible()) {
        _showTimer->start(_pShowDelayMsec);
      }
      break;
    }
    case QEvent::Leave : {
      _showTimer->stop();
      _autoHideTimer->stop();
      _hideTimer->start(_pHideDelayMsec);
      break;
    }
    case QEvent::HoverMove :
    case QEvent::MouseMove : {
      if (_pIsMoveEnable) {
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
  q->move(cursorPoint.x() + _pOffSetX, cursorPoint.y() + _pOffSetY);
  q->show();
  QPropertyAnimation *showAnimation = new QPropertyAnimation(q, "windowOpacity");
  showAnimation->setEasingCurve(QEasingCurve::InOutSine);
  showAnimation->setDuration(250);
  showAnimation->setStartValue(0);
  showAnimation->setEndValue(1);
  showAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  if (_pDisplayMsec > 0) {
    _autoHideTimer->start(_pDisplayMsec);
  }
}

void NXToolTipPrivate::_updatePos()
{
  Q_Q(NXToolTip);
  QPoint cursorPoint = QCursor::pos();
  q->move(cursorPoint.x() + _pOffSetX, cursorPoint.y() + _pOffSetY);
}

void NXToolTipPrivate::_stopAllTimers()
{
  _showTimer->stop();
  _hideTimer->stop();
  _autoHideTimer->stop();
}
