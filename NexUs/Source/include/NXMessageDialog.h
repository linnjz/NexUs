#ifndef NXMESSAGEDIALOG_H
#define NXMESSAGEDIALOG_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXMessageDialogPrivate;

class NX_EXPORT NXMessageDialog : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXMessageDialog)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Title)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Content)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, TitlePixelSize)
  Q_PROPERTY_CREATE_H(int, ContentPixelSize)

public:
  explicit NXMessageDialog(QWidget *parent = nullptr);
  ~NXMessageDialog();
  Q_SIGNAL void confirmed();
  Q_SIGNAL void cancelled();

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXMESSAGEDIALOG_H
