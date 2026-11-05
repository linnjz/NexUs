#ifndef NEXUS_SRC_NXKEYBINDER_H_
#define NEXUS_SRC_NXKEYBINDER_H_

#include <QLabel>

#include "NXDef.h"

class NXKeyBinderPrivate;

class NX_EXPORT NXKeyBinder : public QLabel
{
  Q_OBJECT
  Q_Q_CREATE(NXKeyBinder)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), BinderKeyText)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(quint32, NativeVirtualBinderKey)

public:
  explicit NXKeyBinder(QWidget *parent = nullptr);
  ~NXKeyBinder();
  Q_SIGNAL void binderKeyTextChanged(QString const &binderKeyText);
  Q_SIGNAL void nativeVirtualBinderKeyChanged(quint32 binderKey);

protected:
  bool event(QEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
};

#endif //NEXUS_SRC_NXKEYBINDER_H_
