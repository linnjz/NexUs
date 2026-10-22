#include "NXVirtualListStyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include "NXTheme.h"
#include "NXVirtualList.h"

NXVirtualListStyle::NXVirtualListStyle(QStyle *style)
{
  _pItemHeight = 28;
  _themeMode   = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXVirtualListStyle::~NXVirtualListStyle()
{
}

void NXVirtualListStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  switch (element) {
  case QStyle::PE_PanelItemViewItem : {
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      QRect itemRect = vopt->rect;
      itemRect.adjust(0, 1, 0, -1);
      QPainterPath path;
      path.addRoundedRect(itemRect, 4, 4);
      if (vopt->state & QStyle::State_Selected) {
        if (vopt->state & QStyle::State_MouseOver) {
          painter->fillPath(path, NXThemeColor(_themeMode, BasicSelectedHoverAlpha));
        } else {
          painter->fillPath(path, NXThemeColor(_themeMode, BasicSelectedAlpha));
        }
      } else {
        if (vopt->state & QStyle::State_MouseOver) {
          painter->fillPath(path, NXThemeColor(_themeMode, BasicHoverAlpha));
        }
      }
      painter->restore();
    }
    return;
  }
  case QStyle::PE_PanelItemViewRow : {
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      if (vopt->features == QStyleOptionViewItem::Alternate) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(_themeMode, BasicAlternating));
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

void NXVirtualListStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    return;
  }
  case QStyle::CE_ItemViewItem : {
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      this->drawPrimitive(QStyle::PE_PanelItemViewItem, option, painter, widget);

      QRect itemRect = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

      QRect iconRect = proxy()->subElementRect(SE_ItemViewItemDecoration, vopt, widget);
      QRect textRect = proxy()->subElementRect(SE_ItemViewItemText, vopt, widget);
      iconRect.adjust(_leftPadding, 0, 0, 0);
      textRect.adjust(_leftPadding, 0, 0, 0);

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
        painter->setPen(NXThemeColor(_themeMode, BasicText));
        painter->drawText(textRect, vopt->displayAlignment, vopt->text);
      }

      if (vopt->state.testFlag(QStyle::State_Selected)) {
        int heightOffset = itemRect.height() / 4;
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(_themeMode, PrimaryNormal));
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

QSize NXVirtualListStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
  switch (type) {
  case QStyle::CT_ItemViewItem : {
    QSize itemSize = QProxyStyle::sizeFromContents(type, option, size, widget);
    itemSize.setWidth(itemSize.width() + _leftPadding);
    itemSize.setHeight(_pItemHeight);
    return itemSize;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}
