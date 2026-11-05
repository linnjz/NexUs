#ifndef NXCONTENTDIALOGPRIVATE_H
#define NXCONTENTDIALOGPRIVATE_H

#include <QObject>

#include "NXDef.h"
class QVBoxLayout;
class QHBoxLayout;
class NXPushButton;
class NXContentDialog;
class NXMaskWidget;
class NXAppBar;

class NXContentDialogPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXContentDialog)

public:
  explicit NXContentDialogPrivate(QObject *parent = nullptr);
  ~NXContentDialogPrivate();

  void startCloseAnimation(NXPushButton *triggeredButton);

private:
  bool m_IsClosing { false };
  NXThemeType::ThemeMode m_ThemeMode;
  qint64 m_CurrentWinID { 0 };
  NXAppBar *m_AppBar { nullptr };
  NXMaskWidget *m_MaskWidget { nullptr };
  QWidget *m_CentralWidget { nullptr };
  QWidget *m_ButtonWidget { nullptr };
  QVBoxLayout *m_MainLayout { nullptr };
  QHBoxLayout *m_ButtonLayout { nullptr };
  NXPushButton *m_ActiveButton { nullptr };
  QList<NXPushButton *> m_Buttons;

  void _handleCloseAnimation();
  void _moveToCenter();
};

#endif // NXCONTENTDIALOGPRIVATE_H
