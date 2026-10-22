#ifndef NXPIVOTSTYLE_H
#define NXPIVOTSTYLE_H

#include <QPixmap>
#include <QProxyStyle>

#include "NXPivotModel.h"

class NXPivotView;

class NXPivotStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QS_SET_CREF(QColor), TextNormalColor)
  Q_PRIVATE_CREATE(QS_SET_CREF(QColor), TextFocusColor)
  Q_PRIVATE_CREATE(QS_SET_CREF(QColor), TextPressedColor)
  Q_PRIVATE_CREATE(QS_SET_CREF(QColor), IndicatorColor)
  Q_PRIVATE_CREATE(QS_SET_CREF(QColor), ItemBackgroundColor)
  Q_PRIVATE_CREATE(QPoint, OverlayTextOffsetFromCenterAnchor)
  Q_PRIVATE_CREATE(QSize, ItemFixedSize)
  Q_PRIVATE_CREATE(NXPivotType::DisplayMode, DisplayMode, QS_DEFAULT_VALUE(NXPivotType::TextOnly))
  Q_PRIVATE_CREATE(NXPivotType::IndicatorTypeFlags, IndicatorTypeFlags, QS_DEFAULT_VALUE(NXPivotType::IndicatorBottom))
  Q_PRIVATE_CREATE(int, ItemHorizontalSpacing, QS_DEFAULT_VALUE(48))
  Q_PRIVATE_CREATE(int, ItemVerticalSpacing, QS_DEFAULT_VALUE(16))
  Q_PRIVATE_CREATE(int, TextIconSpacing, QS_DEFAULT_VALUE(6))
  Q_PRIVATE_CREATE(bool, IsAutoAdaptiveItemHeight, QS_DEFAULT_VALUE(true))
  Q_PRIVATE_CREATE(bool, IsItemFixedSize, QS_DEFAULT_VALUE(false))
  Q_PRIVATE_CREATE(bool, IsHoverItemBackgroundEnable, QS_DEFAULT_VALUE(false))

public:
  explicit NXPivotStyle(QStyle *style = nullptr);
  ~NXPivotStyle() override;
  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

  QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const override;

  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;

private:
  NXThemeType::ThemeMode _themeMode;

  void _drawCenteredPixmap(QPainter *painter, const QRect &containerRect, const QPixmap &pixmap) const;
  void _drawTextOnly(QPainter *painter, const QRect &contentRect, const QString &text) const;
  void _drawIconOnly(QPainter *painter, const QRect &contentRect, const QPixmap &icon) const;
  void _drawTextBesideIcon(QPainter *painter, const QRect &contentRect, const QPixmap &icon, const QString &text) const;
  void _drawTextUnderIcon(QPainter *painter, const QRect &contentRect, const QPixmap &icon, const QString &text) const;
  void
  _drawIconWithOverlayText(QPainter *painter, const QRect &contentRect, const QPixmap &icon, const QString &text) const;
  void _drawDisplayContent(QPainter *painter, const QRect &contentRect, const NXPivotModel::PivotItem &pivotItem) const;
  void _drawFocusRectBackground(QPainter *painter, const QRect &rect, const QColor &color) const;
  void _drawIndicator(QPainter *painter, const QRect &rect, const QColor &color) const;

  void _alignHeight(const NXPivotView *pivotView, int actualHeight) const;
  QSize _calculateItemSize(const NXPivotModel::PivotItem &pivotItem, const QFontMetrics &metrics, bool hasIcon, bool hasText) const;
};

#endif // NXPIVOTSTYLE_H
