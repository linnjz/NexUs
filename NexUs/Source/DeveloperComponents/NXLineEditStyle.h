#ifndef NXLINEEDITSTYLE_H
#define NXLINEEDITSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXLineEditStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QMargins, ContentsMargins)
  Q_PRIVATE_CREATE(int, BorderRadius)
  Q_PRIVATE_CREATE(int, IconMargin)

public:
  explicit NXLineEditStyle(QStyle *style = nullptr);
  ~NXLineEditStyle();
  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  QRect
  subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode _themeMode;
};

#endif // NXLINEEDITSTYLE_H
