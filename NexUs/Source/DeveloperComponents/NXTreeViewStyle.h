#ifndef NXTREEVIEWSTYLE_H
#define NXTREEVIEWSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXTreeViewStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(int, ItemHeight)
  Q_PROPERTY_CREATE(int, HeaderMargin)
  Q_PROPERTY_CREATE(NXIconType::IconName, IconName)

public:
  explicit NXTreeViewStyle(QStyle *style = nullptr);
  ~NXTreeViewStyle();
  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;
  QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const override;

private:
  NXThemeType::ThemeMode _themeMode;
  int _leftPadding { 11 };
};

#endif // NXTREEVIEWSTYLE_H
