#ifndef NXPLAINTEXTEDIT_H
#define NXPLAINTEXTEDIT_H

#include <QPlainTextEdit>

#include "NXProperty.h"

class NXPlainTextEditPrivate;

class NX_EXPORT NXPlainTextEdit : public QPlainTextEdit
{
  Q_OBJECT
  Q_Q_CREATE(NXPlainTextEdit)

public:
  explicit NXPlainTextEdit(QWidget *parent = nullptr);
  explicit NXPlainTextEdit(QString const &text, QWidget *parent = nullptr);
  ~NXPlainTextEdit();

protected:
  void focusInEvent(QFocusEvent *event) override;
  void focusOutEvent(QFocusEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
};

#endif // NXPLAINTEXTEDIT_H
