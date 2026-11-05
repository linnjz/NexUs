#ifndef NXMESSAGEDIALOGPRIVATE_H
#define NXMESSAGEDIALOGPRIVATE_H

#include <QObject>
#include <QWidget>

#include "NXDef.h"

class NXMessageDialog;

class NXMessageDialogButton : public QWidget
{
  Q_OBJECT

public:
  enum ButtonType
  {
    Confirm,
    Cancel
  };

  explicit NXMessageDialogButton(ButtonType type, QWidget *parent = nullptr);
  ~NXMessageDialogButton();
  Q_SIGNAL void clicked();

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEnterEvent *event) override;
#else
  void enterEvent(QEvent *event) override;
#endif
  void leaveEvent(QEvent *event) override;

private:
  ButtonType m_ButtonType;
  bool m_IsPressed { false };
  bool m_IsHovered { false };
  NXThemeType::ThemeMode m_ThemeMode;
};

class NXMessageDialogPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXMessageDialog)

  Q_PROPERTY_CREATE_D(QString, Title)
  Q_PROPERTY_CREATE_D(QString, Content)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, TitlePixelSize)
  Q_PROPERTY_CREATE_D(int, ContentPixelSize)

public:
  NXThemeType::ThemeMode m_ThemeMode;
  explicit NXMessageDialogPrivate(QObject *parent = nullptr);

  ~NXMessageDialogPrivate();

  NXMessageDialogButton *m_ConfirmButton { nullptr };
  NXMessageDialogButton *m_CancelButton { nullptr };
};

#endif // NXMESSAGEDIALOGPRIVATE_H
