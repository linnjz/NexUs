#ifndef NXWATERMARK_H
#define NXWATERMARK_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"
class NXWatermarkPrivate;

class NX_EXPORT NXWatermark : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXWatermark)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Text)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QImage), Image)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), TextColor)
  Q_PROPERTY_CREATE_H(qreal, Opacity)
  Q_PROPERTY_CREATE_H(qreal, Rotation)
  Q_PROPERTY_CREATE_H(int, FontPixelSize)
  Q_PROPERTY_CREATE_H(int, GapX)
  Q_PROPERTY_CREATE_H(int, GapY)
  Q_PROPERTY_CREATE_H(int, ImageWidth)
  Q_PROPERTY_CREATE_H(int, ImageHeight)

public:
  explicit NXWatermark(QWidget *parent = nullptr);
  explicit NXWatermark(QString const &text, QWidget *parent = nullptr);
  ~NXWatermark();

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXWATERMARK_H
