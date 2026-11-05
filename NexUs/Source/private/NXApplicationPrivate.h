#ifndef NXAPPLICATIONPRIVATE_H
#define NXAPPLICATIONPRIVATE_H

#include <QColor>
#include <QIcon>
#include <QObject>

#include "NXDef.h"
class NXApplication;

class NXApplicationPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXApplication)
  Q_PROPERTY_CREATE_D(QString, NXMicaImagePath)
  Q_PROPERTY_CREATE_D(NXApplicationType::WindowDisplayMode, WindowDisplayMode)
  Q_PROPERTY_CREATE_D(int, FontPixelSize)

public:
  explicit NXApplicationPrivate(QObject *parent = nullptr);
  ~NXApplicationPrivate();
  Q_SLOT void onThemeModeChanged(NXThemeType::ThemeMode themeMode);
  Q_SLOT void onSystemPaletteChanged();
  void syncSystemTheme();
  Q_SIGNAL void initMicaBase(QImage const &img);

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  friend class NXMicaBaseInitObject;
  NXThemeType::ThemeMode m_ThemeMode;
  QList<QWidget *> m_MicaWidgetList;
  QImage m_LightBaseImage;
  QImage m_DarkBaseImage;
  void _initMicaBaseImage(QImage const &img);
  QRect _calculateWindowVirtualGeometry(QWidget *widget);
  void _updateMica(QWidget *widget, bool isProcessEvent = true);
  void _updateAllMicaWidget();
  void _resetAllMicaWidget();
  bool _isSystemDarkMode() const;
};

#endif // NXAPPLICATIONPRIVATE_H
