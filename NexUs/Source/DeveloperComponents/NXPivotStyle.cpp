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

QSize LogicalPixmapSize(const QPixmap &pixmap)
{
  const qreal dpr  = qMax(1.0, pixmap.devicePixelRatioF());
  const int width  = qMax(1, qRound(static_cast<qreal>(pixmap.width()) / dpr));
  const int height = qMax(1, qRound(static_cast<qreal>(pixmap.height()) / dpr));
  return QSize(width, height);
}
} // namespace

NXPivotStyle::NXPivotStyle(QStyle *style)
{
  _themeMode            = nxTheme->getThemeMode();
  _pTextNormalColor     = NXThemeColor(_themeMode, BasicTextNoFocus);
  _pTextFocusColor      = NXThemeColor(_themeMode, BasicText);
  _pTextPressedColor    = NXThemeColor(_themeMode, BasicTextPress);
  _pIndicatorColor      = NXThemeColor(_themeMode, PrimaryNormal);
  _pItemBackgroundColor = Qt::transparent;
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXPivotStyle::~NXPivotStyle()
{
}

void NXPivotStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
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

void NXPivotStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    // viewport视口外的其他区域背景
    return;
  }
  case QStyle::CE_ItemViewItem : {
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      const auto *pivotView = qobject_cast<const NXPivotView *>(widget);
      if (!pivotView) {
        break;
      }
      const QModelIndex &pressIndex   = pivotView->getPressIndex();
      const QModelIndex &currentIndex = pivotView->getCommittedIndex();
      const auto pivotItem            = vopt->index.data(Qt::UserRole).value<NXPivotModel::PivotItem>();
      const bool isHover              = vopt->state.testFlag(QStyle::State_MouseOver);
      const bool isSelected           = currentIndex.row() == vopt->index.row();

      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

      QRect focusRect = proxy()->subElementRect(SE_ItemViewItemFocusRect, vopt, widget);

      painter->setPen(Qt::NoPen);
      if (isSelected && _pItemBackgroundColor != Qt::transparent) {
        _drawFocusRectBackground(painter, focusRect, _pItemBackgroundColor);
        if (_pIndicatorTypeFlags.testFlag(NXPivotType::IndicatorCheck)) {
          _drawIndicator(painter, focusRect, _pIndicatorColor);
        }
      } else if (isHover && _pIsHoverItemBackgroundEnable) {
        _drawFocusRectBackground(painter, focusRect, NXThemeColor(_themeMode, BasicBaseDeep));
      }

      QColor contentColor = _pTextNormalColor;
      if (isSelected || isHover) {
        contentColor = _pTextFocusColor;
      }
      if (pressIndex.row() == vopt->index.row()) {
        contentColor = _pTextPressedColor;
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

int NXPivotStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
  //switch (metric)
  //{
  //case QStyle::PM_FocusFrameHMargin :
  //{
  //  return _pItemHorizontalSpacing;
  //}
  //default :
  //{
  //  break;
  //}
  //}

  return QProxyStyle::pixelMetric(metric, option, widget);
}

QRect NXPivotStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
  if (element == SE_ItemViewItemFocusRect) {
    QRect rect = option->rect;
    rect.setHeight(widget->height());
    return rect;
  }
  return QProxyStyle::subElementRect(element, option, widget);
}

QSize NXPivotStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
  auto pivotView = qobject_cast<const NXPivotView *>(widget);
  if (!pivotView || type != QStyle::CT_ItemViewItem) {
    return QProxyStyle::sizeFromContents(type, option, size, widget);
  }

  if (_pIsItemFixedSize) {
    int actualHeight = _pItemFixedSize.height();
    _alignHeight(pivotView, actualHeight);
    return _pItemFixedSize;
  }

  const auto *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option);
  if (!vopt) {
    return QProxyStyle::sizeFromContents(type, option, size, widget);
  }
  const auto pivotItem = vopt->index.data(Qt::UserRole).value<NXPivotModel::PivotItem>();
  bool hasIcon         = !pivotItem.icon.isNull();
  bool hasText         = !pivotItem.text.isEmpty();
  if (!hasIcon && !hasText) {
    return QSize {};
  }
  const QFontMetrics metrics(pivotView->font());
  QSize computedSize = _calculateItemSize(pivotItem, metrics, hasIcon, hasText);
  _alignHeight(pivotView, computedSize.height());

  return computedSize;
}

void NXPivotStyle::_drawCenteredPixmap(QPainter *painter, const QRect &containerRect, const QPixmap &pixmap) const
{
  const QSize drawSize = LogicalPixmapSize(pixmap);
  const int drawWidth  = drawSize.width();
  const int drawHeight = drawSize.height();
  const int x          = containerRect.x() + (containerRect.width() - drawWidth) / 2;
  const int y          = containerRect.y() + (containerRect.height() - drawHeight) / 2;

  painter->drawPixmap(QPoint { x, y }, pixmap);
}

void NXPivotStyle::_drawTextOnly(QPainter *painter, const QRect &contentRect, const QString &text) const
{
  const QFontMetrics metrics(painter->font());
  const QString drawText = metrics.elidedText(text, Qt::ElideRight, contentRect.width());
  painter->drawText(contentRect.toRectF().adjusted(0, 0, -3.5, -3.5), Qt::AlignCenter, drawText);
}

void NXPivotStyle::_drawIconOnly(QPainter *painter, const QRect &contentRect, const QPixmap &icon) const
{
  _drawCenteredPixmap(painter, contentRect, icon);
}

void NXPivotStyle::_drawTextBesideIcon(QPainter *painter, const QRect &contentRect, const QPixmap &icon, const QString &text) const
{
  const QFontMetrics metrics(painter->font());
  const QSize iconDrawSize = LogicalPixmapSize(icon);
  const int textMaxWidth   = qMax(0, contentRect.width() - iconDrawSize.width() - _pTextIconSpacing);
  const QString drawText   = metrics.elidedText(text, Qt::ElideRight, textMaxWidth);
  const int textWidth      = qMin(textMaxWidth, metrics.horizontalAdvance(drawText));
  const int groupWidth     = iconDrawSize.width() + _pTextIconSpacing + textWidth;
  const int startX         = contentRect.x() + qMax(0, (contentRect.width() - groupWidth) / 2);

  const QRect iconRect(startX, contentRect.y() + (contentRect.height() - iconDrawSize.height()) / 2, iconDrawSize.width(), iconDrawSize.height());
  _drawCenteredPixmap(painter, iconRect, icon);

  const int textStartX     = iconRect.right() + 1 + _pTextIconSpacing;
  const int textWidthLimit = qMax(0, contentRect.right() - textStartX + 1);
  const QRect textRect(textStartX, contentRect.y(), textWidthLimit, contentRect.height());
  painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, drawText);
}

void NXPivotStyle::_drawTextUnderIcon(QPainter *painter, const QRect &contentRect, const QPixmap &icon, const QString &text) const
{
  const QFontMetrics metrics(painter->font());
  const int textHeight     = metrics.height();
  const QSize iconDrawSize = LogicalPixmapSize(icon);
  const int totalHeight    = iconDrawSize.height() + _pTextIconSpacing + textHeight;
  const int startY         = contentRect.y() + qMax(0, (contentRect.height() - totalHeight) / 2);

  const QRect iconRect(contentRect.x() + qMax(0, (contentRect.width() - iconDrawSize.width()) / 2), startY, iconDrawSize.width(), iconDrawSize.height());
  _drawCenteredPixmap(painter, iconRect, icon);

  const QRect textRect(contentRect.x(), startY + iconDrawSize.height() + _pTextIconSpacing, contentRect.width(), textHeight);
  const QString drawText = metrics.elidedText(text, Qt::ElideRight, textRect.width());
  painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter, drawText);
}

void NXPivotStyle::_drawIconWithOverlayText(QPainter *painter, const QRect &contentRect, const QPixmap &icon, const QString &text) const
{
  _drawCenteredPixmap(painter, contentRect, icon);

  const QFontMetrics metrics(painter->font());
  const int textWidthLimit = qMax(1, contentRect.width());
  const QString drawText   = metrics.elidedText(text, Qt::ElideRight, textWidthLimit);
  const int textWidth      = qMax(1, qMin(textWidthLimit, metrics.horizontalAdvance(drawText)));
  const int textHeight     = metrics.height();

  QPoint center = contentRect.center();
  center += _pOverlayTextOffsetFromCenterAnchor;

  QRect overlayRect(center.x() - textWidth / 2, center.y() - textHeight / 2, textWidth, textHeight);

  const int maxX = contentRect.right() - overlayRect.width() + 1;
  const int maxY = contentRect.bottom() - overlayRect.height() + 1;
  overlayRect.moveLeft(qBound(contentRect.left(), overlayRect.left(), maxX));
  overlayRect.moveTop(qBound(contentRect.top(), overlayRect.top(), maxY));

  painter->drawText(overlayRect, Qt::AlignLeft | Qt::AlignVCenter, drawText);
}

void NXPivotStyle::_drawDisplayContent(QPainter *painter, const QRect &contentRect, const NXPivotModel::PivotItem &pivotItem) const
{
  const bool hasText = !pivotItem.text.isEmpty();
  const bool hasIcon = !pivotItem.icon.isNull();

  switch (_pDisplayMode) {
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

void NXPivotStyle::_drawFocusRectBackground(QPainter *painter, const QRect &rect, const QColor &color) const
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

void NXPivotStyle::_drawIndicator(QPainter *painter, const QRect &rect, const QColor &color) const
{
  painter->save();

  const int triangleSide = rect.width() / 16 * 5;
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

void NXPivotStyle::_alignHeight(const NXPivotView *pivotView, int actualHeight) const
{
  if (_pIsAutoAdaptiveItemHeight && pivotView->height() != actualHeight) {
    pivotView->parentWidget()->setFixedHeight(actualHeight);
  }
}

QSize NXPivotStyle::_calculateItemSize(const NXPivotModel::PivotItem &pivotItem, const QFontMetrics &metrics, bool hasIcon, bool hasText) const
{
  switch (_pDisplayMode) {
  case NXPivotType::IconOnly : {
    const QSize iconSize = hasIcon ? pivotItem.icon.size() : QSize {};
    int width            = iconSize.width() + _pItemHorizontalSpacing;
    int height           = iconSize.height() + _pItemVerticalSpacing;
    return { width, height };
  }
  case NXPivotType::TextOnly : {
    int width  = (hasText ? metrics.horizontalAdvance(pivotItem.text) : 0) + _pItemHorizontalSpacing;
    int height = (hasText ? metrics.height() : 0) + _pItemVerticalSpacing;
    return { width, height };
  }
  case NXPivotType::TextBesideIcon : {
    const QSize iconSize = hasIcon ? pivotItem.icon.size() : QSize {};
    int iconWidth        = iconSize.width() + _pItemHorizontalSpacing;
    int iconHeight       = iconSize.height() + _pItemVerticalSpacing;
    int textWidth        = (hasText ? metrics.horizontalAdvance(pivotItem.text) : 0) + _pItemHorizontalSpacing;
    int textHeight       = (hasText ? metrics.height() : 0) + _pItemVerticalSpacing;
    int actualHeight     = qMax(textHeight, iconHeight);
    int totalWidth       = textWidth + iconWidth + _pTextIconSpacing;
    return { totalWidth, actualHeight };
  }
  case NXPivotType::TextUnderIcon : {
    const QSize iconSize = hasIcon ? pivotItem.icon.size() : QSize {};
    int iconWidth        = iconSize.width() + _pItemHorizontalSpacing;
    int iconHeight       = iconSize.height() + _pItemVerticalSpacing;
    int textWidth        = (hasText ? metrics.horizontalAdvance(pivotItem.text) : 0) + _pItemHorizontalSpacing;
    int textHeight       = (hasText ? metrics.height() : 0) + _pItemVerticalSpacing;
    int actualHeight     = textHeight + iconHeight + _pTextIconSpacing;
    int totalWidth       = qMax(textWidth, iconWidth);
    return { totalWidth, actualHeight };
  }
  case NXPivotType::IconWithOverlayText : {
    const QSize iconSize = hasIcon ? pivotItem.icon.size() : QSize {};
    int width            = iconSize.width() + _pItemHorizontalSpacing;
    int height           = iconSize.height() + _pItemVerticalSpacing;
    return { width, height };
  }
  default : Q_UNREACHABLE();
  }
}
