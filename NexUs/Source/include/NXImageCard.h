#ifndef NXIMAGECARD_H
#define NXIMAGECARD_H

#include <QWidget>

#include "NXProperty.h"

class NXImageCardPrivate;

class NX_EXPORT NXImageCard : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXImageCard)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QImage), CardImage)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(bool, IsPreserveAspectCrop)

public:
  explicit NXImageCard(QWidget *parent = nullptr);
  ~NXImageCard();

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXIMAGECARD_H
