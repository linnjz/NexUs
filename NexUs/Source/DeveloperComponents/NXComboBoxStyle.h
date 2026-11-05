#ifndef NXCOMBOBOXSTYLE_H
#define NXCOMBOBOXSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXComboBoxStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ExpandIconRotate)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ExpandMarkWidth)

public:
  explicit NXComboBoxStyle(QStyle *style = nullptr);
  ~NXComboBoxStyle();
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawComplexControl(ComplexControl control, QStyleOptionComplex const *option, QPainter *painter, QWidget const *widget = nullptr) const override;

  QRect subControlRect(ComplexControl cc, QStyleOptionComplex const *opt, SubControl sc, QWidget const *widget) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;

  int getShadowBorderWidth() const;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_ShadowBorderWidth { 6 };
};

#endif // NXCOMBOBOXSTYLE_H
