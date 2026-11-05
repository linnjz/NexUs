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
  void drawComplexControl(ComplexControl control, QStyleOptionComplex const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  QRect subControlRect(ComplexControl cc, QStyleOptionComplex const *opt, SubControl sc, QWidget const *widget) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXSPINBOXSTYLE_H
