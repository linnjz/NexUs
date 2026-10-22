#include "NXTreeViewStyle.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include "NXApplication.h"
#include "NXTheme.h"

NXTreeViewStyle::NXTreeViewStyle(QStyle *style)
{
  _pItemHeight   = 35;
  _pHeaderMargin = 5;
  _pIconName     = NXIconType::None;
  _themeMode     = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXTreeViewStyle::~NXTreeViewStyle()
{
}

void NXTreeViewStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  switch (element) {
  case QStyle::PE_PanelItemViewItem : {
    // Item背景
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      QRect itemRect = vopt->rect;
      itemRect.adjust(0, 2, 0, -2);
      QPainterPath path;
      path.addRoundedRect(itemRect, 4, 4);
      bool isEnable = vopt->state.testFlag(QStyle::State_Enabled);
      if (vopt->state & QStyle::State_Selected) {
        if (vopt->state & QStyle::State_MouseOver && isEnable) {
          // 选中时覆盖
          painter->fillPath(path, NXThemeColor(_themeMode, BasicSelectedHoverAlpha));
        } else {
          // 选中
          painter->fillPath(path, NXThemeColor(_themeMode, BasicSelectedAlpha));
        }
      } else {
        if (vopt->state & QStyle::State_MouseOver && isEnable) {
          // 覆盖时颜色
          painter->fillPath(path, NXThemeColor(_themeMode, BasicHoverAlpha));
        }
      }
      painter->restore();
    }
    return;
  }
  case QStyle::PE_IndicatorBranch : {
    // 展开图标绘制
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      if (vopt->state.testFlag(QStyle::State_Children)) {
        painter->save();
        painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        QRect indicatorRect = option->rect;
        indicatorRect.adjust(0, 0, -2, 0);
        QFont iconFont = QFont(QStringLiteral("NXAwesome"));
        iconFont.setPixelSize(nxApp->getFontPixelSize() + 1);
        painter->setFont(iconFont);
        painter->setPen(NXThemeColor(_themeMode, BasicText));
        painter->drawText(indicatorRect, Qt::AlignVCenter | Qt::AlignRight, vopt->state.testFlag(QStyle::State_Open) ? QChar((unsigned short) NXIconType::AngleDown) : QChar((unsigned short) NXIconType::AngleRight));
        painter->restore();
      }
    }
    return;
  }
  case QStyle::PE_IndicatorItemViewItemDrop : {
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter->setPen(NXThemeColor(_themeMode, BasicText));
    painter->setBrush(Qt::NoBrush);
    QRect optionRect = option->rect;
    if (optionRect.height() == 0) {
      painter->drawLine(optionRect.topLeft(), optionRect.topRight());
    } else {
      painter->drawRect(optionRect);
    }
    painter->restore();
    return;
  }
  case QStyle::PE_PanelItemViewRow : {
    return;
  }
  case QStyle::PE_Widget : {
    return;
  }
  case QStyle::PE_Frame : {
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void NXTreeViewStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    // viewport视口外的其他区域背景
    QRect frameRect = option->rect;
    frameRect.adjust(1, 1, -1, -1);
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);
    painter->setPen(NXThemeColor(_themeMode, PopupBorder));
    painter->setBrush(NXThemeColor(_themeMode, BasicBaseAlpha));
    painter->drawRoundedRect(frameRect, 3, 3);
    painter->restore();
    return;
  }
  case QStyle::CE_HeaderLabel : {
    // 表头文字绘制
    if (const QStyleOptionHeader *hopt = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
      QRect headerRect = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
      if (!hopt->text.isEmpty()) {
        painter->setPen(NXThemeColor(_themeMode, BasicText));
        painter->drawText(headerRect, hopt->textAlignment, hopt->text);
      }
      painter->restore();
    }
    return;
  }
  case QStyle::CE_HeaderSection : {
    // 表头背景绘制
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(_themeMode, BasicBaseDeepAlpha));
    painter->drawRect(option->rect);
    painter->restore();
    return;
  }
  case QStyle::CE_ItemViewItem : {
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      // 背景绘制
      this->drawPrimitive(QStyle::PE_PanelItemViewItem, option, painter, widget);

      // 内容绘制
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

      // 复选框绘制
      if (vopt->features & QStyleOptionViewItem::HasCheckIndicator) {
        QRect checkRect = proxy()->subElementRect(SE_ItemViewItemCheckIndicator, vopt, widget);
        painter->save();
        //图标绘制
        if (vopt->checkState == Qt::Checked) {
          painter->setPen(Qt::NoPen);
          painter->setBrush(NXThemeColor(_themeMode, PrimaryNormal));
          painter->drawRoundedRect(checkRect, 2, 2);
          QFont iconFont = QFont(QStringLiteral("NXAwesome"));
          iconFont.setPixelSize(checkRect.width() * 0.85);
          painter->setFont(iconFont);
          painter->setPen(NXThemeColor(NXThemeType::Dark, BasicText));
          painter->drawText(checkRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Check));
        } else if (vopt->checkState == Qt::PartiallyChecked) {
          painter->setPen(Qt::NoPen);
          painter->setBrush(NXThemeColor(_themeMode, PrimaryNormal));
          painter->drawRoundedRect(checkRect, 2, 2);
          painter->setPen(NXThemeColor(NXThemeType::Dark, BasicText));
          QLine checkLine(checkRect.x() + 3, checkRect.center().y(), checkRect.right() - 3, checkRect.center().y());
          painter->drawLine(checkLine);
        } else {
          painter->setPen(QPen(NXThemeColor(_themeMode, BasicBorderDeep), 1.2));
          painter->setBrush(Qt::transparent);
          painter->drawRoundedRect(checkRect, 2, 2);
        }
        painter->restore();
      }
      // 图标绘制
      if (!vopt->icon.isNull()) {
        QRect iconRect     = proxy()->subElementRect(SE_ItemViewItemDecoration, vopt, widget);
        QIcon::Mode mode   = QIcon::Normal;
        QIcon::State state = vopt->state & QStyle::State_Open ? QIcon::On : QIcon::Off;
        vopt->icon.paint(painter, iconRect, vopt->decorationAlignment, mode, state);
      }

      QRect itemRect = option->rect;
      bool isFirst =
          vopt->state.testFlag(QStyle::State_Selected) && (vopt->viewItemPosition == QStyleOptionViewItem::Beginning ||
                                                           vopt->viewItemPosition == QStyleOptionViewItem::OnlyOne);
      // 文字绘制
      if (!vopt->text.isEmpty()) {
        QRect textRect = proxy()->subElementRect(SE_ItemViewItemText, vopt, widget);
        if (_pIconName != NXIconType::None) {
          const int lineHeight = textRect.height();
          const int iconSize   = qMax(16, static_cast<int>(lineHeight * 0.55));
          QRect iconRect(vopt->icon.isNull() ? itemRect.left() + 10 : textRect.left(),
                         textRect.top() + (lineHeight - iconSize) / 2, // 垂直居中
                         iconSize, iconSize);
          QRect adjustedTextRect = vopt->icon.isNull() ? itemRect.adjusted(iconSize + 13, 0, 0, 0)
                                                       : textRect.adjusted(iconSize + 3, 0, 0, 0);
          if (vopt->state.testFlag(QStyle::State_Enabled)) {
            painter->setPen(isFirst ? NXThemeColor(_themeMode, PrimaryNormal) : NXThemeColor(_themeMode, BasicText));
          } else {
            painter->setPen(NXThemeColor(_themeMode, BasicTextDisable));
          }
          painter->setFont(vopt->font);
          painter->drawText(adjustedTextRect, vopt->displayAlignment | Qt::TextSingleLine, vopt->text.mid(0, 50)); // 防止长文本溢出

          QFont iconFont(QStringLiteral("NXAwesome"));
          iconFont.setPixelSize(iconSize * 0.8);
          painter->setFont(iconFont);
          painter->drawText(iconRect, Qt::AlignCenter, QChar(_pIconName));
        } else {
          if (vopt->state.testFlag(QStyle::State_Enabled)) {
            painter->setPen(isFirst ? NXThemeColor(_themeMode, PrimaryNormal) : NXThemeColor(_themeMode, BasicText));
          } else {
            painter->setPen(NXThemeColor(_themeMode, BasicTextDisable));
          }
          painter->drawText(vopt->icon.isNull() ? itemRect.adjusted(15, 0, 0, 0) : textRect, vopt->displayAlignment | Qt::TextSingleLine, vopt->text.mid(0, 50));
        }
      }
      // 选中特效
      int heightOffset = itemRect.height() / 4;
      painter->setPen(Qt::NoPen);
      painter->setBrush(NXThemeColor(_themeMode, PrimaryNormal));
      if (isFirst) {
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

QSize NXTreeViewStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
  switch (type) {
  case QStyle::CT_ItemViewItem : {
    QSize itemSize = QProxyStyle::sizeFromContents(type, option, size, widget);
    itemSize.setHeight(_pItemHeight);
    return itemSize;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}

int NXTreeViewStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
  switch (metric) {
  case QStyle::PM_HeaderMargin : {
    return _pHeaderMargin;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}

QRect NXTreeViewStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
  switch (element) {
  case QStyle::SE_ItemViewItemCheckIndicator : {
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      if (vopt->viewItemPosition != QStyleOptionViewItem::Middle && vopt->viewItemPosition != QStyleOptionViewItem::End) {
        QRect indicatorRect = QProxyStyle::subElementRect(element, option, widget);
        indicatorRect.adjust(_leftPadding, 0, _leftPadding, 0);
        return indicatorRect;
      }
    }
    break;
  }
  case QStyle::SE_ItemViewItemDecoration : {
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      if (vopt->viewItemPosition != QStyleOptionViewItem::Middle && vopt->viewItemPosition != QStyleOptionViewItem::End) {
        QRect iconRect = QProxyStyle::subElementRect(element, option, widget);
        iconRect.adjust(_leftPadding + 5, 0, _leftPadding + 5, 0);
        return iconRect;
      }
    }
    break;
  }
  case QStyle::SE_ItemViewItemText : {
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      if (vopt->viewItemPosition != QStyleOptionViewItem::Middle && vopt->viewItemPosition != QStyleOptionViewItem::End) {
        QRect textRect = QProxyStyle::subElementRect(element, option, widget);
        textRect.adjust(_leftPadding + 10, 0, 0, 0);
        return textRect;
      }
    }
    break;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::subElementRect(element, option, widget);
}
