#include "NXShadowGraphicsEffectPrivate.h"

#include <QPainter>
#include <QWidget>

QT_BEGIN_NAMESPACE
extern Q_WIDGETS_EXPORT void
qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
extern Q_WIDGETS_EXPORT void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed = 0);
QT_END_NAMESPACE

NXShadowGraphicsEffectPrivate::NXShadowGraphicsEffectPrivate(QObject *parent)
    : QObject { parent }
{
}

NXShadowGraphicsEffectPrivate::~NXShadowGraphicsEffectPrivate()
{
}

void NXShadowGraphicsEffectPrivate::_drawInsetShadow(QPainter *painter, QPixmap const &pixmap, QPoint const &pos)
{
  QSize const pixmapSize = pixmap.size();
  qreal const pixelRatio = pixmap.devicePixelRatioF();
  qreal const radian     = m_Spread * M_SQRT1_2;
  QRectF clearRect(QPointF(0, 0), pixmapSize / pixelRatio);
  clearRect.adjust(radian, radian, -radian, -radian);
  QPointF topLeftOffset = clearRect.topLeft(), bottomRightOffset = clearRect.bottomRight();
  QPointF maskStartPos = topLeftOffset;
  switch (m_RotateMode) {
  case NXShadowGraphicsEffectType::Rotate45 :
    maskStartPos += m_LightOffset;
    topLeftOffset += m_LightOffset;
    bottomRightOffset += m_DarkOffset;
    break;
  case NXShadowGraphicsEffectType::Rotate135 :
    maskStartPos += { -m_LightOffset.y(), m_LightOffset.x() };
    topLeftOffset += { -m_DarkOffset.y(), m_LightOffset.x() };
    bottomRightOffset += { -m_LightOffset.y(), m_DarkOffset.x() };
    break;
  case NXShadowGraphicsEffectType::Rotate225 :
    maskStartPos += { -m_LightOffset.x(), -m_LightOffset.y() };
    topLeftOffset += { -m_DarkOffset.x(), -m_DarkOffset.y() };
    bottomRightOffset += { -m_LightOffset.x(), -m_LightOffset.y() };
    break;
  case NXShadowGraphicsEffectType::Rotate315 :
    maskStartPos += { m_LightOffset.y(), m_LightOffset.x() };
    topLeftOffset += { m_LightOffset.y(), -m_DarkOffset.x() };
    bottomRightOffset += { m_DarkOffset.y(), -m_LightOffset.x() };
    break;
  default : break;
  }

  QImage resultImage(pixmap.size(), QImage::Format_ARGB32_Premultiplied);
  resultImage.setDevicePixelRatio(pixmap.devicePixelRatioF());
  resultImage.fill(m_LightColor);
  QImage maskImage(pixmap.size(), QImage::Format_ARGB32_Premultiplied);
  maskImage.setDevicePixelRatio(pixmap.devicePixelRatioF());
  maskImage.fill(m_DarkColor);

  QPainter innerPainter(&resultImage);
  // 组合两种阴影
  innerPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
  innerPainter.drawImage(maskStartPos, maskImage);

  // 重叠区域清除
  innerPainter.setCompositionMode(QPainter::CompositionMode_Clear);
  innerPainter.fillRect(QRectF { topLeftOffset, bottomRightOffset }, Qt::transparent);

  // 应用模糊效果
  qt_blurImage(resultImage, m_Blur, true);

  innerPainter.end();

  painter->drawImage(pos, resultImage);
}

void NXShadowGraphicsEffectPrivate::_drawOutsetShadow(QPainter *painter, QPixmap const &pixmap, QPoint const &pos)
{
  QImage pixmapImage = pixmap.toImage();
  QPointF lightOffset, darkOffset;
  switch (m_RotateMode) {
  case NXShadowGraphicsEffectType::Rotate45 :
    lightOffset = m_LightOffset;
    darkOffset  = m_DarkOffset;
    break;
  case NXShadowGraphicsEffectType::Rotate135 :
    lightOffset = { -m_LightOffset.y(), m_LightOffset.x() };
    darkOffset  = { -m_DarkOffset.y(), m_DarkOffset.x() };
    break;
  case NXShadowGraphicsEffectType::Rotate225 :
    lightOffset = { -m_LightOffset.x(), -m_LightOffset.y() };
    darkOffset  = { -m_DarkOffset.x(), -m_DarkOffset.y() };
    break;
  case NXShadowGraphicsEffectType::Rotate315 :
    lightOffset = { m_LightOffset.y(), -m_LightOffset.x() };
    darkOffset  = { m_DarkOffset.y(), -m_DarkOffset.x() };
    break;
  default : break;
  }

  QImage blurImage(pixmapImage.size(), QImage::Format_ARGB32_Premultiplied);
  blurImage.setDevicePixelRatio(pixmap.devicePixelRatio());
  blurImage.fill(0);

  QPainter blurPainter(&blurImage);
  qt_blurImage(&blurPainter, pixmapImage, m_Blur, true, true);
  blurPainter.end();

  auto applyColorFunc = [](QImage source, QColor const &color) -> QImage {
    QPainter painter(&source);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(source.rect(), color);
    painter.end();
    return source;
  };

  QImage lightShadow = applyColorFunc(blurImage, m_LightColor);
  QImage darkShadow  = applyColorFunc(std::move(blurImage), m_DarkColor);
  painter->drawImage(pos + lightOffset, lightShadow);
  painter->drawImage(pos + darkOffset, darkShadow);

  painter->drawPixmap(pos, pixmap);
}
