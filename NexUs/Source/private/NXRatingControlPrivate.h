#ifndef NXRATINGCONTROLPRIVATE_H
#define NXRATINGCONTROLPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXRatingControl;

class NXRatingControlPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXRatingControl)
  Q_PROPERTY_CREATE_D(int, MaxRating)
  Q_PROPERTY_CREATE_D(int, Rating)
  Q_PROPERTY_CREATE_D(int, StarSize)
  Q_PROPERTY_CREATE_D(int, Spacing)
  Q_PROPERTY_CREATE_D(bool, IsReadOnly)

public:
  explicit NXRatingControlPrivate(QObject *parent = nullptr);
  ~NXRatingControlPrivate();

private:
  int m_HoveredStar { -1 };
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXRATINGCONTROLPRIVATE_H
