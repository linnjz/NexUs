#include "NXListViewStyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include "NXListView.h"
#include "NXTheme.h"

NXListViewStyle::NXListViewStyle(QStyle *style)
    : m_ItemHeight { 35 }
    , m_IsTransparent { true }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXListViewStyle::~NXListViewStyle()
{
}

void NXListViewStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::PE_PanelItemViewItem : {
    // Item背景
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      QRect itemRect = vopt->rect;
      itemRect.adjust(0, 2, 0, -2);
      QPainterPath path;
      path.addRoundedRect(itemRect, 4, 4);
      if (vopt->state & QStyle::State_Selected) {
        if (vopt->state & QStyle::State_MouseOver) {
          // 选中时覆盖
          painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicSelectedHoverAlpha));
        } else {
          // 选中
          painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicSelectedAlpha));
        }
      } else {
        if (vopt->state & QStyle::State_MouseOver) {
          // 覆盖时颜色
          painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicHoverAlpha));
        }
      }
      painter->restore();
    }
    return;
  }
  case QStyle::PE_PanelItemViewRow : {
    // Item背景隔行变色
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

void NXListViewStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    // viewport视口外的其他区域背景
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);
    painter->setPen(NXThemeColor(m_ThemeMode, PopupBorder));
    painter->setBrush(m_IsTransparent ? NXThemeColor(m_ThemeMode, BasicBaseAlpha) : QColor(0xF3, 0xF4, 0xF5));
    painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), 3, 3);
    painter->restore();
    return;
  }
  case QStyle::CE_ItemViewItem : {
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      // 背景绘制
      this->drawPrimitive(QStyle::PE_PanelItemViewItem, option, painter, widget);

      // 内容绘制
      QRect itemRect = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
      NXListView const *listView   = dynamic_cast<NXListView const *>(widget);
      QListView::ViewMode viewMode = listView->viewMode();
      // QRect checkRect = proxy()->subElementRect(SE_ItemViewItemCheckIndicator, vopt, widget);
      QRect iconRect = proxy()->subElementRect(SE_ItemViewItemDecoration, vopt, widget);
      QRect textRect = proxy()->subElementRect(SE_ItemViewItemText, vopt, widget);
      iconRect.adjust(m_LeftPadding, 0, 0, 0);
      textRect.adjust(m_LeftPadding, 0, 0, 0);
      // 图标绘制
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
      // 文字绘制
      if (!vopt->text.isEmpty()) {
        painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
        painter->drawText(textRect, vopt->displayAlignment, vopt->text);
      }
      // 选中特效
      if (vopt->state.testFlag(QStyle::State_Selected) && viewMode == QListView::ListMode) {
        int heightOffset = itemRect.height() / 4;
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
        painter->drawRoundedRect(
            QRectF(itemRect.x() + 3, itemRect.y() + heightOffset, 3, itemRect.height() - 2 * heightOffset), 1.5, 1.5);
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

QSize NXListViewStyle::sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const
{
  switch (type) {
  case QStyle::CT_ItemViewItem : {
    QSize itemSize               = QProxyStyle::sizeFromContents(type, option, size, widget);
    NXListView const *listView   = dynamic_cast<NXListView const *>(widget);
    QListView::ViewMode viewMode = listView->viewMode();
    if (viewMode == QListView::ListMode) {
      itemSize.setWidth(itemSize.width() + m_LeftPadding);
    }
    itemSize.setHeight(m_ItemHeight);
    return itemSize;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}
