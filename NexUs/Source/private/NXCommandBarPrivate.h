#ifndef NXCOMMANDBARPRIVATE_H
#define NXCOMMANDBARPRIVATE_H

#include <QObject>

#include "NXCommandBar.h"
#include "NXProperty.h"

class NXMenu;

class NXCommandBarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXCommandBar)
  Q_PROPERTY_CREATE_D(int, ButtonSize)

public:
  explicit NXCommandBarPrivate(QObject *parent = nullptr);
  ~NXCommandBarPrivate();

  QList<QRect> _getVisibleItemRects() const;
  int _getItemWidth(NXCommandBar::CommandItem const &item) const;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_HoveredIndex { -1 };
  int m_PressedIndex { -1 };
  int m_VisibleCount { 0 };
  QList<NXCommandBar::CommandItem> m_Items;
  NXMenu *m_OverflowMenu { nullptr };
};

#endif // NXCOMMANDBARPRIVATE_H
