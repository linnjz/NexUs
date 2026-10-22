#ifndef NXTABLEWIDGETSTYLE_H
#define NXTABLEWIDGETSTYLE_H

#include <QModelIndex>
#include <QProxyStyle>

#include "NXDef.h"

class QStyleOptionViewItem;

class NXTableWidgetStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(int, ItemHeight)
  Q_PROPERTY_CREATE(int, HeaderMargin)
  Q_PROPERTY_CREATE(int, CurrentHoverRow)
  Q_PROPERTY_CREATE(bool, IsTransparent)

public:
  explicit NXTableWidgetStyle(QStyle *style = nullptr);
  ~NXTableWidgetStyle();
  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode _themeMode;
  int _horizontalPadding { 8 };
};

#endif // NXTABLEWIDGETSTYLE_H
