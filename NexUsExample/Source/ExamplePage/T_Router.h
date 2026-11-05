#ifndef T_ROUTER_H
#define T_ROUTER_H

#include "T_BasePage.h"

class NXPlainTextEdit;

class T_Router : public T_BasePage
{
  Q_OBJECT

public:
  Q_INVOKABLE explicit T_Router(QWidget *parent = nullptr);
  ~T_Router();

private:
  NXPlainTextEdit *_logEdit { nullptr };
  void appendLog(QString const &text);
};

#endif // T_ROUTER_H
