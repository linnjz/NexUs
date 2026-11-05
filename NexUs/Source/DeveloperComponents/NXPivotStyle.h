#ifndef NXPIVOTSTYLE_H
#define NXPIVOTSTYLE_H

#include <QPixmap>
#include <QProxyStyle>

#include "NXPivotModel.h"

class NXPivotView;

class NXPivotStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PRIVATE_CREATE_D(NXThemeType::ThemeMode, ThemeMode)
  Q_PRIVATE_CREATE(QPS_SCREF(QColor), TextNormalColor)
  Q_PRIVATE_CREATE(QPS_SCREF(QColor), TextFocusColor)
  Q_PRIVATE_CREATE(QPS_SCREF(QColor), TextPressedColor)
  Q_PRIVATE_CREATE(QPS_SCREF(QColor), IndicatorColor)
  Q_PRIVATE_CREATE(QPS_SCREF(QColor), ItemBackgroundColor)
  Q_PRIVATE_CREATE(QPoint, OverlayTextOffsetFromCenterAnchor)
  Q_PRIVATE_CREATE(QSize, ItemFixedSize)
  Q_PRIVATE_CREATE(NXPivotType::DisplayMode, DisplayMode)
  Q_PRIVATE_CREATE(NXPivotType::IndicatorTypeFlags, IndicatorTypeFlags)
  Q_PRIVATE_CREATE(int, ItemHorizontalSpacing)
  Q_PRIVATE_CREATE(int, ItemVerticalSpacing)
  Q_PRIVATE_CREATE(int, TextIconSpacing)
  Q_PRIVATE_CREATE(bool, IsAutoAdaptiveItemHeight)
  Q_PRIVATE_CREATE(bool, IsItemFixedSize)
  Q_PRIVATE_CREATE(bool, IsHoverItemBackgroundEnable)

public:
  explicit NXPivotStyle(QStyle *style = nullptr);
  ~NXPivotStyle() override;
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;

  QRect subElementRect(SubElement element, QStyleOption const *option, QWidget const *widget) const override;

  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;

private:

  void _drawCenteredPixmap(QPainter *painter, QRect const &containerRect, QPixmap const &pixmap) const;
  void _drawTextOnly(QPainter *painter, QRect const &contentRect, QString const &text) const;
  void _drawIconOnly(QPainter *painter, QRect const &contentRect, QPixmap const &icon) const;
  void _drawTextBesideIcon(QPainter *painter, QRect const &contentRect, QPixmap const &icon, QString const &text) const;
  void _drawTextUnderIcon(QPainter *painter, QRect const &contentRect, QPixmap const &icon, QString const &text) const;
  void
  _drawIconWithOverlayText(QPainter *painter, QRect const &contentRect, QPixmap const &icon, QString const &text) const;
  void _drawDisplayContent(QPainter *painter, QRect const &contentRect, NXPivotModel::PivotItem const &pivotItem) const;
  void _drawFocusRectBackground(QPainter *painter, QRect const &rect, QColor const &color) const;
  void _drawIndicator(QPainter *painter, QRect const &rect, QColor const &color) const;

  void _alignHeight(NXPivotView const *pivotView, int actualHeight) const;
  QSize _calculateItemSize(NXPivotModel::PivotItem const &pivotItem, QFontMetrics const &metrics, bool hasIcon, bool hasText) const;
};

#endif // NXPIVOTSTYLE_H
