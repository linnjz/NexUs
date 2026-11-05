#ifndef NXFRAMEWORK_NXDIALOGPRIVATE_H
#define NXFRAMEWORK_NXDIALOGPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXAppBar;
class NXDialog;

class NXDialogPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXDialog)

public:
  explicit NXDialogPrivate(QObject *parent = nullptr);
  ~NXDialogPrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
  NXApplicationType::WindowDisplayMode m_WindowDisplayMode;
  NXAppBar *m_AppBar { nullptr };
};

#endif //NXFRAMEWORK_NXDIALOGPRIVATE_H
