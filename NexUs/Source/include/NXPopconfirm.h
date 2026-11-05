#ifndef NXPOPCONFIRM_H
#define NXPOPCONFIRM_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXPopconfirmPrivate;

class NX_EXPORT NXPopconfirm : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXPopconfirm)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Title)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Content)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), ConfirmButtonText)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), CancelButtonText)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(NXIconType::IconName, Icon)
  Q_PROPERTY_CREATE_H(bool, IsLightDismiss)

public:
  explicit NXPopconfirm(QWidget *parent = nullptr);
  ~NXPopconfirm();

  void showPopconfirm(QWidget *target);
  void closePopconfirm();
  Q_SIGNAL void confirmed();
  Q_SIGNAL void cancelled();
  Q_SIGNAL void closed();

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXPOPCONFIRM_H
