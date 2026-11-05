#ifndef NXMASKWIDGET_H
#define NXMASKWIDGET_H

#include <QWidget>

#include "NXProperty.h"

class NXMaskWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), MaskAlpha)

public:
  explicit NXMaskWidget(QWidget *parent = nullptr);
  ~NXMaskWidget();
  void doMaskAnimation(int endValue);
  Q_SIGNAL void animationFinished();

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXMASKWIDGET_H
