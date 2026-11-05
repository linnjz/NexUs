#ifndef NXMESSAGEBUTTONPRIVATE_H
#define NXMESSAGEBUTTONPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXMessageButton;

class NXMessageButtonPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXMessageButton)
  Q_PROPERTY_CREATE_D(QString, BarTitle)
  Q_PROPERTY_CREATE_D(QString, BarText)
  Q_PROPERTY_CREATE_D(QWidget *, MessageTargetWidget)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, DisplayMsec)
  Q_PROPERTY_CREATE_D(NXMessageBarType::MessageMode, MessageMode)
  Q_PROPERTY_CREATE_D(NXMessageBarType::PositionPolicy, PositionPolicy)

public:
  explicit NXMessageButtonPrivate(QObject *parent = nullptr);
  ~NXMessageButtonPrivate();

private:
  bool m_IsLeftButtonPress { false };
  int m_ShadowBorderWidth { 3 };
  int m_PenBorderWidth { 1 };
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXMESSAGEBUTTONPRIVATE_H
