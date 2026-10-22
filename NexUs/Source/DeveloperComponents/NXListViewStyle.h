#ifndef NXLISTVIEWSTYLE_H
#define NXLISTVIEWSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXListViewStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(int, ItemHeight)
  Q_PROPERTY_CREATE(bool, IsTransparent)

public:
  explicit NXListViewStyle(QStyle *style = nullptr);
  ~NXListViewStyle();
  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;

private:
  NXThemeType::ThemeMode _themeMode;
  int _leftPadding { 11 };
};

#endif // NXLISTVIEWSTYLE_H
