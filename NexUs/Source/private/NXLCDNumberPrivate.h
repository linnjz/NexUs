#ifndef NEXUS_NXLCDNUMBERPRIVATE_H
#define NEXUS_NXLCDNUMBERPRIVATE_H

#include <QObject>

#include "NXDef.h"
class QTimer;
class NXLCDNumber;
class NXLCDNumberStyle;

class NXLCDNumberPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXLCDNumber)
  Q_PROPERTY_CREATE_D(QString, AutoClockFormat)
  Q_PROPERTY_CREATE_D(bool, IsUseAutoClock)

public:
  explicit NXLCDNumberPrivate(QObject *parent = nullptr);
  ~NXLCDNumberPrivate();

  Q_SLOT void onThemeModeChanged(NXThemeType::ThemeMode themeMode);

private:
  NXThemeType::ThemeMode m_ThemeMode;
  QTimer *m_ClockTimer { nullptr };
  NXLCDNumberStyle *m_LcdNumberStyle { nullptr };
};

#endif //NEXUS_NXLCDNUMBERPRIVATE_H
