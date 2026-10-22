#ifndef NXSPINBOXSTYLE_H
#define NXSPINBOXSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXSpinBoxStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PRIVATE_CREATE(NXSpinBoxType::ButtonMode, ButtonMode)

public:
  explicit NXSpinBoxStyle(QStyle *style = nullptr);
  ~NXSpinBoxStyle();
  void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget) const override;

private:
  NXThemeType::ThemeMode _themeMode;
};

#endif // NXSPINBOXSTYLE_H
