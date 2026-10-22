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
  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;

private:
  NXThemeType::ThemeMode _themeMode;
  void _drawIndicator(QPainter *painter, const QStyleOptionToolButton *bopt, const QWidget *widget) const;
  void _drawIcon(QPainter *painter, QRectF iconRect, const QStyleOptionToolButton *bopt, const QWidget *widget) const;
  void _drawText(QPainter *painter, QRect contentRect, const QStyleOptionToolButton *bopt) const;
};

#endif // NXTOOLBARSTYLE_H
