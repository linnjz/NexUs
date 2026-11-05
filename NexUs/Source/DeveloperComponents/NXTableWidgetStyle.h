#ifndef NXTABLEWIDGETSTYLE_H
#define NXTABLEWIDGETSTYLE_H

#include <QModelIndex>
#include <QProxyStyle>

#include "NXDef.h"

class QStyleOptionViewItem;

class NXTableWidgetStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), ItemHeight)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), HeaderMargin)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), CurrentHoverRow)
  Q_PROPERTY_CREATE(QPS_PAEMIT(bool), IsTransparent)

public:
  explicit NXTableWidgetStyle(QStyle *style = nullptr);
  ~NXTableWidgetStyle();
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_HorizontalPadding { 8 };
};

#endif // NXTABLEWIDGETSTYLE_H
