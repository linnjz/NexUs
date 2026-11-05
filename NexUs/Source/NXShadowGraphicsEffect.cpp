#include "NXShadowGraphicsEffect.h"

#include <QPainter>

#include "private/NXShadowGraphicsEffectPrivate.h"
#include "NXTheme.h"

Q_PROPERTY_CREATE_CPP(NXShadowGraphicsEffect, QPS_SCREF_PAEMIT(QColor), LightColor)
Q_PROPERTY_CREATE_CPP(NXShadowGraphicsEffect, QPS_SCREF_PAEMIT(QColor), DarkColor)
Q_PROPERTY_CREATE_CPP(NXShadowGraphicsEffect, QPS_PAEMIT(QPointF), LightOffset)
Q_PROPERTY_CREATE_CPP(NXShadowGraphicsEffect, QPS_PAEMIT(QPointF), DarkOffset)
Q_PROPERTY_CREATE_CPP(NXShadowGraphicsEffect, QPS_PAEMIT(qreal), Blur)
Q_PROPERTY_CREATE_CPP(NXShadowGraphicsEffect, QPS_PAEMIT(qreal), Spread)
Q_PROPERTY_CREATE_CPP(NXShadowGraphicsEffect, QPS_PAEMIT(NXShadowGraphicsEffectType::RotateMode), RotateMode)
Q_PROPERTY_CREATE_CPP(NXShadowGraphicsEffect, QPS_PAEMIT(NXShadowGraphicsEffectType::ProjectionMode), ProjectionMode)

NXShadowGraphicsEffect::NXShadowGraphicsEffect(QObject *parent /*= nullptr*/)
    : QGraphicsEffect { parent }
    , d_ptr(new NXShadowGraphicsEffectPrivate())
{
  Q_D(NXShadowGraphicsEffect);
  d->q_ptr            = this;
  d->m_Blur           = 0.0;
  d->m_Spread         = 0.0;
  d->m_LightColor     = NXThemeColor(d->m_ThemeMode, BasicBaseAlpha);
  d->m_DarkColor      = NXThemeColor(d->m_ThemeMode, BasicBaseAlpha);
  d->m_RotateMode     = NXShadowGraphicsEffectType::RotateMode::Rotate45;
  d->m_ProjectionMode = NXShadowGraphicsEffectType::ProjectionMode::Inset;
  d->m_LightOffset    = QPointF { 0.0, 0.0 };
  d->m_DarkOffset     = QPointF { 0.0, 0.0 };
}

NXShadowGraphicsEffect::~NXShadowGraphicsEffect()
{
}

QRectF NXShadowGraphicsEffect::boundingRectFor(QRectF const &rect) const
{
  Q_CONST_D(NXShadowGraphicsEffect);

  // 对于内阴影，不需要扩展边界
  if (d->m_ProjectionMode == NXShadowGraphicsEffectType::ProjectionMode::Inset) {
    return rect.united(rect.translated(0, 0));
  }

  // 对于外阴影，需要扩展边界以容纳阴影
  qreal boundary = d->m_Blur + d->m_Spread * M_SQRT1_2;

  // 无论是矩形还是圆形阴影，都需要在所有方向上扩展边界
  return rect.united(rect.translated(0, 0).adjusted(-boundary, -boundary, boundary, boundary));
}

void NXShadowGraphicsEffect::draw(QPainter *painter)
{
  Q_D(NXShadowGraphicsEffect);
  QPoint pos;
  QPixmap const pixmap = sourcePixmap(Qt::DeviceCoordinates, &pos, PadToEffectiveBoundingRect);
  if (pixmap.isNull())
    return;
  if (d->m_Blur <= 0.0)
    return;
  QTransform restoreTransform = painter->worldTransform();
  painter->setWorldTransform(QTransform());
  painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  painter->setPen(Qt::NoPen);

  if (d->m_ProjectionMode == NXShadowGraphicsEffectType::ProjectionMode::Inset) {
    d->_drawInsetShadow(painter, pixmap, pos);
  } else {
    d->_drawOutsetShadow(painter, pixmap, pos);
  }

  painter->setWorldTransform(restoreTransform);
}
