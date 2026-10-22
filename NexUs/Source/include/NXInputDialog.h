#ifndef NXINPUTDIALOG_H
#define NXINPUTDIALOG_H

#include <QAbstractNativeEventFilter>
#include <QDialog>
#include <QLineEdit>

#include "NXAppBar.h"
#include "NXProperty.h"

class NXInputDialogPrivate;

class NX_EXPORT NXInputDialog : public QDialog
{
  Q_OBJECT
  Q_Q_CREATE(NXInputDialog)
  Q_TAKEOVER_NATIVEEVENT_H
  Q_PROPERTY_CREATE_H(QS_SET_CREF(QString), TitleText)
  Q_PROPERTY_CREATE_H(QS_SET_CREF(QString), SubTitleText)
  Q_PROPERTY_CREATE_H(QS_SET_CREF(QString), LabelText)
  Q_PROPERTY_CREATE_H(QS_SET_CREF(QString), TextValue)
  Q_PROPERTY_CREATE_H(QS_SET_CREF(QString), OkButtonText)
  Q_PROPERTY_CREATE_H(QS_SET_CREF(QString), CancelButtonText)
  Q_PROPERTY_CREATE_H(QS_SET_CREF(QString), PlaceholderText)
  Q_PROPERTY_CREATE_H(double, DoubleValue)
  Q_PROPERTY_CREATE_H(int, IntValue)
  Q_PROPERTY_CREATE_H(int, InputMinimumWidth)
  Q_PROPERTY_CREATE_H(int, InputMaximumWidth)

public:
  explicit NXInputDialog(QWidget *parent = nullptr);
  ~NXInputDialog();

  static QString getText(QWidget *parent, const QString &title, const QString &subtitle, const QString &label, const QString &text = QString(), bool *ok = nullptr, const QString &okButtonText = QStringLiteral("确定"), const QString &cancelButtonText = QStringLiteral("取消"), int inputMinWidth = 100, int inputMaxWidth = QWIDGETSIZE_MAX);

  static int getInt(QWidget *parent, const QString &title, const QString &subtitle, const QString &label, int value = 0, int minValue = -2147483647, int maxValue = 2147483647, int step = 1, bool *ok = nullptr, const QString &okButtonText = QStringLiteral("确定"), const QString &cancelButtonText = QStringLiteral("取消"), int inputMinWidth = 100, int inputMaxWidth = QWIDGETSIZE_MAX);

  static double getDouble(QWidget *parent, const QString &title, const QString &subtitle, const QString &label, double value = 0.0, double minValue = -2147483647.0, double maxValue = 2147483647.0, int decimals = 2, bool *ok = nullptr, const QString &okButtonText = QStringLiteral("确定"), const QString &cancelButtonText = QStringLiteral("取消"), int inputMinWidth = 100, int inputMaxWidth = QWIDGETSIZE_MAX);

  static QString getMultiLineText(QWidget *parent, const QString &title, const QString &subtitle, const QString &label, const QString &text = QString(), bool *ok = nullptr, const QString &okButtonText = QStringLiteral("确定"), const QString &cancelButtonText = QStringLiteral("取消"), int inputMinWidth = 100, int inputMaxWidth = QWIDGETSIZE_MAX);

  void setTextEchoMode(QLineEdit::EchoMode mode);
  QLineEdit::EchoMode textEchoMode() const;

  void setIntRange(int minValue, int maxValue, int step = 1);
  void setDoubleRange(double minValue, double maxValue, int decimals = 2);
  void setMultiLine(bool multiLine);

protected:
  void showEvent(QShowEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  Q_SIGNAL void textValueChanged(const QString &text);
  Q_SIGNAL void intValueChanged(int value);
  Q_SIGNAL void doubleValueChanged(double value);

private:
  Q_SLOT void onOkButtonClicked();
  Q_SLOT void onCancelButtonClicked();
  void updateLabels();
};

#endif // NXINPUTDIALOG_H
