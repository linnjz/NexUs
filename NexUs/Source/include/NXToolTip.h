#ifndef NXTOOLTIP_H
#define NXTOOLTIP_H

#include <QWidget>

#include "NXProperty.h"
class NXToolTipPrivate;

class NX_EXPORT NXToolTip : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXToolTip)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), ToolTip)
  Q_PROPERTY_CREATE_H(QWidget *, CustomWidget)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, DisplayMsec)
  Q_PROPERTY_CREATE_H(int, ShowDelayMsec)
  Q_PROPERTY_CREATE_H(int, HideDelayMsec)
  Q_PROPERTY_CREATE_H(int, OffSetX)
  Q_PROPERTY_CREATE_H(int, OffSetY)
  Q_PROPERTY_CREATE_H(bool, IsMoveEnable)

public:
  explicit NXToolTip(QWidget *parent = nullptr);
  ~NXToolTip();

  void updatePos();

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXTOOLTIP_H
