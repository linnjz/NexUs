#ifndef NXCHECKBOX_H
#define NXCHECKBOX_H

#include <QCheckBox>

#include "NXDef.h"

class NXCheckBoxPrivate;

class NX_EXPORT NXCheckBox : public QCheckBox
{
  Q_OBJECT
  Q_Q_CREATE(NXCheckBox)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, CheckIndicatorWidth)
  Q_PROPERTY_CREATE_H(int, TextPixelSize)
  Q_PROPERTY_CREATE_H(int, TextPointSize)
  Q_PROPERTY_CREATE_H(NXTextType::TextStyle, TextStyle)

public:
  explicit NXCheckBox(QWidget *parent = nullptr);
  explicit NXCheckBox(QString const &text, QWidget *parent = nullptr);
  ~NXCheckBox();
};

#endif // NXCHECKBOX_H
