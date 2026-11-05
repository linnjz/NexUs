#ifndef NXPUSHBUTTON_H
#define NXPUSHBUTTON_H

#include <QPushButton>

#include "NXDef.h"
#include "NXProperty.h"
class NXPushButtonPrivate;

class NX_EXPORT NXPushButton : public QPushButton
{
  Q_OBJECT
  Q_Q_CREATE(NXPushButton)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), LightDefaultColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), DarkDefaultColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), LightHoverColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), DarkHoverColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), LightPressColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), DarkPressColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), LightTextColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), DarkTextColor)
  Q_PROPERTY_CREATE_H(NXTextType::TextStyle, TextStyle)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, TextPixelSize)
  Q_PROPERTY_CREATE_H(int, TextPointSize)
  Q_PROPERTY_CREATE_H(bool, IsHoverEnable)

public:
  explicit NXPushButton(QWidget *parent = nullptr);
  explicit NXPushButton(QString const &text, QWidget *parent = nullptr);
  ~NXPushButton();

  void setNXIcon(NXIconType::IconName icon);
  void setNXIcon(NXIconType::IconName icon, int iconSize);

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXPUSHBUTTON_H
