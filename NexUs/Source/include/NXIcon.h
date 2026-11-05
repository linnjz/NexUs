#ifndef NXICON_H
#define NXICON_H
#include <QIcon>

#include "LinnSingleton.h"
#include "NXDef.h"
#include "NXProperty.h"

class NX_EXPORT NXIcon
{
  Q_SINGLETON_CREATE(QSS_UNIQUE(NXIcon))

private:
  explicit NXIcon();
  ~NXIcon();

public:
  QIcon getNXIcon(NXIconType::IconName awesome);
  QIcon getNXIcon(NXIconType::IconName awesome, QColor const &iconColor);
  QIcon getNXIcon(NXIconType::IconName awesome, int pixelSize);
  QIcon getNXIcon(NXIconType::IconName awesome, int pixelSize, QColor const &iconColor);
  QIcon getNXIcon(NXIconType::IconName awesome, int pixelSize, int fixedWidth, int fixedHeight);
  QIcon
  getNXIcon(NXIconType::IconName awesome, int pixelSize, int fixedWidth, int fixedHeight, QColor const &iconColor);
};

#endif // NXICON_H
