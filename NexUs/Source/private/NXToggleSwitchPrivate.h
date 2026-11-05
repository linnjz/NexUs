#ifndef NXTOGGLESWITCHPRIVATE_H
#define NXTOGGLESWITCHPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXToggleSwitch;

class NXToggleSwitchPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXToggleSwitch);

public:
  explicit NXToggleSwitchPrivate(QObject *parent = nullptr);
  ~NXToggleSwitchPrivate();

private:
  bool m_IsToggled { false };
  int m_Margin { 1 };
  qreal m_CircleCenterX { 0 };
  qreal m_CircleRadius { 0 };
  bool m_IsLeftButtonPress { false };
  bool m_IsMousePressMove { false };
  int m_LastMouseX { 0 };
  int m_PressStartX { 0 };
  NXThemeType::ThemeMode m_ThemeMode;
  void _startPosAnimation(qreal startX, qreal endX, bool isToggle);
  void _startRadiusAnimation(qreal startRadius, qreal endRadius);
  void _adjustCircleCenterX();
};

#endif // NXTOGGLESWITCHPRIVATE_H
