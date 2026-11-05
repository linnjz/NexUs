#include "NXCountdownPrivate.h"

#include <QDateTime>
#include <QPropertyAnimation>
#include <QTimer>

#include "NXCountdown.h"

NXCountdownPrivate::NXCountdownPrivate(QObject *parent)
    : QObject { parent }
{
}

NXCountdownPrivate::~NXCountdownPrivate()
{
}

void NXCountdownPrivate::onTimerTick()
{
  Q_Q(NXCountdown);
  if (m_UseTargetMode) {
    qint64 secs       = QDateTime::currentDateTime().secsTo(m_TargetDateTime);
    m_RemainingSeconds = qMax(static_cast<qint64>(0), secs);
  } else {
    m_RemainingSeconds = qMax(static_cast<qint64>(0), m_RemainingSeconds - 1);
  }

  int curSeconds = static_cast<int>(m_RemainingSeconds % 60);
  if (m_PrevSeconds != curSeconds) {
    _startFlipAnimation();
    m_PrevSeconds = curSeconds;
  }
  m_PrevMinutes = static_cast<int>((m_RemainingSeconds % 3600) / 60);
  m_PrevHours   = static_cast<int>((m_RemainingSeconds % 86400) / 3600);
  m_PrevDays    = static_cast<int>(m_RemainingSeconds / 86400);

  Q_EMIT q->tick(m_RemainingSeconds);
  q->update();

  if (m_RemainingSeconds <= 0) {
    m_IsRunning = false;
    m_Timer->stop();
    Q_EMIT q->timeout();
  }
}

void NXCountdownPrivate::_startFlipAnimation()
{
  Q_Q(NXCountdown);
  m_FlipAngle                  = 0;
  QPropertyAnimation *flipAnim = new QPropertyAnimation(this, "FlipAngle");
  connect(flipAnim, &QPropertyAnimation::valueChanged, q, [=]() {
    q->update();
  });
  flipAnim->setDuration(300);
  flipAnim->setStartValue(0.0);
  flipAnim->setEndValue(180.0);
  flipAnim->setEasingCurve(QEasingCurve::OutCubic);
  flipAnim->start(QAbstractAnimation::DeleteWhenStopped);
}
