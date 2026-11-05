#ifndef NXCOUNTDOWN_H
#define NXCOUNTDOWN_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXCountdownPrivate;

class NX_EXPORT NXCountdown : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXCountdown)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, DigitWidth)
  Q_PROPERTY_CREATE_H(int, DigitHeight)
  Q_PROPERTY_CREATE_H(int, DigitSpacing)
  Q_PROPERTY_CREATE_H(int, FontPixelSize)
  Q_PROPERTY_CREATE_H(bool, IsShowDays)
  Q_PROPERTY_CREATE_H(bool, IsShowHours)
  Q_PROPERTY_CREATE_H(bool, IsShowMinutes)
  Q_PROPERTY_CREATE_H(bool, IsShowSeconds)

public:
  explicit NXCountdown(QWidget *parent = nullptr);
  ~NXCountdown();

  void setTargetDateTime(QDateTime const &dateTime);
  QDateTime getTargetDateTime() const;

  void setRemainingSeconds(qint64 seconds);
  qint64 getRemainingSeconds() const;

  void start();
  void pause();
  void resume();
  void stop();

  bool isRunning() const;
  Q_SIGNAL void timeout();
  Q_SIGNAL void tick(qint64 remainingSeconds);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  void _updateSize();
};

#endif // NXCOUNTDOWN_H
