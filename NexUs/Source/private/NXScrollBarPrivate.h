#ifndef NXSCROLLBARPRIVATE_H
#define NXSCROLLBARPRIVATE_H

#include <QAbstractScrollArea>
#include <QObject>
#include <QScrollBar>

#include "NXProperty.h"
class QTimer;
class QPropertyAnimation;
class NXScrollBar;

class NXScrollBarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXScrollBar)
  Q_PROPERTY_CREATE_D(qreal, SpeedLimit)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), TargetMaximum)
  Q_PROPERTY_CREATE_D(bool, IsAnimation)

public:
  explicit NXScrollBarPrivate(QObject *parent = nullptr);
  ~NXScrollBarPrivate();
  Q_SLOT void onRangeChanged(int min, int max);

private:
  bool m_IsExpand { false };
  int m_ScrollValue { -1 };
  QScrollBar *m_OriginScrollBar { nullptr };
  QAbstractScrollArea *m_OriginScrollArea { nullptr };
  QTimer *m_ExpandTimer { nullptr };
  QPropertyAnimation *m_SlideSmoothAnimation { nullptr };
  void _scroll(Qt::KeyboardModifiers modifiers, int value);
  int _pixelPosToRangeValue(int pos) const;

  // 映射处理函数
  void _initAllConfig();
  void _handleScrollBarValueChanged(QScrollBar *scrollBar, int value);
  void _handleScrollBarRangeChanged(int min, int max);
  void _handleScrollBarGeometry();
};

#endif // NXSCROLLBARPRIVATE_H
