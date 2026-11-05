#ifndef NXCHECKBOXSTYLE_H
#define NXCHECKBOXSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXCheckBoxStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PRIVATE_CREATE(int, CheckIndicatorWidth)
  Q_PRIVATE_CREATE(int, BorderRadius)

public:
  explicit NXCheckBoxStyle(QStyle *style = nullptr);
  ~NXCheckBoxStyle();
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;
  QRect subElementRect(SubElement element, QStyleOption const *option, QWidget const *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXCHECKBOXSTYLE_H
