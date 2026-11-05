#include "NXTableViewStyle.h"

#include <QHeaderView>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionViewItem>

#include "NXTableView.h"
#include "NXTheme.h"

NXTableViewStyle::NXTableViewStyle(QStyle *style)
    : m_CurrentHoverIndex {}
    , m_HeaderMargin { 6 }
    , m_BorderRadius { 3 }
    , m_CheckIndicatorWidth { 18 }
    , m_DefaultPadding { 11 }
    , m_IsHoverRowEffectEnable { true }
    , m_IsSelectedRowEffectEnable { true }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXTableViewStyle::~NXTableViewStyle()
{
}

void NXTableViewStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  //qDebug() << element << option->rect << widget->objectName();
  switch (element) {
  case QStyle::PE_IndicatorItemViewItemDrop : {
    if (option->rect.isNull())
      return;

    QPen pen;
    pen.setColor(NXThemeColor(m_ThemeMode, PrimaryNormal));
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
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      NXTableView const *tabView = dynamic_cast<NXTableView const *>(widget);
      if (!tabView) {
        return;
      }
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      QAbstractItemView::SelectionBehavior selectionBehavior = tabView->selectionBehavior();
      if (selectionBehavior == QAbstractItemView::SelectRows) {
        if (m_IsHoverRowEffectEnable && vopt->index.row() == m_CurrentHoverIndex.row()) {
          painter->setPen(Qt::NoPen);
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicHoverAlpha));
          painter->drawRect(vopt->rect);
        }
      } else {
        if (m_IsHoverRowEffectEnable &&
            (vopt->state.testFlag(QStyle::State_Selected) || vopt->state.testFlag(QStyle::State_MouseOver))) {
          painter->setPen(Qt::NoPen);
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
      NXTableView const *tabView = dynamic_cast<NXTableView const *>(widget);
      if (!tabView) {
        return;
      }
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      QRect itemRect = vopt->rect;
      painter->setPen(Qt::NoPen);
      if (vopt->state & QStyle::State_Selected) {
        // 选中
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicSelectedAlpha));
        painter->drawRect(itemRect);
      } else {
        if (vopt->features.testFlag(QStyleOptionViewItem::Alternate)) {
          // Item背景隔行变色
          painter->setPen(Qt::NoPen);
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

void NXTableViewStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  //qDebug() << element << option->rect;
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    // viewport视口外的其他区域背景
    QRect frameRect = option->rect;
    frameRect.adjust(1, 1, -1, -1);
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);
    painter->setPen(NXThemeColor(m_ThemeMode, PopupBorder));
    painter->setBrush(NXThemeColor(m_ThemeMode, BasicBaseAlpha));
    painter->drawRoundedRect(frameRect, m_BorderRadius, m_BorderRadius);
    painter->restore();
    return;
  }
  case QStyle::CE_HeaderLabel : {
    // 表头文字绘制
    if (QStyleOptionHeader const *hopt = qstyleoption_cast<QStyleOptionHeader const *>(option)) {
      QRect headerRect = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
      if (!hopt->text.isEmpty()) {
        painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
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
    if (QStyleOptionHeader const *hopt = qstyleoption_cast<QStyleOptionHeader const *>(option)) {
      QHeaderView const *header = qobject_cast<QHeaderView const *>(widget);
      if (QAbstractItemModel const *model = header ? header->model() : nullptr) {
        if (QVariant value = model->headerData(hopt->section, header->orientation(), Qt::CheckStateRole);
            value.isValid()) {
          Qt::CheckState checkState = value.value<Qt::CheckState>();
          QRect checkRect           = option->rect;
          checkRect.setSize(QSize(m_CheckIndicatorWidth, m_CheckIndicatorWidth));
          int padding = getColumnPadding(hopt->section);
          checkRect.moveLeft(option->rect.left() + padding + 3);
          checkRect.moveTop(option->rect.center().y() - m_CheckIndicatorWidth / 2 + 1);
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
    painter->setBrush(NXThemeColor(m_ThemeMode, BasicBaseDeep));
    painter->drawRect(frameRect);
    painter->restore();
    return;
  }
  case QStyle::CE_ItemViewItem : {
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      // 背景绘制
      NXTableView const *tabView = dynamic_cast<NXTableView const *>(widget);
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
        painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
        painter->drawText(textRect, vopt->displayAlignment, vopt->text);
      }
      if (m_IsSelectedRowEffectEnable) {
        // 选中特效
        QRect itemRect   = option->rect;
        int heightOffset = itemRect.height() / 4;
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
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

int NXTableViewStyle::pixelMetric(PixelMetric metric, QStyleOption const *option, QWidget const *widget) const
{
  //qDebug() << metric << QProxyStyle::pixelMetric(metric, option, widget);
  switch (metric) {
  case QStyle::PM_IndicatorWidth :
  case QStyle::PM_IndicatorHeight : return m_CheckIndicatorWidth;
  case QStyle::PM_HeaderMargin    : {
    return m_HeaderMargin;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}

void NXTableViewStyle::setColumnPadding(int column, int padding)
{
  m_ColumnPaddingMap[column] = padding;
}

int NXTableViewStyle::getColumnPadding(int column) const
{
  return m_ColumnPaddingMap.value(column, m_DefaultPadding);
}

void NXTableViewStyle::clearColumnPadding(int column)
{
  m_ColumnPaddingMap.remove(column);
}

void NXTableViewStyle::_drawCheckIndicator(QPainter *painter, QRect rect, Qt::CheckState state) const
{
  painter->save();

  if (state == Qt::Checked) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
    painter->drawRoundedRect(rect, 2, 2);

    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(rect.width() * 0.85);
    painter->setFont(iconFont);
    painter->setPen(NXThemeColor(NXThemeType::Dark, BasicText));
    painter->drawText(rect, Qt::AlignCenter, QChar(NXIconType::Check));
  } else if (state == Qt::PartiallyChecked) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
    painter->drawRoundedRect(rect, 2, 2);

    painter->setPen(NXThemeColor(NXThemeType::Dark, BasicText));
    QLine line(rect.left() + 3, rect.center().y(), rect.right() - 3, rect.center().y());
    painter->drawLine(line);
  } else // Qt::Unchecked
  {
    painter->setPen(QPen(NXThemeColor(m_ThemeMode, BasicBorderDeep), 1.2));
    painter->setBrush(Qt::transparent);
    painter->drawRoundedRect(rect, 2, 2);
  }

  painter->restore();
}
