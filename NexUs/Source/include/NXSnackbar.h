#ifndef NXSTACKBAR_H
#define NXSTACKBAR_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXSnackbarPrivate;

class NX_EXPORT NXSnackbar : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXSnackbar)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, DisplayMsec)

public:
  enum SnackbarType
  {
    Success = 0,
    Info,
    Warning,
    Error
  };
  Q_ENUM(SnackbarType)

  void dismiss();

  static void setMaxCount(int count);
  static int getMaxCount();

  static NXSnackbar *success(QString const &text, QString const &actionText = QStringLiteral(""), int displayMsec = 4000, QWidget *parent = nullptr);
  static NXSnackbar *info(QString const &text, QString const &actionText = QStringLiteral(""), int displayMsec = 4000, QWidget *parent = nullptr);
  static NXSnackbar *warning(QString const &text, QString const &actionText = QStringLiteral(""), int displayMsec = 4000, QWidget *parent = nullptr);
  static NXSnackbar *error(QString const &text, QString const &actionText = QStringLiteral(""), int displayMsec = 4000, QWidget *parent = nullptr);
  Q_SIGNAL void actionClicked();
  Q_SIGNAL void closed();

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

private:
  explicit NXSnackbar(
      SnackbarType type, QString const &text, QString const &actionText, int displayMsec, QWidget *parent = nullptr);
  ~NXSnackbar();
  static void _reflowAll();
};

#endif // NXSTACKBAR_H
