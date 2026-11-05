#ifndef NXMENUSTYLE_H
#define NXMENUSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXMenuStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), MenuItemHeight)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), BorderRadius)

public:
  explicit NXMenuStyle(QStyle *style = nullptr);
  ~NXMenuStyle();
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;

private:
  mutable bool m_IsAnyoneItemHasIcon { false };
  int m_ShadowBorderWidth { 6 };
  NXThemeType::ThemeMode m_ThemeMode;
  mutable int m_IconWidth { 0 };
};

#endif // NXMENUSTYLE_H
