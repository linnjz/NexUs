#ifndef NXPUSHBUTTONPRIVATE_H
#define NXPUSHBUTTONPRIVATE_H

#include <QColor>
#include <QObject>

#include "NXDef.h"
class NXPushButton;

class NXPushButtonPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXPushButton)
  Q_PROPERTY_CREATE_D(QColor, LightDefaultColor)
  Q_PROPERTY_CREATE_D(QColor, DarkDefaultColor)
  Q_PROPERTY_CREATE_D(QColor, LightHoverColor)
  Q_PROPERTY_CREATE_D(QColor, DarkHoverColor)
  Q_PROPERTY_CREATE_D(QColor, LightPressColor)
  Q_PROPERTY_CREATE_D(QColor, DarkPressColor)
  Q_PROPERTY_CREATE_D(QColor, LightTextColor)
  Q_PROPERTY_CREATE_D(QColor, DarkTextColor)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(bool, IsHoverEnable)

public:
  explicit NXPushButtonPrivate(QObject *parent = nullptr);
  ~NXPushButtonPrivate();

private:
  bool m_IsPressed { false };
  bool m_HasIcon { false };
  int m_IconSize { 18 };
  NXThemeType::ThemeMode m_ThemeMode;
  NXIconType::IconName m_Icon { NXIconType::None };
  NXTextType::TextStyle m_TextStyle { NXTextType::NoStyle };
};

#endif // NXPUSHBUTTONPRIVATE_H
