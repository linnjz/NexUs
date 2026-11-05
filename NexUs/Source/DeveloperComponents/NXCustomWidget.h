#ifndef NXCUSTOMWIDGET_H
#define NXCUSTOMWIDGET_H

#include <QDialog>

#include "NXAppBar.h"
#include "NXDef.h"

class QVBoxLayout;

class NXCustomWidget : public QDialog
{
  Q_OBJECT
  Q_TAKEOVER_NATIVEEVENT_H

public:
  explicit NXCustomWidget(QWidget *parent = nullptr);
  ~NXCustomWidget();

  void setCentralWidget(QWidget *widget);

protected:
  void paintEvent(QPaintEvent *event) override;
  QVBoxLayout *m_MainLayout { nullptr };
  NXAppBar *m_AppBar { nullptr };
  QWidget *m_CentralWidget { nullptr };

private:
  NXThemeType::ThemeMode m_ThemeMode;
  NXApplicationType::WindowDisplayMode m_WindowDisplayMode;
};

#endif // NXCUSTOMWIDGET_H
