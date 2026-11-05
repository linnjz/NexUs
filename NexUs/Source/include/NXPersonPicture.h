#ifndef NXPERSONPICTURE_H
#define NXPERSONPICTURE_H

#include <QPixmap>
#include <QWidget>

#include "NXProperty.h"
class NXPersonPicturePrivate;

class NX_EXPORT NXPersonPicture : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXPersonPicture)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), DisplayName)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QPixmap), Picture)
  Q_PROPERTY_CREATE_H(int, PictureSize)

public:
  explicit NXPersonPicture(QWidget *parent = nullptr);
  ~NXPersonPicture();
  QSize sizeHint() const override;

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXPERSONPICTURE_H
