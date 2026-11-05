#ifndef NXCOLORVALUESLIDERSTYLE_H
#define NXCOLORVALUESLIDERSTYLE_H

#include <QLinearGradient>
#include <QProxyStyle>

#include "NXDef.h"

class NXColorValueSliderStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QPS_SCREF(QColor), BaseColor)
  Q_PRIVATE_CREATE(bool, IsUseAlpha)

public:
  explicit NXColorValueSliderStyle(QStyle *style = nullptr);
  ~NXColorValueSliderStyle();
  void drawComplexControl(ComplexControl control, QStyleOptionComplex const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;
  int styleHint(StyleHint hint, QStyleOption const *option = nullptr, QWidget const *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  mutable QStyle::State m_LastState { QStyle::State_None };
  mutable qreal m_CircleRadius { 0 };
  QLinearGradient *m_BaseGradient { nullptr };
  void _startRadiusAnimation(qreal startRadius, qreal endRadius, QWidget *widget) const;
};

#endif // NXCOLORVALUESLIDERSTYLE_H
