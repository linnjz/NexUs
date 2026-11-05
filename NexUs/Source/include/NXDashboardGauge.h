#ifndef NXDASHBOARDGAUGE_H
#define NXDASHBOARDGAUGE_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXDashboardGaugePrivate;

class NX_EXPORT NXDashboardGauge : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXDashboardGauge)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Title)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Unit)
  Q_PROPERTY_CREATE_H(qreal, Minimum)
  Q_PROPERTY_CREATE_H(qreal, Maximum)
  Q_PROPERTY_CREATE_H(qreal, Value)
  Q_PROPERTY_CREATE_H(int, MajorTickCount)
  Q_PROPERTY_CREATE_H(int, MinorTickCount)
  Q_PROPERTY_CREATE_H(int, StartAngle)
  Q_PROPERTY_CREATE_H(int, SpanAngle)
  Q_PROPERTY_CREATE_H(int, ArcWidth)
  Q_PROPERTY_CREATE_H(int, ValuePixelSize)
  Q_PROPERTY_CREATE_H(int, Decimals)
  Q_PROPERTY_CREATE_H(bool, IsAnimated)

public:
  explicit NXDashboardGauge(QWidget *parent = nullptr);
  ~NXDashboardGauge();

  void setDangerPercent(qreal percent);
  qreal getDangerPercent() const;

  void setWarningPercent(qreal percent);
  qreal getWarningPercent() const;

  void setTickWarningPercent(qreal percent);
  qreal getTickWarningPercent() const;
  Q_SIGNAL void valueChanged(qreal value);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXDASHBOARDGAUGE_H
