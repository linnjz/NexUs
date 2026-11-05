#ifndef NXTOAST_H
#define NXTOAST_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXToastPrivate;

class NX_EXPORT NXToast : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXToast)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, DisplayMsec)

public:
  enum ToastType
  {
    Success = 0,
    Info,
    Warning,
    Error
  };
  Q_ENUM(ToastType)

  static void success(QString const &text, int displayMsec = 2000, QWidget *parent = nullptr);
  static void info(QString const &text, int displayMsec = 2000, QWidget *parent = nullptr);
  static void warning(QString const &text, int displayMsec = 2000, QWidget *parent = nullptr);
  static void error(QString const &text, int displayMsec = 2000, QWidget *parent = nullptr);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  explicit NXToast(ToastType type, QString const &text, int displayMsec, QWidget *parent = nullptr);
  ~NXToast();
};

#endif // NXTOAST_H
