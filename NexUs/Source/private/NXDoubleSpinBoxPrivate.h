#ifndef NXDOUBLESPINBOXPRIVATE_H
#define NXDOUBLESPINBOXPRIVATE_H

#include <QObject>

#include "NXDef.h"

class NXMenu;
class NXDoubleSpinBox;
class NXSpinBoxStyle;

class NXDoubleSpinBoxPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXDoubleSpinBox)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ExpandMarkWidth)

public:
  explicit NXDoubleSpinBoxPrivate(QObject *parent = nullptr);
  ~NXDoubleSpinBoxPrivate();
  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  NXThemeType::ThemeMode m_ThemeMode;
  NXSpinBoxStyle *m_Style { nullptr };
  NXMenu *_createStandardContextMenu();
  void _changeTheme();
};

#endif // NXDOUBLESPINBOXPRIVATE_H
