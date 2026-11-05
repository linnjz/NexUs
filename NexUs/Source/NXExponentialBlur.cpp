#include "NXExponentialBlur.h"

#include <QPainter>
#include <QPixmap>

#include "private/NXExponentialBlurPrivate.h"
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#  include <cmath>
#endif

NXExponentialBlur::NXExponentialBlur(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXExponentialBlurPrivate())
{
  Q_D(NXExponentialBlur);
  d->q_ptr = this;
}

NXExponentialBlur::~NXExponentialBlur()
{
}

QPixmap NXExponentialBlur::doExponentialBlur(QImage const &img, quint16 const &blurRadius)
{
  QImage shadowImage = img.convertToFormat(QImage::Format_ARGB32);
  NXExponentialBlur::getInstance()->d_ptr->_drawExponentialBlur(shadowImage, blurRadius);
  return QPixmap::fromImage(shadowImage);
}
