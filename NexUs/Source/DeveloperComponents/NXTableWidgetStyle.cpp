#include "NXTableWidgetStyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionViewItem>

#include "NXTableWidget.h"
#include "NXTheme.h"

NXTableWidgetStyle::NXTableWidgetStyle(QStyle *style)
    : m_ItemHeight { 35 }
    , m_HeaderMargin { 6 }
    , m_CurrentHoverRow { -1 }
    , m_IsTransparent { false }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXTableWidgetStyle::~NXTableWidgetStyle()
{
}

void NXTableWidgetStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::PE_PanelItemViewItem : {
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      NXTableWidget const *tableWidget = dynamic_cast<NXTableWidget const *>(widget);
      if (!tableWidget) {
        return;
      }

      QVariant backgroundData  = vopt->index.data(Qt::BackgroundRole);
      bool hasCustomBackground = backgroundData.isValid();

      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setPen(Qt::NoPen);

      if (hasCustomBackground) {
        QBrush customBrush = backgroundData.value<QBrush>();
        painter->setBrush(customBrush);
        painter->drawRect(vopt->rect);
      }

      QAbstractItemView::SelectionBehavior selectionBehavior = tableWidget->selectionBehavior();
      if (selectionBehavior == QAbstractItemView::SelectRows) {
        if (vopt->index.row() == m_CurrentHoverRow && !hasCustomBackground) {
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicHoverAlpha));
          painter->drawRect(vopt->rect);
        }
      } else {
        if ((vopt->state.testFlag(QStyle::State_Selected) || vopt->state.testFlag(QStyle::State_MouseOver)) &&
            !hasCustomBackground) {
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicHoverAlpha));
          painter->drawRect(vopt->rect);
        }
      }
      painter->restore();
    }
    return;
  }
  case QStyle::PE_PanelItemViewRow : {
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      QVariant backgroundData  = vopt->index.data(Qt::BackgroundRole);
      bool hasCustomBackground = backgroundData.isValid();

      if (hasCustomBackground) {
        return;
      }

      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      QRect itemRect = vopt->rect;
      painter->setPen(Qt::NoPen);

      if (vopt->state & QStyle::State_Selected) {
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicSelectedAlpha));
        painter->drawRect(itemRect);
      } else {
        if (vopt->features.testFlag(QStyleOptionViewItem::Alternate)) {
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicAlternating));
          painter->drawRect(vopt->rect);
        }
      }
      painter->restore();
    }
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

void NXTableWidgetStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    if (!m_IsTransparent) {
      QRect frameRect = option->rect;
      frameRect.adjust(1, 1, -1, -1);
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing);
      painter->setPen(NXThemeColor(m_ThemeMode, PopupBorder));
      painter->setBrush(NXThemeColor(m_ThemeMode, BasicBaseAlpha));
      painter->drawRoundedRect(frameRect, 3, 3);
      painter->restore();
    }
    return;
  }
  case QStyle::CE_HeaderLabel : {
    if (QStyleOptionHeader const *hopt = qstyleoption_cast<QStyleOptionHeader const *>(option)) {
      QRect headerRect = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
      if (!hopt->text.isEmpty()) {
        painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
        painter->drawText(headerRect, hopt->textAlignment, hopt->text);
      }
      painter->restore();
    }
    return;
  }
  case QStyle::CE_HeaderSection : {
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(m_ThemeMode, BasicBaseDeepAlpha));
    painter->drawRect(option->rect);
    if (option->state.testFlag(QStyle::State_Sunken)) {
      painter->setBrush(NXThemeColor(m_ThemeMode, BasicPressAlpha));
    } else {
      if (option->state.testFlag(QStyle::State_MouseOver)) {
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicHoverAlpha));
      }
    }
    painter->drawRect(option->rect);
    painter->restore();
    return;
  }
  case QStyle::CE_HeaderEmptyArea : {
    QRect frameRect = option->rect;
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(m_ThemeMode, BasicBaseDeep));
    painter->drawRect(frameRect);
    painter->restore();
    return;
  }
  case QStyle::CE_ItemViewItem : {
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      NXTableWidget const *tableWidget = dynamic_cast<NXTableWidget const *>(widget);
      if (!tableWidget) {
        return;
      }
      this->drawPrimitive(QStyle::PE_PanelItemViewItem, option, painter, widget);
      QAbstractItemView::SelectionBehavior selectionBehavior = tableWidget->selectionBehavior();
      QRect itemRect                                         = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
      QRect iconRect = proxy()->subElementRect(SE_ItemViewItemDecoration, vopt, widget);
      QRect textRect = proxy()->subElementRect(SE_ItemViewItemText, vopt, widget);
      if (vopt->index.column() == 0) {
        iconRect.adjust(m_HorizontalPadding, 0, m_HorizontalPadding, 0);
        textRect.adjust(m_HorizontalPadding, 0, 0, 0);
      }
      if (!vopt->icon.isNull()) {
        QIcon::Mode mode   = QIcon::Normal;
        QIcon::State state = vopt->state & QStyle::State_Open ? QIcon::On : QIcon::Off;
        vopt->icon.paint(painter, iconRect, vopt->decorationAlignment, mode, state);
      }
      if (!vopt->text.isEmpty()) {
        QVariant foregroundData = vopt->index.data(Qt::ForegroundRole);
        if (foregroundData.isValid()) {
          QBrush foregroundBrush = foregroundData.value<QBrush>();
          painter->setPen(foregroundBrush.color());
        } else {
          painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
        }
        painter->drawText(textRect, vopt->displayAlignment, vopt->text);
      }
      int heightOffset = itemRect.height() / 4;
      painter->setPen(Qt::NoPen);
      painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
      if (vopt->state.testFlag(QStyle::State_Selected)) {
        if (selectionBehavior == QAbstractItemView::SelectRows && vopt->index.column() == 0) {
          painter->drawRoundedRect(
              QRectF(itemRect.x() + 3, itemRect.y() + heightOffset, 3, itemRect.height() - 2 * heightOffset), 3, 3);
        }
      }
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

QSize NXTableWidgetStyle::sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const
{
  switch (type) {
  case QStyle::CT_ItemViewItem : {
    QSize itemSize = QProxyStyle::sizeFromContents(type, option, size, widget);
    itemSize.setHeight(m_ItemHeight);
    return itemSize;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}

int NXTableWidgetStyle::pixelMetric(PixelMetric metric, QStyleOption const *option, QWidget const *widget) const
{
  switch (metric) {
  case QStyle::PM_HeaderMargin : {
    return m_HeaderMargin;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}
