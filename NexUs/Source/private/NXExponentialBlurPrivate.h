#ifndef NXEXPONENTIALBLURPRIVATE_H
#define NXEXPONENTIALBLURPRIVATE_H

#include <QObject>

#include "NXProperty.h"

class NXExponentialBlur;

class NXExponentialBlurPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXExponentialBlur)

public:
  explicit NXExponentialBlurPrivate(QObject *parent = nullptr);
  ~NXExponentialBlurPrivate();

private:
  static int m_Aprec;
  static int m_Zprec;
  static void _drawExponentialBlur(QImage &image, quint16 const &qRadius);
  static void _drawRowBlur(QImage &image, int row, int alpha);
  static void _drawColumnBlur(QImage &image, int column, int alpha);
  static void _drawInnerBlur(unsigned char *bptr, int &zR, int &zG, int &zB, int &zA, int alpha);
};

#endif // NXEXPONENTIALBLURPRIVATE_H
