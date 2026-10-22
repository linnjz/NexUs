#ifndef NXWINDOWSTYLE_H
#define NXWINDOWSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXWindowStyle : public QProxyStyle
{
  Q_OBJECT

public:
  explicit NXWindowStyle(QStyle *style = nullptr);
  ~NXWindowStyle();
  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode _themeMode;
};

#endif // NXWINDOWSTYLE_H
