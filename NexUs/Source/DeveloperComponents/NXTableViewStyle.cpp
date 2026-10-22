#include "NXTableViewStyle.h"

#include <QHeaderView>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionViewItem>

#include "NXTableView.h"
#include "NXTheme.h"

NXTableViewStyle::NXTableViewStyle(QStyle *style)
{
  _pCurrentHoverIndex         = QModelIndex();
  _pHeaderMargin              = 6;
  _pBorderRadius              = 3;
  _pCheckIndicatorWidth       = 18;
  _pDefaultPadding            = 11;
  _pIsHoverRowEffectEnable    = true;
  _pIsSelectedRowEffectEnable = true;
  _themeMode                  = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXTableViewStyle::~NXTableViewStyle()
{
}

void NXTableViewStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  //qDebug() << element << option->rect << widget->objectName();
  switch (element) {
  case QStyle::PE_IndicatorItemViewItemDrop : {
    if (option->rect.isNull())
      return;

    QPen pen;
    pen.setColor(NXThemeColor(_themeMode, PrimaryNormal));
    pen.setWidth(2);
    painter->save();
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);

    painter->drawEllipse(QPoint { 8, option->rect.top() }, 5, 5);
    painter->drawLine(8, option->rect.top(), widget->width() - 8, option->rect.top());
    painter->restore();

    return;
  }
  case QStyle::PE_PanelItemViewItem : {
    // 行覆盖绘制
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      const NXTableView *tabView = dynamic_cast<const NXTableView *>(widget);
      if (!tabView) {
        return;
      }
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      QAbstractItemView::SelectionBehavior selectionBehavior = tabView->selectionBehavior();
      if (selectionBehavior == QAbstractItemView::SelectRows) {
        if (_pIsHoverRowEffectEnable && vopt->index.row() == _pCurrentHoverIndex.row()) {
          painter->setPen(Qt::NoPen);
          painter->setBrush(NXThemeColor(_themeMode, BasicHoverAlpha));
          painter->drawRect(vopt->rect);
        }
      } else {
        if (_pIsHoverRowEffectEnable &&
            (vopt->state.testFlag(QStyle::State_Selected) || vopt->state.testFlag(QStyle::State_MouseOver))) {
          painter->setPen(Qt::NoPen);
          painter->setBrush(NXThemeColor(_themeMode, BasicHoverAlpha));
          painter->drawRect(vopt->rect);
        }
      }
      painter->restore();
    }
    return;
  }
  case QStyle::PE_PanelItemViewRow : {
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      const NXTableView *tabView = dynamic_cast<const NXTableView *>(widget);
      if (!tabView) {
        return;
      }
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      QRect itemRect = vopt->rect;
      painter->setPen(Qt::NoPen);
      if (vopt->state & QStyle::State_Selected) {
        // 选中
        painter->setBrush(NXThemeColor(_themeMode, BasicSelectedAlpha));
        painter->drawRect(itemRect);
      } else {
        if (vopt->features.testFlag(QStyleOptionViewItem::Alternate)) {
          // Item背景隔行变色
          painter->setPen(Qt::NoPen);
          painter->setBrush(NXThemeColor(_themeMode, BasicAlternating));
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

void NXTableViewStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  //qDebug() << element << option->rect;
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    // viewport视口外的其他区域背景
    QRect frameRect = option->rect;
    frameRect.adjust(1, 1, -1, -1);
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);
    painter->setPen(NXThemeColor(_themeMode, PopupBorder));
    painter->setBrush(NXThemeColor(_themeMode, BasicBaseAlpha));
    painter->drawRoundedRect(frameRect, _pBorderRadius, _pBorderRadius);
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
      if (!hopt->icon.isNull()) {
        QIcon::Mode mode   = QIcon::Normal;
        QIcon::State state = hopt->state & QStyle::State_Open ? QIcon::On : QIcon::Off;
        QRect iconRect     = proxy()->subElementRect(QStyle::SE_HeaderLabel, hopt, widget);
        hopt->icon.paint(painter, iconRect, hopt->textAlignment, mode, state);
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
    if (option->state.testFlag(QStyle::State_Sunken)) {
      painter->setBrush(NXThemeColor(_themeMode, BasicPressAlpha));
    } else {
      if (option->state.testFlag(QStyle::State_MouseOver)) {
        painter->setBrush(NXThemeColor(_themeMode, BasicHoverAlpha));
      }
    }
    painter->drawRect(option->rect);
    if (const QStyleOptionHeader *hopt = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
      const QHeaderView *header = qobject_cast<const QHeaderView *>(widget);
      if (const QAbstractItemModel *model = header ? header->model() : nullptr) {
        if (QVariant value = model->headerData(hopt->section, header->orientation(), Qt::CheckStateRole);
            value.isValid()) {
          Qt::CheckState checkState = value.value<Qt::CheckState>();
          QRect checkRect           = option->rect;
          checkRect.setSize(QSize(_pCheckIndicatorWidth, _pCheckIndicatorWidth));
          int padding = getColumnPadding(hopt->section);
          checkRect.moveLeft(option->rect.left() + padding + 3);
          checkRect.moveTop(option->rect.center().y() - _pCheckIndicatorWidth / 2 + 1);
          _drawCheckIndicator(painter, checkRect, checkState);
        }
      }
    }
    painter->restore();
    return;
  }
  case QStyle::CE_HeaderEmptyArea : {
    // 表头未使用区域背景绘制
    QRect frameRect = option->rect;
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(_themeMode, BasicBaseDeep));
    painter->drawRect(frameRect);
    painter->restore();
    return;
  }
  case QStyle::CE_ItemViewItem : {
    if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
      // 背景绘制
      const NXTableView *tabView = dynamic_cast<const NXTableView *>(widget);
      if (!tabView) {
        return;
      }
      this->drawPrimitive(QStyle::PE_PanelItemViewItem, option, painter, widget);
      QAbstractItemView::SelectionBehavior selectionBehavior = tabView->selectionBehavior();
      // 内容绘制
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
      // 复选框绘制
      if (vopt->features & QStyleOptionViewItem::HasCheckIndicator) {
        QRect checkRect = proxy()->subElementRect(SE_ItemViewItemCheckIndicator, vopt, widget);
        int padding     = getColumnPadding(vopt->index.column());
        checkRect.adjust(padding, 0, padding, 0);
        _drawCheckIndicator(painter, checkRect, vopt->checkState);
      }
      // 图标绘制
      if (!vopt->icon.isNull()) {
        QRect iconRect = proxy()->subElementRect(SE_ItemViewItemDecoration, vopt, widget);
        int padding    = getColumnPadding(vopt->index.column());
        iconRect.adjust(padding, 0, padding, 0);
        QIcon::Mode mode = QIcon::Normal;
        // if (!(vopt->state.testFlag(QStyle::State_Enabled)))
        // {
        //     mode = QIcon::Disabled;
        // }
        // else if (vopt->state.testFlag(QStyle::State_Selected))
        // {
        //     mode = QIcon::Selected;
        // }
        QIcon::State state = vopt->state & QStyle::State_Open ? QIcon::On : QIcon::Off;
        vopt->icon.paint(painter, iconRect, vopt->decorationAlignment, mode, state);
      }
      // 文字绘制
      if (!vopt->text.isEmpty()) {
        QRect textRect = proxy()->subElementRect(SE_ItemViewItemText, vopt, widget);
        int padding    = getColumnPadding(vopt->index.column());
        textRect.adjust(padding, 0, 0, 0);
        painter->setPen(NXThemeColor(_themeMode, BasicText));
        painter->drawText(textRect, vopt->displayAlignment, vopt->text);
      }
      if (_pIsSelectedRowEffectEnable) {
        // 选中特效
        QRect itemRect   = option->rect;
        int heightOffset = itemRect.height() / 4;
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(_themeMode, PrimaryNormal));
        if (vopt->state.testFlag(QStyle::State_Selected)) {
          if (selectionBehavior == QAbstractItemView::SelectRows && vopt->index.column() == 0) {
            painter->drawRoundedRect(
                QRectF(itemRect.x() + 3, itemRect.y() + heightOffset, 3, itemRect.height() - 2 * heightOffset), 1.5, 1.5);
          } else if (selectionBehavior == QAbstractItemView::SelectItems) {
            int barWidth = itemRect.width() * 0.8;
            painter->drawRoundedRect(
                QRectF(itemRect.x() + (itemRect.width() - barWidth) / 2, itemRect.y() + 3, barWidth, 3), 1.5, 1.5);
          }
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

int NXTableViewStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
  //qDebug() << metric << QProxyStyle::pixelMetric(metric, option, widget);
  switch (metric) {
  case QStyle::PM_IndicatorWidth :
  case QStyle::PM_IndicatorHeight : return _pCheckIndicatorWidth;
  case QStyle::PM_HeaderMargin    : {
    return _pHeaderMargin;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}

void NXTableViewStyle::setColumnPadding(int column, int padding)
{
  _columnPaddingMap[column] = padding;
}

int NXTableViewStyle::getColumnPadding(int column) const
{
  return _columnPaddingMap.value(column, _pDefaultPadding);
}

void NXTableViewStyle::clearColumnPadding(int column)
{
  _columnPaddingMap.remove(column);
}

void NXTableViewStyle::_drawCheckIndicator(QPainter *painter, QRect rect, Qt::CheckState state) const
{
  painter->save();

  if (state == Qt::Checked) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(_themeMode, PrimaryNormal));
    painter->drawRoundedRect(rect, 2, 2);

    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(rect.width() * 0.85);
    painter->setFont(iconFont);
    painter->setPen(NXThemeColor(NXThemeType::Dark, BasicText));
    painter->drawText(rect, Qt::AlignCenter, QChar(NXIconType::Check));
  } else if (state == Qt::PartiallyChecked) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(_themeMode, PrimaryNormal));
    painter->drawRoundedRect(rect, 2, 2);

    painter->setPen(NXThemeColor(NXThemeType::Dark, BasicText));
    QLine line(rect.left() + 3, rect.center().y(), rect.right() - 3, rect.center().y());
    painter->drawLine(line);
  } else // Qt::Unchecked
  {
    painter->setPen(QPen(NXThemeColor(_themeMode, BasicBorderDeep), 1.2));
    painter->setBrush(Qt::transparent);
    painter->drawRoundedRect(rect, 2, 2);
  }

  painter->restore();
}
