#ifndef NX_SHADOW_GRAPHICSEFFECT_PRIVATE_H_
#define NX_SHADOW_GRAPHICSEFFECT_PRIVATE_H_
#include <QObject>
#include <QPointF>
#include <QColor>

#include "NXDef.h"
class QPainter;
class NXShadowGraphicsEffect;

class NXShadowGraphicsEffectPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXShadowGraphicsEffect)
  Q_PROPERTY_CREATE_D(QColor, LightColor)
  Q_PROPERTY_CREATE_D(QColor, DarkColor)
  Q_PROPERTY_CREATE_D(QPointF, LightOffset)
  Q_PROPERTY_CREATE_D(QPointF, DarkOffset)
  Q_PROPERTY_CREATE_D(qreal, Blur)
  Q_PROPERTY_CREATE_D(qreal, Spread)
  Q_PROPERTY_CREATE_D(NXShadowGraphicsEffectType::RotateMode, RotateMode)
  Q_PROPERTY_CREATE_D(NXShadowGraphicsEffectType::ProjectionMode, ProjectionMode)

public:
  explicit NXShadowGraphicsEffectPrivate(QObject *parent = nullptr);
  ~NXShadowGraphicsEffectPrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
  void _drawInsetShadow(QPainter *painter, QPixmap const &pixmap, QPoint const &pos);
  void _drawOutsetShadow(QPainter *painter, QPixmap const &pixmap, QPoint const &pos);
};
#endif // !NX_SHADOW_GRAPHICSEFFECT_PRIVATE_H_
