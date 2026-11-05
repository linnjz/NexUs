#ifndef NXPOPULARCARDFLOATER_H
#define NXPOPULARCARDFLOATER_H

#include <QWidget>

#include "NXDef.h"
class NXPushButton;
class NXPopularCard;
class NXPopularCardPrivate;
class QGraphicsOpacityEffect;

class NXPopularCardFloater : public QWidget
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), HoverYOffset)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), HoverOpacity)

public:
  explicit NXPopularCardFloater(NXPopularCard *card, NXPopularCardPrivate *cardPrivate, QWidget *parent = nullptr);
  ~NXPopularCardFloater();
  void showFloater();
  void hideFloater();

protected:
  bool event(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

private:
  friend class NXPopularCard;
  bool m_IsHideAnimationFinished { true };
  NXThemeType::ThemeMode m_ThemeMode;
  int m_FloatGeometryOffset { 25 };
  NXPushButton *m_OverButton { nullptr };
  QGraphicsOpacityEffect *m_OpacityEffect { nullptr };
  NXPopularCard *m_Card { nullptr };
  NXPopularCardPrivate *m_CardPrivate { nullptr };
  QRect _calculateTargetGeometry(QRect cardGeometry);
};

#endif // NXPOPULARCARDFLOATER_H
