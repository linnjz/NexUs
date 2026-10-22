#ifndef NXSCROLLBARSTYLE_H
#define NXSCROLLBARSTYLE_H
#include <QProxyStyle>

#include "NXDef.h"
class NXScrollBar;

class NXScrollBarStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(qreal, Opacity)
  Q_PROPERTY_CREATE(qreal, SliderExtent)
  Q_PRIVATE_CREATE(NXScrollBar *, ScrollBar)
  Q_PRIVATE_CREATE(bool, IsExpand)

public:
  explicit NXScrollBarStyle(QStyle *style = nullptr);
  ~NXScrollBarStyle();
  void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;
  int styleHint(StyleHint hint, const QStyleOption *option = nullptr, const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override;
  void startExpandAnimation(bool isExpand);

private:
  NXThemeType::ThemeMode _themeMode;
#ifdef Q_OS_MACOS
  int _scrollBarExtent { 8 };
#else
  int _scrollBarExtent { 10 };
#endif
#ifdef Q_OS_MACOS
  qreal _sliderMargin { 2.0 };
#else
  qreal _sliderMargin { 2.5 };
#endif
};

#endif // NXSCROLLBARSTYLE_H
