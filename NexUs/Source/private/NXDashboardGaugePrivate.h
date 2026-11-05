#ifndef NXDASHBOARDGAUGEPRIVATE_H
#define NXDASHBOARDGAUGEPRIVATE_H

#include <QObject>

#include "NXDef.h"
#include "NXProperty.h"

class NXDashboardGauge;

class NXDashboardGaugePrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXDashboardGauge)
  Q_PROPERTY_CREATE_D(QString, Title)
  Q_PROPERTY_CREATE_D(QString, Unit)
  Q_PROPERTY_CREATE_D(qreal, Minimum)
  Q_PROPERTY_CREATE_D(qreal, Maximum)
  Q_PROPERTY_CREATE_D(qreal, Value)
  Q_PROPERTY_CREATE_D(int, MajorTickCount)
  Q_PROPERTY_CREATE_D(int, MinorTickCount)
  Q_PROPERTY_CREATE_D(int, StartAngle)
  Q_PROPERTY_CREATE_D(int, SpanAngle)
  Q_PROPERTY_CREATE_D(int, ArcWidth)
  Q_PROPERTY_CREATE_D(int, ValuePixelSize)
  Q_PROPERTY_CREATE_D(int, Decimals)
  Q_PROPERTY_CREATE_D(bool, IsAnimated)
  Q_PROPERTY(qreal AnimatedValue MEMBER m_AnimatedValue)

public:
  explicit NXDashboardGaugePrivate(QObject *parent = nullptr);
  ~NXDashboardGaugePrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
  qreal m_DangerPercent { 0.85 };
  qreal m_WarningPercent { 0.65 };
  qreal m_TickWarningPercent { 0.7 };
  qreal m_AnimatedValue { 0 };
};

#endif // NXDASHBOARDGAUGEPRIVATE_H
