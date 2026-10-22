#ifndef NXPLAINTEXTEDITSTYLE_H
#define NXPLAINTEXTEDITSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXPlainTextEditStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(qreal, ExpandMarkWidth)

public:
  explicit NXPlainTextEditStyle(QStyle *style = nullptr);
  ~NXPlainTextEditStyle();
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode _themeMode;
};

#endif // NXPLAINTEXTEDITSTYLE_H
