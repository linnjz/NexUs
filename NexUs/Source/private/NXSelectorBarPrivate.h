#ifndef NXSELECTORBARPRIVATE_H
#define NXSELECTORBARPRIVATE_H

#include <QObject>

#include "NXDef.h"
#include "NXProperty.h"

class QVariantAnimation;
class NXSelectorBar;

class NXSelectorBarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXSelectorBar)
  Q_PROPERTY_CREATE_D(int, CurrentIndex)
  Q_PROPERTY_CREATE_D(int, BorderRadius)

public:
  struct SelectorItem
  {
    NXIconType::IconName icon = NXIconType::None;
    QString text;
  };

  explicit NXSelectorBarPrivate(QObject *parent = nullptr);
  ~NXSelectorBarPrivate();


private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_HoveredIndex { -1 };
  qreal m_IndicatorX { 0 };
  qreal m_IndicatorWidth { 0 };
  QList<SelectorItem> m_Items;
  QVariantAnimation *m_IndicatorAnimation { nullptr };
  void _startIndicatorAnimation(qreal targetX, qreal targetWidth);
  int _itemIndexAtPos(QPoint pos) const;
};

#endif // NXSELECTORBARPRIVATE_H
