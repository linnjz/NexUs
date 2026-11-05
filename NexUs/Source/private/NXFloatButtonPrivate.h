#ifndef NXFLOATBUTTONPRIVATE_H
#define NXFLOATBUTTONPRIVATE_H

#include <QObject>

#include "NXDef.h"

class NXFloatButton;
class NXMenu;

class NXFloatButtonPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXFloatButton)
  Q_PROPERTY_CREATE_D(int, ButtonSize)
  Q_PROPERTY_CREATE_D(int, Margin)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), HoverAlpha)

public:
  explicit NXFloatButtonPrivate(QObject *parent = nullptr);
  ~NXFloatButtonPrivate();

  void updatePosition();

private:
  bool m_IsPressed { false };
  bool m_IsAlphaAnimationFinished { true };
  NXIconType::IconName m_Icon { NXIconType::Plus };
  int m_Position { 0 };
  NXThemeType::ThemeMode m_ThemeMode;
  NXMenu *m_Menu { nullptr };
};

#endif // NXFLOATBUTTONPRIVATE_H
