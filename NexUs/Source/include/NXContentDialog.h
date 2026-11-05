#ifndef NXCONTENTDIALOG_H
#define NXCONTENTDIALOG_H
#include <QAbstractNativeEventFilter>
#include <QDialog>

#include "NXAppBar.h"
#include "NXProperty.h"
class NXContentDialogPrivate;
class NXPushButton;

class NX_EXPORT NXContentDialog : public QDialog
{
  Q_OBJECT
  Q_Q_CREATE(NXContentDialog)
  Q_TAKEOVER_NATIVEEVENT_H

public:
  explicit NXContentDialog(QWidget *parent);
  ~NXContentDialog();

  void setCentralWidget(QWidget *widget);

  NXPushButton *addButton(QString const &text);
  void removeButton(NXPushButton *button);
  QList<NXPushButton *> buttons() const;

  void close();

  NXAppBar *appBar() const;
  Q_SIGNAL void buttonClicked(NXPushButton *button);

protected:
  void showEvent(QShowEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
};

#endif // NXCONTENTDIALOG_H
