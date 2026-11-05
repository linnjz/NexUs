#ifndef NXPLAINTEXTEDITSTYLE_H
#define NXPLAINTEXTEDITSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXPlainTextEditStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ExpandMarkWidth)

public:
  explicit NXPlainTextEditStyle(QStyle *style = nullptr);
  ~NXPlainTextEditStyle();
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXPLAINTEXTEDITSTYLE_H
