#include "NXVirtualListStyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include "NXTheme.h"
#include "NXVirtualList.h"

NXVirtualListStyle::NXVirtualListStyle(QStyle *style)
    : m_ItemHeight { 28 }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXVirtualListStyle::~NXVirtualListStyle()
{
}

void NXVirtualListStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::PE_PanelItemViewItem : {
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      QRect itemRect = vopt->rect;
      itemRect.adjust(0, 1, 0, -1);
      QPainterPath path;
      path.addRoundedRect(itemRect, 4, 4);
      if (vopt->state & QStyle::State_Selected) {
        if (vopt->state & QStyle::State_MouseOver) {
          painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicSelectedHoverAlpha));
        } else {
          painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicSelectedAlpha));
        }
      } else {
        if (vopt->state & QStyle::State_MouseOver) {
          painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicHoverAlpha));
        }
      }
      painter->restore();
    }
    return;
  }
  case QStyle::PE_PanelItemViewRow : {
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      if (vopt->features == QStyleOptionViewItem::Alternate) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicAlternating));
        painter->drawRect(vopt->rect);
        painter->restore();
      }
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

void NXVirtualListStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    return;
  }
  case QStyle::CE_ItemViewItem : {
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      this->drawPrimitive(QStyle::PE_PanelItemViewItem, option, painter, widget);

      QRect itemRect = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

      QRect iconRect = proxy()->subElementRect(SE_ItemViewItemDecoration, vopt, widget);
      QRect textRect = proxy()->subElementRect(SE_ItemViewItemText, vopt, widget);
      iconRect.adjust(m_LeftPadding, 0, 0, 0);
      textRect.adjust(m_LeftPadding, 0, 0, 0);

      if (!vopt->icon.isNull()) {
        QIcon::Mode mode = QIcon::Normal;
        if (!(vopt->state.testFlag(QStyle::State_Enabled))) {
          mode = QIcon::Disabled;
        } else if (vopt->state.testFlag(QStyle::State_Selected)) {
          mode = QIcon::Selected;
        }
        QIcon::State state = vopt->state & QStyle::State_Open ? QIcon::On : QIcon::Off;
        vopt->icon.paint(painter, iconRect, vopt->decorationAlignment, mode, state);
      }

      if (!vopt->text.isEmpty()) {
        painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
        painter->drawText(textRect, vopt->displayAlignment, vopt->text);
      }

      if (vopt->state.testFlag(QStyle::State_Selected)) {
        int heightOffset = itemRect.height() / 4;
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
        painter->drawRoundedRect(
            QRectF(itemRect.x() + 3, itemRect.y() + heightOffset, 3, itemRect.height() - 2 * heightOffset), 3, 3);
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

QSize NXVirtualListStyle::sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const
{
  switch (type) {
  case QStyle::CT_ItemViewItem : {
    QSize itemSize = QProxyStyle::sizeFromContents(type, option, size, widget);
    itemSize.setWidth(itemSize.width() + m_LeftPadding);
    itemSize.setHeight(m_ItemHeight);
    return itemSize;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}
