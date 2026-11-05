#ifndef NXEXPONENTIALBLUR_H
#define NXEXPONENTIALBLUR_H

#include <QObject>

#include "LinnSingleton.h"
#include "NXProperty.h"

class NXExponentialBlurPrivate;

class NX_EXPORT NXExponentialBlur : public QObject
{
  Q_OBJECT
  Q_Q_CREATE(NXExponentialBlur)
  Q_SINGLETON_CREATE(QSS_UNIQUE(NXExponentialBlur))

private:
  explicit NXExponentialBlur(QObject *parent = nullptr);
  ~NXExponentialBlur();

public:
  static QPixmap doExponentialBlur(QImage const &img, quint16 const &blurRadius);
};

#endif // NXEXPONENTIALBLUR_H
