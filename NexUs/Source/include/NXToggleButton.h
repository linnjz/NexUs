#ifndef NXTOGGLEBUTTON_H
#define NXTOGGLEBUTTON_H

#include <QPushButton>

#include "NXDef.h"
class NXToggleButtonPrivate;

class NX_EXPORT NXToggleButton : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXToggleButton)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Text)
  Q_PROPERTY_CREATE_H(int, BorderRadius)

public:
  explicit NXToggleButton(QWidget *parent = nullptr);
  explicit NXToggleButton(QString const &text, QWidget *parent = nullptr);
  ~NXToggleButton();

  void setNXIcon(NXIconType::IconName icon);

  void setIsToggled(bool isToggled);
  bool getIsToggled() const;
  Q_SIGNAL void toggled(bool checked);

protected:
  bool event(QEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXTOGGLEBUTTON_H
