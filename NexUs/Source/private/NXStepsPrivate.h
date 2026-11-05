#ifndef NXSTEPSPRIVATE_H
#define NXSTEPSPRIVATE_H

#include <QObject>
#include <QStringList>

#include "NXDef.h"

class NXSteps;

class NXStepsPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXSteps)
  Q_PROPERTY_CREATE_D(int, CurrentStep)
  Q_PROPERTY_CREATE_D(int, StepCount)

public:
  NXThemeType::ThemeMode m_ThemeMode;
  QStringList m_StepTitles;
  explicit NXStepsPrivate(QObject *parent = nullptr);
  ~NXStepsPrivate();
};

#endif // NXSTEPSPRIVATE_H
