#ifndef NXCOMBOBOXSTYLE_H
#define NXCOMBOBOXSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXComboBoxStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(qreal, ExpandIconRotate)
  Q_PROPERTY_CREATE(qreal, ExpandMarkWidth)

public:
  explicit NXComboBoxStyle(QStyle *style = nullptr);
  ~NXComboBoxStyle();
  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;

  QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget) const override;
  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;

  int getShadowBorderWidth() const;

private:
  NXThemeType::ThemeMode _themeMode;
  int _shadowBorderWidth { 6 };
};

#endif // NXCOMBOBOXSTYLE_H
