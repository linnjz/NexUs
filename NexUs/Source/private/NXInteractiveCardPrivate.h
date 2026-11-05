#ifndef NXINTERACTIVECARDPRIVATE_H
#define NXINTERACTIVECARDPRIVATE_H

#include <QImage>
#include <QObject>
#include <QPixmap>
#include <QSize>

#include "NXDef.h"
class NXInteractiveCard;

class NXInteractiveCardPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXInteractiveCard)
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
  explicit NXInteractiveCardPrivate(QObject *parent = nullptr);
  ~NXInteractiveCardPrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXINTERACTIVECARDPRIVATE_H
