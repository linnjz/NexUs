#ifndef NXIMAGECARDPRIVATE_H
#define NXIMAGECARDPRIVATE_H

#include <QImage>
#include <QObject>

#include "NXDef.h"

class NXImageCard;

class NXImageCardPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXImageCard)
  Q_PROPERTY_CREATE_D(QImage, CardImage)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(bool, IsPreserveAspectCrop)

public:
  explicit NXImageCardPrivate(QObject *parent = nullptr);
  ~NXImageCardPrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXIMAGECARDPRIVATE_H
