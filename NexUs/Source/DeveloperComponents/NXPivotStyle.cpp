#include "NXPivotStyle.h"

#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QtGlobal>

#include "NXPivotView.h"
#include "NXTheme.h"

namespace
{
constexpr qreal RADIUS = 3.8;

QSize LogicalPixmapSize(QPixmap const &pixmap)
{
  qreal const dpr  = qMax(1.0, pixmap.devicePixelRatioF());
  int const width  = qMax(1, qRound(static_cast<qreal>(pixmap.width()) / dpr));
  int const height = qMax(1, qRound(static_cast<qreal>(pixmap.height()) / dpr));
  return QSize(width, height);
}
} // namespace

NXPivotStyle::NXPivotStyle(QStyle *style)
    : m_ThemeMode { nxTheme->getThemeMode() }
    , m_TextNormalColor { NXThemeColor(m_ThemeMode, BasicTextNoFocus) }
    , m_TextFocusColor { NXThemeColor(m_ThemeMode, BasicText) }
    , m_TextPressedColor { NXThemeColor(m_ThemeMode, BasicTextPress) }
    , m_IndicatorColor { NXThemeColor(m_ThemeMode, PrimaryNormal) }
    , m_ItemBackgroundColor { Qt::transparent }
    , m_OverlayTextOffsetFromCenterAnchor {}
    , m_ItemFixedSize {}
    , m_DisplayMode { NXPivotType::TextOnly }
    , m_IndicatorTypeFlags { NXPivotType::IndicatorBottom }
    , m_ItemHorizontalSpacing { 48 }
    , m_ItemVerticalSpacing { 16 }
    , m_TextIconSpacing { 6 }
    , m_IsAutoAdaptiveItemHeight { true }
    , m_IsItemFixedSize { false }
    , m_IsHoverItemBackgroundEnable { false }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXPivotStyle::~NXPivotStyle()
{
}

void NXPivotStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::PE_PanelItemViewRow : {
    return;
  }
  case QStyle::PE_Widget : {
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void NXPivotStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    // viewport视口外的其他区域背景
    return;
  }
  case QStyle::CE_ItemViewItem : {
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      auto const *pivotView = qobject_cast<NXPivotView const *>(widget);
      if (!pivotView) {
        break;
      }
      QModelIndex const &pressIndex   = pivotView->getPressIndex();
      QModelIndex const &currentIndex = pivotView->getCommittedIndex();
      auto const pivotItem            = vopt->index.data(Qt::UserRole).value<NXPivotModel::PivotItem>();
      bool const isHover              = vopt->state.testFlag(QStyle::State_MouseOver);
      bool const isSelected           = currentIndex.row() == vopt->index.row();

      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

      QRect focusRect = proxy()->subElementRect(SE_ItemViewItemFocusRect, vopt, widget);

      painter->setPen(Qt::NoPen);
      if (isSelected && m_ItemBackgroundColor != Qt::transparent) {
        _drawFocusRectBackground(painter, focusRect, m_ItemBackgroundColor);
        if (m_IndicatorTypeFlags.testFlag(NXPivotType::IndicatorCheck)) {
          _drawIndicator(painter, focusRect, m_IndicatorColor);
        }
      } else if (isHover && m_IsHoverItemBackgroundEnable) {
        _drawFocusRectBackground(painter, focusRect, NXThemeColor(m_ThemeMode, BasicBaseDeep));
      }

      QColor contentColor = m_TextNormalColor;
      if (isSelected || isHover) {
        contentColor = m_TextFocusColor;
      }
      if (pressIndex.row() == vopt->index.row()) {
        contentColor = m_TextPressedColor;
      }

      painter->setPen(contentColor);
      _drawDisplayContent(painter, focusRect, pivotItem);

      painter->restore();
    }
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawControl(element, option, painter, widget);
}

int NXPivotStyle::pixelMetric(PixelMetric metric, QStyleOption const *option, QWidget const *widget) const
{
  //switch (metric)
  //{
  //case QStyle::PM_FocusFrameHMargin :
  //{
  //  return m_ItemHorizontalSpacing;
  //}
  //default :
  //{
  //  break;
  //}
  //}

  return QProxyStyle::pixelMetric(metric, option, widget);
}

QRect NXPivotStyle::subElementRect(SubElement element, QStyleOption const *option, QWidget const *widget) const
{
  if (element == SE_ItemViewItemFocusRect) {
    QRect rect = option->rect;
    rect.setHeight(widget->height());
    return rect;
  }
  return QProxyStyle::subElementRect(element, option, widget);
}

QSize NXPivotStyle::sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const
{
  auto pivotView = qobject_cast<NXPivotView const *>(widget);
  if (!pivotView || type != QStyle::CT_ItemViewItem) {
    return QProxyStyle::sizeFromContents(type, option, size, widget);
  }

  if (m_IsItemFixedSize) {
    int actualHeight = m_ItemFixedSize.height();
    _alignHeight(pivotView, actualHeight);
    return m_ItemFixedSize;
  }

  auto const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option);
  if (!vopt) {
    return QProxyStyle::sizeFromContents(type, option, size, widget);
  }
  auto const pivotItem = vopt->index.data(Qt::UserRole).value<NXPivotModel::PivotItem>();
  bool hasIcon         = !pivotItem.icon.isNull();
  bool hasText         = !pivotItem.text.isEmpty();
  if (!hasIcon && !hasText) {
    return QSize {};
  }
  QFontMetrics const metrics(pivotView->font());
  QSize computedSize = _calculateItemSize(pivotItem, metrics, hasIcon, hasText);
  _alignHeight(pivotView, computedSize.height());

  return computedSize;
}

void NXPivotStyle::_drawCenteredPixmap(QPainter *painter, QRect const &containerRect, QPixmap const &pixmap) const
{
  QSize const drawSize = LogicalPixmapSize(pixmap);
  int const drawWidth  = drawSize.width();
  int const drawHeight = drawSize.height();
  int const x          = containerRect.x() + (containerRect.width() - drawWidth) / 2;
  int const y          = containerRect.y() + (containerRect.height() - drawHeight) / 2;

  painter->drawPixmap(QPoint { x, y }, pixmap);
}

void NXPivotStyle::_drawTextOnly(QPainter *painter, QRect const &contentRect, QString const &text) const
{
  QFontMetrics const metrics(painter->font());
  QString const drawText = metrics.elidedText(text, Qt::ElideRight, contentRect.width());
  painter->drawText(contentRect.toRectF().adjusted(0, 0, -3.5, -3.5), Qt::AlignCenter, drawText);
}

void NXPivotStyle::_drawIconOnly(QPainter *painter, QRect const &contentRect, QPixmap const &icon) const
{
  _drawCenteredPixmap(painter, contentRect, icon);
}

void NXPivotStyle::_drawTextBesideIcon(QPainter *painter, QRect const &contentRect, QPixmap const &icon, QString const &text) const
{
  QFontMetrics const metrics(painter->font());
  QSize const iconDrawSize = LogicalPixmapSize(icon);
  int const textMaxWidth   = qMax(0, contentRect.width() - iconDrawSize.width() - m_TextIconSpacing);
  QString const drawText   = metrics.elidedText(text, Qt::ElideRight, textMaxWidth);
  int const textWidth      = qMin(textMaxWidth, metrics.horizontalAdvance(drawText));
  int const groupWidth     = iconDrawSize.width() + m_TextIconSpacing + textWidth;
  int const startX         = contentRect.x() + qMax(0, (contentRect.width() - groupWidth) / 2);

  QRect const iconRect(startX, contentRect.y() + (contentRect.height() - iconDrawSize.height()) / 2, iconDrawSize.width(), iconDrawSize.height());
  _drawCenteredPixmap(painter, iconRect, icon);

  int const textStartX     = iconRect.right() + 1 + m_TextIconSpacing;
  int const textWidthLimit = qMax(0, contentRect.right() - textStartX + 1);
  QRect const textRect(textStartX, contentRect.y(), textWidthLimit, contentRect.height());
  painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, drawText);
}

void NXPivotStyle::_drawTextUnderIcon(QPainter *painter, QRect const &contentRect, QPixmap const &icon, QString const &text) const
{
  QFontMetrics const metrics(painter->font());
  int const textHeight     = metrics.height();
  QSize const iconDrawSize = LogicalPixmapSize(icon);
  int const totalHeight    = iconDrawSize.height() + m_TextIconSpacing + textHeight;
  int const startY         = contentRect.y() + qMax(0, (contentRect.height() - totalHeight) / 2);

  QRect const iconRect(contentRect.x() + qMax(0, (contentRect.width() - iconDrawSize.width()) / 2), startY, iconDrawSize.width(), iconDrawSize.height());
  _drawCenteredPixmap(painter, iconRect, icon);

  QRect const textRect(contentRect.x(), startY + iconDrawSize.height() + m_TextIconSpacing, contentRect.width(), textHeight);
  QString const drawText = metrics.elidedText(text, Qt::ElideRight, textRect.width());
  painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter, drawText);
}

void NXPivotStyle::_drawIconWithOverlayText(QPainter *painter, QRect const &contentRect, QPixmap const &icon, QString const &text) const
{
  _drawCenteredPixmap(painter, contentRect, icon);

  QFontMetrics const metrics(painter->font());
  int const textWidthLimit = qMax(1, contentRect.width());
  QString const drawText   = metrics.elidedText(text, Qt::ElideRight, textWidthLimit);
  int const textWidth      = qMax(1, qMin(textWidthLimit, metrics.horizontalAdvance(drawText)));
  int const textHeight     = metrics.height();

  QPoint center = contentRect.center();
  center += m_OverlayTextOffsetFromCenterAnchor;

  QRect overlayRect(center.x() - textWidth / 2, center.y() - textHeight / 2, textWidth, textHeight);

  int const maxX = contentRect.right() - overlayRect.width() + 1;
  int const maxY = contentRect.bottom() - overlayRect.height() + 1;
  overlayRect.moveLeft(qBound(contentRect.left(), overlayRect.left(), maxX));
  overlayRect.moveTop(qBound(contentRect.top(), overlayRect.top(), maxY));

  painter->drawText(overlayRect, Qt::AlignLeft | Qt::AlignVCenter, drawText);
}

void NXPivotStyle::_drawDisplayContent(QPainter *painter, QRect const &contentRect, NXPivotModel::PivotItem const &pivotItem) const
{
  bool const hasText = !pivotItem.text.isEmpty();
  bool const hasIcon = !pivotItem.icon.isNull();

  switch (m_DisplayMode) {
  case NXPivotType::IconOnly : {
    if (hasIcon) {
      _drawIconOnly(painter, contentRect, pivotItem.icon);
    }
    break;
  }
  case NXPivotType::TextOnly : {
    if (hasText) {
      _drawTextOnly(painter, contentRect, pivotItem.text);
    }
    break;
  }
  case NXPivotType::TextBesideIcon : {
    if (hasIcon && hasText) {
      _drawTextBesideIcon(painter, contentRect, pivotItem.icon, pivotItem.text);
    } else if (hasIcon) {
      _drawIconOnly(painter, contentRect, pivotItem.icon);
    } else if (hasText) {
      _drawTextOnly(painter, contentRect, pivotItem.text);
    }
    break;
  }
  case NXPivotType::TextUnderIcon : {
    if (hasIcon && hasText) {
      _drawTextUnderIcon(painter, contentRect, pivotItem.icon, pivotItem.text);
    } else if (hasIcon) {
      _drawIconOnly(painter, contentRect, pivotItem.icon);
    } else if (hasText) {
      _drawTextOnly(painter, contentRect, pivotItem.text);
    }
    break;
  }
  case NXPivotType::IconWithOverlayText : {
    _drawIconWithOverlayText(painter, contentRect, pivotItem.icon, pivotItem.text);
    break;
  }
  default : {
    if (hasText) {
      _drawTextOnly(painter, contentRect, pivotItem.text);
    }
    break;
  }
  }
}

void NXPivotStyle::_drawFocusRectBackground(QPainter *painter, QRect const &rect, QColor const &color) const
{
  painter->save();

  painter->setBrush(color);

  QPainterPath path;
  path.moveTo(rect.left() + RADIUS, rect.top());
  path.lineTo(rect.right() - RADIUS, rect.top());
  path.arcTo(QRectF(rect.right() - RADIUS * 2, rect.top(), RADIUS * 2, RADIUS * 2), 90, -90);
  path.lineTo(rect.right(), rect.bottom());
  path.lineTo(rect.left(), rect.bottom());
  path.lineTo(rect.left(), rect.top() + RADIUS);
  path.arcTo(QRectF(rect.left(), rect.top(), RADIUS * 2, RADIUS * 2), 180, -90);
  path.closeSubpath();
  painter->drawPath(path);

  painter->restore();
}

void NXPivotStyle::_drawIndicator(QPainter *painter, QRect const &rect, QColor const &color) const
{
  painter->save();

  int const triangleSide = rect.width() / 16 * 5;
  painter->setBrush(color);

  QPainterPath path;
  QPoint startTop(rect.left() + RADIUS, rect.top());
  QPoint endTop(rect.left() + triangleSide, rect.top());
  QPoint endLeft(rect.left(), rect.top() + triangleSide);
  QPoint startLeft(rect.left(), rect.top() + RADIUS);

  path.moveTo(startTop);
  path.lineTo(endTop);
  path.lineTo(endLeft);
  path.lineTo(startLeft);
  path.arcTo(QRectF(rect.left(), rect.top(), RADIUS * 2, RADIUS * 2), 180, -90);
  path.closeSubpath();
  painter->drawPath(path);

  painter->setPen(Qt::white);
  QFont iconFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(13);
  iconFont.setBold(true);
  painter->setFont(iconFont);

  // 三角形重心（直角三角形直角在左上角，重心为 (side/3, side/3)）
  QPointF center(rect.left() + triangleSide / 3.0, rect.top() + triangleSide / 3.0);
  QFontMetricsF fm(iconFont);
  QRectF textRect(center.x() - fm.height() / 2, center.y() - fm.height() / 2, fm.height(), fm.height());
  painter->drawText(textRect, Qt::AlignCenter, QChar(static_cast<unsigned short>(NXIconType::Check)));

  painter->restore();
}

void NXPivotStyle::_alignHeight(NXPivotView const *pivotView, int actualHeight) const
{
  if (m_IsAutoAdaptiveItemHeight && pivotView->height() != actualHeight) {
    pivotView->parentWidget()->setFixedHeight(actualHeight);
  }
}

QSize NXPivotStyle::_calculateItemSize(NXPivotModel::PivotItem const &pivotItem, QFontMetrics const &metrics, bool hasIcon, bool hasText) const
{
  switch (m_DisplayMode) {
  case NXPivotType::IconOnly : {
    QSize const iconSize = hasIcon ? pivotItem.icon.size() : QSize {};
    int width            = iconSize.width() + m_ItemHorizontalSpacing;
    int height           = iconSize.height() + m_ItemVerticalSpacing;
    return { width, height };
  }
  case NXPivotType::TextOnly : {
    int width  = (hasText ? metrics.horizontalAdvance(pivotItem.text) : 0) + m_ItemHorizontalSpacing;
    int height = (hasText ? metrics.height() : 0) + m_ItemVerticalSpacing;
    return { width, height };
  }
  case NXPivotType::TextBesideIcon : {
    QSize const iconSize = hasIcon ? pivotItem.icon.size() : QSize {};
    int iconWidth        = iconSize.width() + m_ItemHorizontalSpacing;
    int iconHeight       = iconSize.height() + m_ItemVerticalSpacing;
    int textWidth        = (hasText ? metrics.horizontalAdvance(pivotItem.text) : 0) + m_ItemHorizontalSpacing;
    int textHeight       = (hasText ? metrics.height() : 0) + m_ItemVerticalSpacing;
    int actualHeight     = qMax(textHeight, iconHeight);
    int totalWidth       = textWidth + iconWidth + m_TextIconSpacing;
    return { totalWidth, actualHeight };
  }
  case NXPivotType::TextUnderIcon : {
    QSize const iconSize = hasIcon ? pivotItem.icon.size() : QSize {};
    int iconWidth        = iconSize.width() + m_ItemHorizontalSpacing;
    int iconHeight       = iconSize.height() + m_ItemVerticalSpacing;
    int textWidth        = (hasText ? metrics.horizontalAdvance(pivotItem.text) : 0) + m_ItemHorizontalSpacing;
    int textHeight       = (hasText ? metrics.height() : 0) + m_ItemVerticalSpacing;
    int actualHeight     = textHeight + iconHeight + m_TextIconSpacing;
    int totalWidth       = qMax(textWidth, iconWidth);
    return { totalWidth, actualHeight };
  }
  case NXPivotType::IconWithOverlayText : {
    QSize const iconSize = hasIcon ? pivotItem.icon.size() : QSize {};
    int width            = iconSize.width() + m_ItemHorizontalSpacing;
    int height           = iconSize.height() + m_ItemVerticalSpacing;
    return { width, height };
  }
  default : Q_UNREACHABLE();
  }
}
