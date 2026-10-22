#ifndef NXMENUBARSTYLE_H
#define NXMENUBARSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXMenuBarStyle : public QProxyStyle
{
  Q_OBJECT

public:
  explicit NXMenuBarStyle(QStyle *style = nullptr);
  ~NXMenuBarStyle();
  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode _themeMode;
  int _menuBarItemMargin { 0 };
};

#endif // NXMENUBARSTYLE_H
