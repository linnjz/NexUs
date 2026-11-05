#ifndef NXLISTVIEWSTYLE_H
#define NXLISTVIEWSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXListViewStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), ItemHeight)
  Q_PROPERTY_CREATE(QPS_PAEMIT(bool), IsTransparent)

public:
  explicit NXListViewStyle(QStyle *style = nullptr);
  ~NXListViewStyle();
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_LeftPadding { 11 };
};

#endif // NXLISTVIEWSTYLE_H
