#ifndef NXACTIONCOMMANDERVIEWSTYLE_H
#define NXACTIONCOMMANDERVIEWSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXActionCommanderViewStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(int, ItemHeight)
  Q_PROPERTY_CREATE(int, HeaderMargin)

public:
  explicit NXActionCommanderViewStyle(QStyle *style = nullptr);
  ~NXActionCommanderViewStyle() override;

  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override;
  QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const override;

  NXThemeType::ThemeMode getThemeMode() const;

private:
  NXThemeType::ThemeMode _themeMode;
  int _leftPadding { 11 };
};

#endif //NXACTIONCOMMANDERVIEWSTYLE_H
