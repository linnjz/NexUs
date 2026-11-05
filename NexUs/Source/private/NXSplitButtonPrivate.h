#ifndef NXSPLITBUTTONPRIVATE_H
#define NXSPLITBUTTONPRIVATE_H

#include <QObject>

#include "NXDef.h"
#include "NXProperty.h"
class NXMenu;
class NXSplitButton;

class NXSplitButtonPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXSplitButton)
  Q_PROPERTY_CREATE_D(QString, Text)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(NXIconType::IconName, NXIcon)

public:
  explicit NXSplitButtonPrivate(QObject *parent = nullptr);
  ~NXSplitButtonPrivate();

private:
  bool m_IsLeftHovered { false };
  bool m_IsRightHovered { false };
  bool m_IsLeftPressed { false };
  bool m_IsRightPressed { false };
  NXThemeType::ThemeMode m_ThemeMode;
  NXMenu *m_Menu { nullptr };
  static int const m_DropdownWidth = 30;
};

#endif // NXSPLITBUTTONPRIVATE_H
