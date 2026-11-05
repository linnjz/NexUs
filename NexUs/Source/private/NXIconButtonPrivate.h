#ifndef NXICONBUTTONPRIVATE_H
#define NXICONBUTTONPRIVATE_H

#include <QColor>
#include <QObject>
#include <QPixmap>

#include "NXDef.h"
class NXIconButton;

class NXIconButtonPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXIconButton)
  Q_PROPERTY_CREATE_D(QColor, LightHoverColor)
  Q_PROPERTY_CREATE_D(QColor, DarkHoverColor)
  Q_PROPERTY_CREATE_D(QColor, LightIconColor)
  Q_PROPERTY_CREATE_D(QColor, DarkIconColor)
  Q_PROPERTY_CREATE_D(QColor, LightHoverIconColor)
  Q_PROPERTY_CREATE_D(QColor, DarkHoverIconColor)
  Q_PROPERTY_CREATE_D(qreal, Opacity)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), HoverAlpha)
  Q_PROPERTY_CREATE_D(NXIconType::IconName, Awesome)
  Q_PROPERTY_CREATE_D(bool, IsSelected)

public:
  explicit NXIconButtonPrivate(QObject *parent = nullptr);
  ~NXIconButtonPrivate();

private:
  bool m_IsAlphaAnimationFinished { true };
  NXThemeType::ThemeMode m_ThemeMode;
  QPixmap m_IconPix;
};

#endif // NXICONBUTTONPRIVATE_H
