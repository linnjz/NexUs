#ifndef NXRADIOBUTTONSTYLE_H
#define NXRADIOBUTTONSTYLE_H

#include <QPointer>
#include <QProxyStyle>

#include "NXDef.h"

class QPropertyAnimation;

class NXRadioButtonStyle : public QProxyStyle
{
  Q_OBJECT
  // 动画保存半径比例，字体或指示器尺寸变化时仍按当前绘制区域缩放。
  Q_PROPERTY_CREATE(qreal, CircleScale)

public:
  explicit NXRadioButtonStyle(QStyle *style = nullptr);
  ~NXRadioButtonStyle();
  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode _themeMode;
  qreal _targetCircleScale { 2.0 / 3.8 };
  QPropertyAnimation *_circleScaleAnimation { nullptr };
  QPointer<QWidget> _animationWidget;

  void _updateCircleScale(QStyle::State state, const QWidget *widget);
};

#endif // NXRADIOBUTTONSTYLE_H
