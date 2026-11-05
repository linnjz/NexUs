#ifndef NXSCROLLPAGEAREAPRIVATE_H
#define NXSCROLLPAGEAREAPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXScrollPageArea;

class NXScrollPageAreaPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXScrollPageArea)
  Q_PROPERTY_CREATE_D(int, BorderRadius)

public:
  explicit NXScrollPageAreaPrivate(QObject *parent = nullptr);
  ~NXScrollPageAreaPrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXSCROLLPAGEAREAPRIVATE_H
