#ifndef NXCOPYBUTTONH
#define NXCOPYBUTTONH

#include <QPushButton>

#include "NXDef.h"
#include "NXProperty.h"
class NXCopyButtonPrivate;

class NX_EXPORT NXCopyButton : public QPushButton
{
  Q_OBJECT
  Q_Q_CREATE(NXCopyButton)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), CopyText)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), SuccessText)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, SuccessDuration)

public:
  explicit NXCopyButton(QWidget *parent = nullptr);
  explicit NXCopyButton(QString const &text, QWidget *parent = nullptr);
  ~NXCopyButton();

  void setText(QString const &text);
  void setNXIcon(NXIconType::IconName icon);
  void setSuccessIcon(NXIconType::IconName icon);
  Q_SIGNAL void copyCompleted(QString const &text);

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXCOPYBUTTONH
