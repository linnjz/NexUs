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
  void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode _themeMode;
};

#endif // NXGROUPBOXSTYLE_H
