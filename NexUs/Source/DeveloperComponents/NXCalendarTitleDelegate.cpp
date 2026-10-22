#include "NXCalendarTitleDelegate.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include "NXTheme.h"

NXCalendarTitleDelegate::NXCalendarTitleDelegate(QObject *parent)
    : QStyledItemDelegate { parent }
{
  _themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXCalendarTitleDelegate::~NXCalendarTitleDelegate()
{
}

void NXCalendarTitleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  painter->save();
  painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  QRectF itemRect = option.rect;
  // 文字绘制
  QString title = index.data(Qt::UserRole).toString();
  if (!title.isEmpty()) {
    painter->setPen(NXThemeColor(_themeMode, BasicText));
    QFont font = painter->font();
    font.setWeight(QFont::Bold);
    painter->setFont(font);
    painter->drawText(itemRect, Qt::AlignCenter, title);
  }
  painter->restore();
  QStyledItemDelegate::paint(painter, option, index);
}

QSize NXCalendarTitleDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  return QSize(42, 30);
}
