#ifndef NXREMINDERCARDPRIVATE_H
#define NXREMINDERCARDPRIVATE_H

#include <QImage>
#include <QObject>
#include <QPixmap>
#include <QSize>

#include "NXDef.h"
class NXReminderCard;

class NXReminderCardPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXReminderCard)
  Q_PROPERTY_CREATE_D(QString, Title)
  Q_PROPERTY_CREATE_D(QString, SubTitle)
  Q_PROPERTY_CREATE_D(QPixmap, CardPixmap)
  Q_PROPERTY_CREATE_D(QSize, CardPixmapSize)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, TitlePixelSize)
  Q_PROPERTY_CREATE_D(int, SubTitlePixelSize)
  Q_PROPERTY_CREATE_D(int, TitleSpacing)
  Q_PROPERTY_CREATE_D(int, CardPixmapBorderRadius)
  Q_PROPERTY_CREATE_D(NXCardPixType::PixMode, CardPixMode)

public:
  explicit NXReminderCardPrivate(QObject *parent = nullptr);
  ~NXReminderCardPrivate();

private:
  int m_ShadowBorderWidth { 6 };
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXREMINDERCARDPRIVATE_H
