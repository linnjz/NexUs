#ifndef NXRIBBONTABBARPRIVATE_H
#define NXRIBBONTABBARPRIVATE_H

#include <QObject>
#include <QRect>
#include <QStringList>

#include "NXDef.h"
#include "NXProperty.h"

class NXRibbonTabBar;

class NXRibbonTabBarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXRibbonTabBar)
  Q_PROPERTY_CREATE_D(int, CurrentIndex)

public:
  explicit NXRibbonTabBarPrivate(QObject *parent = nullptr);
  ~NXRibbonTabBarPrivate();

  QList<QRect> tabRects() const;
  int tabAt(QPoint pos) const;

private:
  int m_HoveredIndex { -1 };
  NXThemeType::ThemeMode m_ThemeMode;
  QStringList m_TabTitles;
  QList<bool> m_TabEnableds;
};

#endif // NXRIBBONTABBARPRIVATE_H
