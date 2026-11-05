#ifndef NXTREEVIEWSTYLE_H
#define NXTREEVIEWSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXTreeViewStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), ItemHeight)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), HeaderMargin)
  Q_PROPERTY_CREATE(QPS_PAEMIT(NXIconType::IconName), IconName)

public:
  explicit NXTreeViewStyle(QStyle *style = nullptr);
  ~NXTreeViewStyle();
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;
  QRect subElementRect(SubElement element, QStyleOption const *option, QWidget const *widget) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_LeftPadding { 11 };
};

#endif // NXTREEVIEWSTYLE_H
