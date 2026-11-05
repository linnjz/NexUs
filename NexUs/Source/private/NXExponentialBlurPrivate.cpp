#include "NXExponentialBlurPrivate.h"

#include <QPixmap>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#  include <cmath>
#endif
int NXExponentialBlurPrivate::m_Aprec = 12;
int NXExponentialBlurPrivate::m_Zprec = 7;

NXExponentialBlurPrivate::NXExponentialBlurPrivate(QObject *parent)
    : QObject { parent }
{
}

NXExponentialBlurPrivate::~NXExponentialBlurPrivate()
{
}

void NXExponentialBlurPrivate::_drawExponentialBlur(QImage &image, quint16 const &qRadius)
{
  if (qRadius < 1) {
    return;
  }
  image      = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
  int alpha  = (int) ((1 << m_Aprec) * (1.0f - std::exp(-2.3f / (qRadius + 1.f))));
  int height = image.height();
  int width  = image.width();
  for (int row = 0; row < height; row++) {
    _drawRowBlur(image, row, alpha);
  }

  for (int col = 0; col < width; col++) {
    _drawColumnBlur(image, col, alpha);
  }
}

void NXExponentialBlurPrivate::_drawRowBlur(QImage &image, int row, int alpha)
{
  int zR, zG, zB, zA;

  QRgb *ptr = (QRgb *) image.scanLine(row);
  int width = image.width();

  zR = *((unsigned char *) ptr) << m_Zprec;
  zG = *((unsigned char *) ptr + 1) << m_Zprec;
  zB = *((unsigned char *) ptr + 2) << m_Zprec;
  zA = *((unsigned char *) ptr + 3) << m_Zprec;

  for (int index = 0; index < width; index++) {
    _drawInnerBlur((unsigned char *) &ptr[index], zR, zG, zB, zA, alpha);
  }
  for (int index = width - 2; index >= 0; index--) {
    _drawInnerBlur((unsigned char *) &ptr[index], zR, zG, zB, zA, alpha);
  }
}

void NXExponentialBlurPrivate::_drawColumnBlur(QImage &image, int column, int alpha)
{
  int zR, zG, zB, zA;

  QRgb *ptr = (QRgb *) image.bits();
  ptr += column;
  int height = image.height();
  int width  = image.width();

  zR = *((unsigned char *) ptr) << m_Zprec;
  zG = *((unsigned char *) ptr + 1) << m_Zprec;
  zB = *((unsigned char *) ptr + 2) << m_Zprec;
  zA = *((unsigned char *) ptr + 3) << m_Zprec;

  for (int index = width; index < (height - 1) * width; index += width) {
    _drawInnerBlur((unsigned char *) &ptr[index], zR, zG, zB, zA, alpha);
  }
  for (int index = (height - 2) * width; index >= 0; index -= width) {
    _drawInnerBlur((unsigned char *) &ptr[index], zR, zG, zB, zA, alpha);
  }
}

void NXExponentialBlurPrivate::_drawInnerBlur(unsigned char *bptr, int &zR, int &zG, int &zB, int &zA, int alpha)
{
  int R, G, B, A;
  R = *bptr;
  G = *(bptr + 1);
  B = *(bptr + 2);
  A = *(bptr + 3);

  zR += (alpha * ((R << m_Zprec) - zR)) >> m_Aprec;
  zG += (alpha * ((G << m_Zprec) - zG)) >> m_Aprec;
  zB += (alpha * ((B << m_Zprec) - zB)) >> m_Aprec;
  zA += (alpha * ((A << m_Zprec) - zA)) >> m_Aprec;

  *bptr       = zR >> m_Zprec;
  *(bptr + 1) = zG >> m_Zprec;
  *(bptr + 2) = zB >> m_Zprec;
  *(bptr + 3) = zA >> m_Zprec;
}
