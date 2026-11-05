#ifndef NXFLYOUT_H
#define NXFLYOUT_H

#include <QWidget>

#include "NXProperty.h"
class NXFlyoutPrivate;

class NX_EXPORT NXFlyout : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXFlyout)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Title)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Content)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(bool, IsLightDismiss)

public:
  explicit NXFlyout(QWidget *parent = nullptr);
  ~NXFlyout();

  void setContentWidget(QWidget *widget);
  void showFlyout(QWidget *target);
  void closeFlyout();
  Q_SIGNAL void closed();

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXFLYOUT_H
