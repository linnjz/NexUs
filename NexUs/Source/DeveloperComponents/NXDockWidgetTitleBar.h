#ifndef NXDOCKWIDGETTITLEBAR_H
#define NXDOCKWIDGETTITLEBAR_H

#include <QDockWidget>
#include <QLabel>
#include <QWidget>

#include "NXDef.h"
class NXText;
class NXIconButton;
class NXDockWidget;

class NXDockWidgetTitleBar : public QWidget
{
  Q_OBJECT

public:
  explicit NXDockWidgetTitleBar(QWidget *parent = nullptr);
  ~NXDockWidgetTitleBar();

  Q_SLOT void onFloatButtonClicked();
  Q_SLOT void onCloseButtonClicked();

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  friend class NXDockWidgetPrivate;
  NXThemeType::ThemeMode m_ThemeMode;
  NXDockWidget *m_DockWidget { nullptr };
  QLabel *m_IconLabel { nullptr };
  NXText *m_TitleLabel { nullptr };
  NXIconButton *m_FloatButton { nullptr };
  NXIconButton *m_CloseButton { nullptr };
  void _setVisibleFromFeatures(QDockWidget::DockWidgetFeatures features);
};

#endif // NXDOCKWIDGETTITLEBAR_H
