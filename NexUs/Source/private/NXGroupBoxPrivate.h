#ifndef NXGROUPBOXPRIVATE_H
#define NXGROUPBOXPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXGroupBox;

class NXGroupBoxPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXGroupBox)
  Q_PROPERTY_CREATE_D(int, BorderRadius)

public:
  explicit NXGroupBoxPrivate(QObject *parent = nullptr);
  ~NXGroupBoxPrivate();
  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXGROUPBOXPRIVATE_H
