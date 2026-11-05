#ifndef NXLINEEDIT_H
#define NXLINEEDIT_H

#include <QLineEdit>

#include "NXProperty.h"

class NXLineEditPrivate;

class NX_EXPORT NXLineEdit : public QLineEdit
{
  Q_OBJECT
  Q_Q_CREATE(NXLineEdit)
  Q_PROPERTY_CREATE_H(QMargins, ContentsMargins)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, IconMargin)
  Q_PROPERTY_CREATE_H(bool, IsClearButtonEnable)

public:
  explicit NXLineEdit(QWidget *parent = nullptr);
  explicit NXLineEdit(QString const &text, QWidget *parent = nullptr);

  ~NXLineEdit();
  Q_SIGNAL void focusIn(QString const &text);
  Q_SIGNAL void focusOut(QString const &text);
  Q_SIGNAL void wmFocusOut(QString const &text);

protected:
  void focusInEvent(QFocusEvent *event) override;
  void focusOutEvent(QFocusEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // NXLINEEDIT_H
