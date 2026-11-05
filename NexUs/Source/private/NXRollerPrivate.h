#ifndef NXFRAMEWORK_NEXUS_PRIVATE_NXROLLERPRIVATE_H_
#define NXFRAMEWORK_NEXUS_PRIVATE_NXROLLERPRIVATE_H_

#include <QObject>
#include <QPoint>
#include <QRect>
#include <QTimer>

#include "NXDef.h"
class NXRoller;
class QPropertyAnimation;

class NXRollerPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXRoller)
  Q_PROPERTY_CREATE_D(QStringList, ItemList)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ScrollOffset)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, ItemHeight)
  Q_PROPERTY_CREATE_D(int, MaxVisibleItems)
  Q_PROPERTY_CREATE_D(int, CurrentIndex)
  Q_PROPERTY_CREATE_D(bool, IsContainer)
  Q_PROPERTY_CREATE_D(bool, IsEnableLoop)

public:
  explicit NXRollerPrivate(QObject *parent = nullptr);
  ~NXRollerPrivate();

private:
  bool m_IsUpArrowPress { false };
  bool m_IsDownArrowPress { false };
  NXThemeType::ThemeMode m_ThemeMode;
  int m_PenBorderWidth { 1 };
  int m_IndicatorMargin { 3 };
  qreal m_TargetScrollOffset { 0 };
  QPoint m_MousePoint;
  QRect m_UpArrowRect;
  QRect m_DownArrowRect;
  QPropertyAnimation *m_ScrollAnimation { nullptr };
  QTimer *m_PressSustainTimer { nullptr };
  QTimer *m_RepeatScrollTimer { nullptr };
  void _scroll(int delta);
};

#endif //NXFRAMEWORK_NEXUS_PRIVATE_NXROLLERPRIVATE_H_
