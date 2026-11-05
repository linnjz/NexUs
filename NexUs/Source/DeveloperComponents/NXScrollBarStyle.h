#ifndef NXSCROLLBARSTYLE_H
#define NXSCROLLBARSTYLE_H
#include <QProxyStyle>

#include "NXDef.h"
class NXScrollBar;

class NXScrollBarStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), Opacity)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), SliderExtent)
  Q_PRIVATE_CREATE(NXScrollBar *, ScrollBar)
  Q_PRIVATE_CREATE(bool, IsExpand)

public:
  explicit NXScrollBarStyle(QStyle *style = nullptr);
  ~NXScrollBarStyle();
  void drawComplexControl(ComplexControl control, QStyleOptionComplex const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;
  int styleHint(StyleHint hint, QStyleOption const *option = nullptr, QWidget const *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override;
  void startExpandAnimation(bool isExpand);

private:
  NXThemeType::ThemeMode m_ThemeMode;
#ifdef Q_OS_MACOS
  int m_ScrollBarExtent { 8 };
#else
  int m_ScrollBarExtent { 10 };
#endif
#ifdef Q_OS_MACOS
  qreal m_SliderMargin { 2.0 };
#else
  qreal m_SliderMargin { 2.5 };
#endif
};

#endif // NXSCROLLBARSTYLE_H
