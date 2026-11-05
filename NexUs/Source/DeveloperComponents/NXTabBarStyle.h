#ifndef NXTABBARSTYLE_H
#define NXTABBARSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXTabBarStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QSize, TabSize)
  Q_PRIVATE_CREATE(int, TabCornerRadius)
  Q_PRIVATE_CREATE(bool, IsSelectedIndicatorVisible)

  Q_PRIVATE_CREATE_H(NXTabBarType::TabBarStyle, TabBarStyle)
  Q_PRIVATE_CREATE_D(NXTabBarType::TabBarStyle, TabBarStyle)

public:
  explicit NXTabBarStyle(QStyle *style = nullptr);
  ~NXTabBarStyle();
  void
  drawPrimitive(QStyle::PrimitiveElement pe, QStyleOption const *opt, QPainter *p, QWidget const *w) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;
  QRect subElementRect(SubElement element, QStyleOption const *option, QWidget const *widget) const override;

  int styleHint(StyleHint hint, QStyleOption const *option = nullptr, QWidget const *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override;

  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;

private:
  int m_CloseIndicatorSize;
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXTABBARSTYLE_H
