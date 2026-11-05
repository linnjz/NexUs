#ifndef NXSPINBOXPRIVATE_H
#define NXSPINBOXPRIVATE_H

#include <QObject>

#include "NXDef.h"

class NXMenu;
class NXSpinBox;
class NXSpinBoxStyle;

class NXSpinBoxPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXSpinBox)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ExpandMarkWidth)

public:
  explicit NXSpinBoxPrivate(QObject *parent = nullptr);
  ~NXSpinBoxPrivate();
  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  NXThemeType::ThemeMode m_ThemeMode;
  NXSpinBoxStyle *m_Style { nullptr };
  NXMenu *_createStandardContextMenu();
};

#endif // NXSPINBOXPRIVATE_H
