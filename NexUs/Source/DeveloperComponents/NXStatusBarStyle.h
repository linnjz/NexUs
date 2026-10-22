#ifndef NXSTATUSBARSTYLE_H
#define NXSTATUSBARSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXStatusBarStyle : public QProxyStyle
{
  Q_OBJECT

public:
  explicit NXStatusBarStyle(QStyle *style = nullptr);
  ~NXStatusBarStyle();
  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode _themeMode;
};

#endif // NXSTATUSBARSTYLE_H
