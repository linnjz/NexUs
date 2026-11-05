#ifndef NXSLIDERSTYLE_H
#define NXSLIDERSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXSliderStyle : public QProxyStyle
{
  Q_OBJECT

public:
  explicit NXSliderStyle(QStyle *style = nullptr);
  ~NXSliderStyle();
  void drawComplexControl(ComplexControl control, QStyleOptionComplex const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;
  int styleHint(StyleHint hint, QStyleOption const *option = nullptr, QWidget const *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  mutable QStyle::State m_LastState { QStyle::State_None };
  mutable qreal m_CircleRadius { 0 };
  void _startRadiusAnimation(qreal startRadius, qreal endRadius, QWidget *widget) const;
};

#endif // NXSLIDERSTYLE_H
