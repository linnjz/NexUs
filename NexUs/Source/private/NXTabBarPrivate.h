#ifndef NXTABBARPRIVATE_H
#define NXTABBARPRIVATE_H

#include <QMimeData>
#include <QObject>
#include <QPixmap>

#include "NXProperty.h"
class NXTabBar;
class NXTabBarStyle;
class QTabBarPrivate;
class QPropertyAnimation;

class NXTabBarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXTabBar)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ScrollOffset)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), TargetScrollOffset)

public:
  explicit NXTabBarPrivate(QObject *parent = nullptr);
  ~NXTabBarPrivate();

  void startScrollAnimation();
  void restoreScrollOffset(qreal offset);

private:
  QMimeData *m_MimeData { nullptr };
  NXTabBarStyle *m_Style { nullptr };
  QTabBarPrivate *m_TabBarPrivate { nullptr };
  QPropertyAnimation *m_ScrollAnimation { nullptr };
};

#endif // NXTABBARPRIVATE_H
