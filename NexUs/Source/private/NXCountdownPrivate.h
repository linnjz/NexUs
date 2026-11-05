#ifndef NXCOUNTDOWNPRIVATE_H
#define NXCOUNTDOWNPRIVATE_H

#include <QDateTime>
#include <QObject>

#include "NXDef.h"
class QTimer;
class NXCountdown;

class NXCountdownPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXCountdown)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), FlipAngle)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, DigitWidth)
  Q_PROPERTY_CREATE_D(int, DigitHeight)
  Q_PROPERTY_CREATE_D(int, DigitSpacing)
  Q_PROPERTY_CREATE_D(int, FontPixelSize)
  Q_PROPERTY_CREATE_D(bool, IsShowDays)
  Q_PROPERTY_CREATE_D(bool, IsShowHours)
  Q_PROPERTY_CREATE_D(bool, IsShowMinutes)
  Q_PROPERTY_CREATE_D(bool, IsShowSeconds)

public:
  explicit NXCountdownPrivate(QObject *parent = nullptr);
  ~NXCountdownPrivate();

  Q_SLOT void onTimerTick();
  void _startFlipAnimation();

private:
  friend class NXCountdown;
  bool m_IsRunning { false };
  bool m_UseTargetMode { false };
  NXThemeType::ThemeMode m_ThemeMode;
  int m_PrevSeconds { -1 };
  int m_PrevMinutes { -1 };
  int m_PrevHours { -1 };
  int m_PrevDays { -1 };
  qint64 m_RemainingSeconds { 0 };
  QDateTime m_TargetDateTime;
  QTimer *m_Timer { nullptr };
};

#endif // NXCOUNTDOWNPRIVATE_H
