#ifndef NX_SHADOW_GRAPHICSEFFECT_H_
#define NX_SHADOW_GRAPHICSEFFECT_H_

#include <QGraphicsEffect>
#include "NXDef.h"
class NXShadowGraphicsEffectPrivate;

class NX_EXPORT NXShadowGraphicsEffect : public QGraphicsEffect
{
  Q_OBJECT
  Q_Q_CREATE(NXShadowGraphicsEffect)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), LightColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), DarkColor)
  Q_PROPERTY_CREATE_H(QPointF, LightOffset)
  Q_PROPERTY_CREATE_H(QPointF, DarkOffset)
  Q_PROPERTY_CREATE_H(qreal, Blur)
  Q_PROPERTY_CREATE_H(qreal, Spread)
  Q_PROPERTY_CREATE_H(NXShadowGraphicsEffectType::RotateMode, RotateMode)
  Q_PROPERTY_CREATE_H(NXShadowGraphicsEffectType::ProjectionMode, ProjectionMode)

public:
  explicit NXShadowGraphicsEffect(QObject *parent = nullptr);
  ~NXShadowGraphicsEffect();

protected:
  QRectF boundingRectFor(QRectF const &rect) const override;
  void draw(QPainter *painter) override;
};

#endif // !NX_SHADOW_GRAPHICSEFFECT_H_
