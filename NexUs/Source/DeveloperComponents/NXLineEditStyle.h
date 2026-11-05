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
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  QRect
  subElementRect(SubElement element, QStyleOption const *option, QWidget const *widget) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXLINEEDITSTYLE_H
