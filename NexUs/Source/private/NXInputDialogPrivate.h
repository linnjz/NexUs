#ifndef NXINPUTDIALOGPRIVATE_H
#define NXINPUTDIALOGPRIVATE_H

#include <QObject>

#include "NXDef.h"

class NXInputDialog;
class NXMaskWidget;
class NXAppBar;
class NXLineEdit;
class NXSpinBox;
class NXDoubleSpinBox;
class NXText;
class NXPushButton;
class QWidget;
class QTextEdit;
class QVBoxLayout;

class NXInputDialogPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXInputDialog)
  Q_PROPERTY_CREATE_D(QString, TitleText)
  Q_PROPERTY_CREATE_D(QString, SubTitleText)
  Q_PROPERTY_CREATE_D(QString, LabelText)
  Q_PROPERTY_CREATE_D(QString, TextValue)
  Q_PROPERTY_CREATE_D(QString, OkButtonText)
  Q_PROPERTY_CREATE_D(QString, CancelButtonText)
  Q_PROPERTY_CREATE_D(QString, PlaceholderText)
  Q_PROPERTY_CREATE_D(double, DoubleValue)
  Q_PROPERTY_CREATE_D(int, IntValue)
  Q_PROPERTY_CREATE_D(int, InputMinimumWidth)
  Q_PROPERTY_CREATE_D(int, InputMaximumWidth)

public:
  explicit NXInputDialogPrivate(QObject *parent = nullptr);
  ~NXInputDialogPrivate();

private:
  friend class NXInputDialog;
  bool m_IsMultiLine { false };
  bool m_IsIntMode { false };
  bool m_IsDoubleMode { false };
  NXThemeType::ThemeMode m_ThemeMode;
  qint64 m_CurrentWinID { 0 };
  NXAppBar *m_AppBar { nullptr };
  NXMaskWidget *m_MaskWidget { nullptr };
  NXText *m_TitleLabel { nullptr };
  NXText *m_SubTitleLabel { nullptr };
  NXText *m_InputLabel { nullptr };
  NXLineEdit *m_LineEdit { nullptr };
  NXSpinBox *m_SpinBox { nullptr };
  NXDoubleSpinBox *m_DoubleSpinBox { nullptr };
  QTextEdit *m_TextEdit { nullptr };
  QVBoxLayout *m_ContentLayout { nullptr };
  QWidget *m_ButtonWidget { nullptr };
  NXPushButton *m_OkButton { nullptr };
  NXPushButton *m_CancelButton { nullptr };
  void _doCloseAnimation(bool isAccept);
};

#endif // NXINPUTDIALOGPRIVATE_H
