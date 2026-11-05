#ifndef NXMESSAGEBUTTON_H
#define NXMESSAGEBUTTON_H

#include <QPushButton>
#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"
class NXMessageButtonPrivate;

class NX_EXPORT NXMessageButton : public QPushButton
{
  Q_OBJECT
  Q_Q_CREATE(NXMessageButton)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), BarTitle)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), BarText)
  Q_PROPERTY_CREATE_H(QWidget *, MessageTargetWidget)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, DisplayMsec)
  Q_PROPERTY_CREATE_H(NXMessageBarType::MessageMode, MessageMode)
  Q_PROPERTY_CREATE_H(NXMessageBarType::PositionPolicy, PositionPolicy)

public:
  explicit NXMessageButton(QWidget *parent = nullptr);
  explicit NXMessageButton(QString const &text, QWidget *parent = nullptr);
  ~NXMessageButton();

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXMESSAGEBUTTON_H
