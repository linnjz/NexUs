#ifndef NXRADIOBUTTON_H
#define NXRADIOBUTTON_H

#include <QRadioButton>

#include "NXDef.h"
class NXRadioButtonPrivate;

class NX_EXPORT NXRadioButton : public QRadioButton
{
  Q_OBJECT
  Q_Q_CREATE(NXRadioButton)
  Q_PROPERTY_CREATE_H(int, TextPixelSize)
  Q_PROPERTY_CREATE_H(int, TextPointSize)
  Q_PROPERTY_CREATE_H(NXTextType::TextStyle, TextStyle)

public:
  explicit NXRadioButton(QWidget *parent = nullptr);
  explicit NXRadioButton(QString const &text, QWidget *parent = nullptr);
  ~NXRadioButton();

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXRADIOBUTTON_H
