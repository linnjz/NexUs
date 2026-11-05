#ifndef NXINFOBAR_H
#define NXINFOBAR_H

#include <functional>

#include <QFrame>

#include "NXDef.h"
#include "NXProperty.h"

class NXInfoBarPrivate;

class NX_EXPORT NXInfoBar : public QFrame
{
  Q_OBJECT
  Q_Q_CREATE(NXInfoBar)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Title)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Message)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(NXInfoBarType::InfoBarSeverity, Severity)
  Q_PROPERTY_CREATE_H(NXIconType::IconName, InfoBarIcon)
  Q_PROPERTY_CREATE_H(bool, IsClosable)

public:
  explicit NXInfoBar(QWidget *parent = nullptr);
  explicit NXInfoBar(NXInfoBarType::InfoBarSeverity severity, QWidget *parent = nullptr);
  ~NXInfoBar();

  void addAction(QString const &text, std::function<void()> const &callback);
  void clearActions();

  void closeInfoBar();
  Q_SIGNAL void closed();
  Q_SIGNAL void closeButtonClicked();

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

#endif // NXINFOBAR_H
