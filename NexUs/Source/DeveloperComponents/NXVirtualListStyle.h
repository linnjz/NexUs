#ifndef NXVIRTUALLISTSTYLE_H
#define NXVIRTUALLISTSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXVirtualListStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), ItemHeight)

public:
  explicit NXVirtualListStyle(QStyle *style = nullptr);
  ~NXVirtualListStyle();
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_LeftPadding { 11 };
};

#endif // NXVIRTUALLISTSTYLE_H
