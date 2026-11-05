#ifndef NXTEXTPRIVATE_H
#define NXTEXTPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXText;

class NXTextPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXText)
  Q_PROPERTY_CREATE_D(NXTextType::TextStyle, TextStyle)
  Q_PROPERTY_CREATE_D(NXIconType::IconName, NXIcon)
  Q_PROPERTY_CREATE_D(bool, IsClickEnable)
  Q_PROPERTY_CREATE_D(bool, IsWrapAnywhere)

public:
  explicit NXTextPrivate(QObject *parent = nullptr);
  ~NXTextPrivate();
  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXTEXTPRIVATE_H
