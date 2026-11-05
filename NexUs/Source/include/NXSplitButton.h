#ifndef NXSPLITBUTTON_H
#define NXSPLITBUTTON_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"
class NXMenu;
class NXSplitButtonPrivate;

class NX_EXPORT NXSplitButton : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXSplitButton)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Text)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(NXIconType::IconName, NXIcon)

public:
  explicit NXSplitButton(QWidget *parent = nullptr);
  ~NXSplitButton();

  void setMenu(NXMenu *menu);
  NXMenu *getMenu() const;
  Q_SIGNAL void clicked();

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  QSize sizeHint() const override;
};

#endif // NXSPLITBUTTON_H
