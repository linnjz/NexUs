#ifndef NXQRCODE_H
#define NXQRCODE_H

#include <QWidget>

#include "NXProperty.h"

class NXQRCodePrivate;

class NX_EXPORT NXQRCode : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXQRCode)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), ForegroundColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), BackgroundColor)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, QuietZone)

public:
  enum ErrorCorrectionLevel
  {
    Low = 0,
    Medium,
    Quartile,
    High
  };
  Q_ENUM(ErrorCorrectionLevel)

  explicit NXQRCode(QWidget *parent = nullptr);
  explicit NXQRCode(QString const &text, QWidget *parent = nullptr);
  ~NXQRCode();

  void setText(QString const &text);
  QString getText() const;

  void setErrorCorrectionLevel(ErrorCorrectionLevel level);
  ErrorCorrectionLevel getErrorCorrectionLevel() const;

  QPixmap toPixmap(int size = 256) const;

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXQRCODE_H
