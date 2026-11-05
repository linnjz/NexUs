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
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_MenuBarItemMargin { 0 };
};

#endif // NXMENUBARSTYLE_H
