#ifndef NXDOCKWIDGETPRIVATE_H
#define NXDOCKWIDGETPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXDockWidget;
class NXDockWidgetTitleBar;

class NXDockWidgetPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXDockWidget)

public:
  explicit NXDockWidgetPrivate(QObject *parent = nullptr);
  ~NXDockWidgetPrivate();
  Q_SLOT void onThemeModeChanged(NXThemeType::ThemeMode themeMode);

private:
  qint64 m_CurrentWinID { 0 };
  int m_Margins { 8 };
  NXThemeType::ThemeMode m_ThemeMode;
  NXApplicationType::WindowDisplayMode m_WindowDisplayMode;
  int m_ShadowBorderWidth { 6 };
  NXDockWidgetTitleBar *m_TitleBar { nullptr };
};

#endif // NXDOCKWIDGETPRIVATE_H
