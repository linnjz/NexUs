#ifndef NXPROMOTIONVIEWPRIVATE_H
#define NXPROMOTIONVIEWPRIVATE_H

#include <QObject>
#include <QPixmap>

#include "NXDef.h"
class NXPromotionCard;
class NXPromotionView;

class NXPromotionViewPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXPromotionView)
  Q_PROPERTY_CREATE_D(int, CardExpandWidth)
  Q_PROPERTY_CREATE_D(int, CardCollapseWidth)
  Q_PROPERTY_CREATE_D(int, CurrentIndex)
  Q_PROPERTY_CREATE_D(int, AutoScrollInterval)
  Q_PROPERTY_CREATE_D(bool, IsAutoScroll)

public:
  explicit NXPromotionViewPrivate(QObject *parent = nullptr);
  ~NXPromotionViewPrivate();
  void onPromotionCardClicked(NXPromotionCard *clickedCard);

private:
  bool m_IsAllowSwitch { true };
  NXThemeType::ThemeMode m_ThemeMode;
  int m_LeftPadding { 40 };
  int m_CardSpacing { 5 };
  int m_BottomMargin { 30 };
  int m_IndicatorSpacing { 12 };
  QList<NXPromotionCard *> m_PromotionCardList;
  QTimer *m_AutoScrollTimer { nullptr };
  void _startCardGeometryAnimation(NXPromotionCard *card, QRect start, QRect end);
  void _startHorizontalCardPixmapRatioAnimation(NXPromotionCard *card, qreal start, qreal end);
  void _updatePromotionCardGeometry();
  int _getAdjacentIndex(Qt::LayoutDirection direction, int index);
  int _getRightLimitX();
};

#endif // NXPROMOTIONVIEWPRIVATE_H
