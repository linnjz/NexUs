#ifndef NXTOOLBUTTONSTYLE_H
#define NXTOOLBUTTONSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"
class QStyleOptionToolButton;

class NXToolButtonStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ExpandIconRotate)
  Q_PRIVATE_CREATE(int, BorderRadius)
  Q_PRIVATE_CREATE(bool, IsTransparent)
  Q_PRIVATE_CREATE(bool, IsSelected)

public:
  explicit NXToolButtonStyle(QStyle *style = nullptr);
  ~NXToolButtonStyle();
  void drawComplexControl(ComplexControl control, QStyleOptionComplex const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_ContentMargin { 6 };
  int m_HasMenuIndicatorWidth { 30 };
  void _drawIndicator(QPainter *painter, QStyleOptionToolButton const *bopt, QWidget const *widget) const;
  void _drawIcon(QPainter *painter, QRectF iconRect, QStyleOptionToolButton const *bopt, QWidget const *widget) const;
  void _drawText(QPainter *painter, QRect contentRect, QStyleOptionToolButton const *bopt) const;
  qreal _calculateExpandIndicatorWidth(QStyleOptionToolButton const *bopt, QPainter *painter) const;
};

#endif // NXTOOLBUTTONSTYLE_H
