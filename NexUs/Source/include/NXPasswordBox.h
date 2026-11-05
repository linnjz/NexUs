#ifndef NXPASSWORDBOX_H
#define NXPASSWORDBOX_H

#include <QLineEdit>

#include "NXProperty.h"

class NXPasswordBoxPrivate;

class NX_EXPORT NXPasswordBox : public QLineEdit
{
  Q_OBJECT
  Q_Q_CREATE(NXPasswordBox)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(bool, IsPasswordVisible)

public:
  explicit NXPasswordBox(QWidget *parent = nullptr);
  ~NXPasswordBox();
  Q_SIGNAL void focusIn(QString const &text);
  Q_SIGNAL void focusOut(QString const &text);
  Q_SIGNAL void wmFocusOut(QString const &text);

protected:
  void focusInEvent(QFocusEvent *event) override;
  void focusOutEvent(QFocusEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // NXPASSWORDBOX_H
