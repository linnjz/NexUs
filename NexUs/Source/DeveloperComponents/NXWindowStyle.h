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
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXWINDOWSTYLE_H
