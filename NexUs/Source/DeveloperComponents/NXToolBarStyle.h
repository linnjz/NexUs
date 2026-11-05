#ifndef NXTOOLBARSTYLE_H
#define NXTOOLBARSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"
class QStyleOptionToolButton;

class NXToolBarStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QSize, ToolButtonSize)

public:
  explicit NXToolBarStyle(QStyle *style = nullptr);
  ~NXToolBarStyle();
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  void _drawIndicator(QPainter *painter, QStyleOptionToolButton const *bopt, QWidget const *widget) const;
  void _drawIcon(QPainter *painter, QRectF iconRect, QStyleOptionToolButton const *bopt, QWidget const *widget) const;
  void _drawText(QPainter *painter, QRect contentRect, QStyleOptionToolButton const *bopt) const;
};

#endif // NXTOOLBARSTYLE_H
