#include "NXColorDisplayDelegate.h"

#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include "NXTheme.h"

NXColorDisplayDelegate::NXColorDisplayDelegate(QObject *parent)
    : QStyledItemDelegate { parent }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
}

NXColorDisplayDelegate::~NXColorDisplayDelegate()
{
}

void NXColorDisplayDelegate::paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  painter->save();
  painter->setRenderHints(QPainter::Antialiasing);
  QRectF itemRect = option.rect;
  // 颜色球绘制
  QColor displayColor = index.data(Qt::UserRole).value<QColor>();
  if (displayColor.isValid()) {
    painter->setPen(NXThemeColor(m_ThemeMode, PopupBorderHover));
    painter->setBrush(index.data(Qt::UserRole).value<QColor>());
  } else {
    QPen pen(NXThemeColor(m_ThemeMode, PopupBorderHover));
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
  }
  painter->drawEllipse(itemRect.center(), 10, 10);

  // 覆盖效果绘制
  if (option.state.testFlag(QStyle::State_MouseOver) || option.state.testFlag(QStyle::State_Selected)) {
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(itemRect.center(), 13, 13);
  }

  painter->restore();
  QStyledItemDelegate::paint(painter, option, index);
}

QSize NXColorDisplayDelegate::sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  return QSize(30, 30);
}
