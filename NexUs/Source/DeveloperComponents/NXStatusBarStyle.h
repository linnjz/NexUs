#ifndef NXSTATUSBARSTYLE_H
#define NXSTATUSBARSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXStatusBarStyle : public QProxyStyle
{
  Q_OBJECT

public:
  explicit NXStatusBarStyle(QStyle *style = nullptr);
  ~NXStatusBarStyle();
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXSTATUSBARSTYLE_H
