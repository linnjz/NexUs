#ifndef NXTOGGLEBUTTONPRIVATE_H
#define NXTOGGLEBUTTONPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXToggleButton;

class NXToggleButtonPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXToggleButton)
  Q_PROPERTY_CREATE_D(QString, Text)
  Q_PROPERTY_CREATE_D(NXIconType::IconName, NXIcon, NXIconType::None)
  Q_PROPERTY_CREATE_D(int, BorderRadius, 3)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), ToggleAlpha)

public:
  explicit NXToggleButtonPrivate(QObject *parent = nullptr);
  ~NXToggleButtonPrivate();

private:
  bool m_IsAlphaAnimationFinished { true };
  bool m_IsToggled { false };
  bool m_IsPressed { false };
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXTOGGLEBUTTONPRIVATE_H
