#ifndef NXCOLORVALUESLIDERSTYLE_H
#define NXCOLORVALUESLIDERSTYLE_H

#include <QLinearGradient>
#include <QProxyStyle>

#include "NXDef.h"

class NXColorValueSliderStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QS_SET_CREF(QColor), BaseColor)
  Q_PRIVATE_CREATE(bool, IsUseAlpha)

public:
  explicit NXColorValueSliderStyle(QStyle *style = nullptr);
  ~NXColorValueSliderStyle();
  void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;
  int styleHint(StyleHint hint, const QStyleOption *option = nullptr, const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override;

private:
  NXThemeType::ThemeMode _themeMode;
  mutable QStyle::State _lastState { QStyle::State_None };
  mutable qreal _circleRadius { 0 };
  QLinearGradient *_baseGradient { nullptr };
  void _startRadiusAnimation(qreal startRadius, qreal endRadius, QWidget *widget) const;
};

#endif // NXCOLORVALUESLIDERSTYLE_H
