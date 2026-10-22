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

public:
  explicit NXTabBarStyle(QStyle *style = nullptr);
  ~NXTabBarStyle();
  void
  drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;
  QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const override;

  int styleHint(StyleHint hint, const QStyleOption *option = nullptr, const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override;

  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

private:
  int _closeIndicatorSize;
  NXThemeType::ThemeMode _themeMode;
  NXTabBarType::TabBarStyle _pTabBarStyle;
};

#endif // NXTABBARSTYLE_H
