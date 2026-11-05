#ifndef NXGROUPBOXSTYLE_H
#define NXGROUPBOXSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXGroupBoxStyle : public QProxyStyle
{
  Q_OBJECT

public:
  explicit NXGroupBoxStyle(QStyle *style = nullptr);
  ~NXGroupBoxStyle();
  void drawComplexControl(ComplexControl control, QStyleOptionComplex const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXGROUPBOXSTYLE_H
