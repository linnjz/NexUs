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
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), CircleScale)

public:
  explicit NXRadioButtonStyle(QStyle *style = nullptr);
  ~NXRadioButtonStyle();
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  qreal m_TargetCircleScale { 2.0 / 3.8 };
  QPropertyAnimation *m_CircleScaleAnimation { nullptr };
  QPointer<QWidget> m_AnimationWidget;

  void _updateCircleScale(QStyle::State state, QWidget const *widget);
};

#endif // NXRADIOBUTTONSTYLE_H
