#ifndef NXACTIONCOMMANDERVIEWSTYLE_H
#define NXACTIONCOMMANDERVIEWSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXActionCommanderViewStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), ItemHeight)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), HeaderMargin)

public:
  explicit NXActionCommanderViewStyle(QStyle *style = nullptr);
  ~NXActionCommanderViewStyle() override;

  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option, QWidget const *widget) const override;
  QRect subElementRect(SubElement element, QStyleOption const *option, QWidget const *widget) const override;

  NXThemeType::ThemeMode getThemeMode() const;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_LeftPadding { 11 };
};

#endif //NXACTIONCOMMANDERVIEWSTYLE_H
