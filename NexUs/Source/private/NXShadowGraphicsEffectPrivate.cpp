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

void NXShadowGraphicsEffectPrivate::_drawInsetShadow(QPainter *painter, const QPixmap &pixmap, const QPoint &pos)
{
  const QSize pixmapSize = pixmap.size();
  const qreal pixelRatio = pixmap.devicePixelRatioF();
  const qreal radian     = _pSpread * M_SQRT1_2;
  QRectF clearRect(QPointF(0, 0), pixmapSize / pixelRatio);
  clearRect.adjust(radian, radian, -radian, -radian);
  QPointF topLeftOffset = clearRect.topLeft(), bottomRightOffset = clearRect.bottomRight();
  QPointF maskStartPos = topLeftOffset;
  switch (_pRotateMode) {
  case NXShadowGraphicsEffectType::Rotate45 :
    maskStartPos += _pLightOffset;
    topLeftOffset += _pLightOffset;
    bottomRightOffset += _pDarkOffset;
    break;
  case NXShadowGraphicsEffectType::Rotate135 :
    maskStartPos += { -_pLightOffset.y(), _pLightOffset.x() };
    topLeftOffset += { -_pDarkOffset.y(), _pLightOffset.x() };
    bottomRightOffset += { -_pLightOffset.y(), _pDarkOffset.x() };
    break;
  case NXShadowGraphicsEffectType::Rotate225 :
    maskStartPos += { -_pLightOffset.x(), -_pLightOffset.y() };
    topLeftOffset += { -_pDarkOffset.x(), -_pDarkOffset.y() };
    bottomRightOffset += { -_pLightOffset.x(), -_pLightOffset.y() };
    break;
  case NXShadowGraphicsEffectType::Rotate315 :
    maskStartPos += { _pLightOffset.y(), _pLightOffset.x() };
    topLeftOffset += { _pLightOffset.y(), -_pDarkOffset.x() };
    bottomRightOffset += { _pDarkOffset.y(), -_pLightOffset.x() };
    break;
  default : break;
  }

  QImage resultImage(pixmap.size(), QImage::Format_ARGB32_Premultiplied);
  resultImage.setDevicePixelRatio(pixmap.devicePixelRatioF());
  resultImage.fill(_pLightColor);
  QImage maskImage(pixmap.size(), QImage::Format_ARGB32_Premultiplied);
  maskImage.setDevicePixelRatio(pixmap.devicePixelRatioF());
  maskImage.fill(_pDarkColor);

  QPainter innerPainter(&resultImage);
  // 组合两种阴影
  innerPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
  innerPainter.drawImage(maskStartPos, maskImage);

  // 重叠区域清除
  innerPainter.setCompositionMode(QPainter::CompositionMode_Clear);
  innerPainter.fillRect(QRectF { topLeftOffset, bottomRightOffset }, Qt::transparent);

  // 应用模糊效果
  qt_blurImage(resultImage, _pBlur, true);

  innerPainter.end();

  painter->drawImage(pos, resultImage);
}

void NXShadowGraphicsEffectPrivate::_drawOutsetShadow(QPainter *painter, const QPixmap &pixmap, const QPoint &pos)
{
  QImage pixmapImage = pixmap.toImage();
  QPointF lightOffset, darkOffset;
  switch (_pRotateMode) {
  case NXShadowGraphicsEffectType::Rotate45 :
    lightOffset = _pLightOffset;
    darkOffset  = _pDarkOffset;
    break;
  case NXShadowGraphicsEffectType::Rotate135 :
    lightOffset = { -_pLightOffset.y(), _pLightOffset.x() };
    darkOffset  = { -_pDarkOffset.y(), _pDarkOffset.x() };
    break;
  case NXShadowGraphicsEffectType::Rotate225 :
    lightOffset = { -_pLightOffset.x(), -_pLightOffset.y() };
    darkOffset  = { -_pDarkOffset.x(), -_pDarkOffset.y() };
    break;
  case NXShadowGraphicsEffectType::Rotate315 :
    lightOffset = { _pLightOffset.y(), -_pLightOffset.x() };
    darkOffset  = { _pDarkOffset.y(), -_pDarkOffset.x() };
    break;
  default : break;
  }

  QImage blurImage(pixmapImage.size(), QImage::Format_ARGB32_Premultiplied);
  blurImage.setDevicePixelRatio(pixmap.devicePixelRatio());
  blurImage.fill(0);

  QPainter blurPainter(&blurImage);
  qt_blurImage(&blurPainter, pixmapImage, _pBlur, true, true);
  blurPainter.end();

  auto applyColorFunc = [](QImage source, const QColor &color) -> QImage {
    QPainter painter(&source);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(source.rect(), color);
    painter.end();
    return source;
  };

  QImage lightShadow = applyColorFunc(blurImage, _pLightColor);
  QImage darkShadow  = applyColorFunc(std::move(blurImage), _pDarkColor);
  painter->drawImage(pos + lightOffset, lightShadow);
  painter->drawImage(pos + darkOffset, darkShadow);

  painter->drawPixmap(pos, pixmap);
}
