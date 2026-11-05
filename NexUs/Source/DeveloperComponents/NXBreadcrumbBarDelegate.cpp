#include "NXBreadcrumbBarDelegate.h"

#include <QPainter>

#include "NXTheme.h"

NXBreadcrumbBarDelegate::NXBreadcrumbBarDelegate(QObject *parent)
    : QStyledItemDelegate { parent }
    , m_PressIndex {}
    , m_ThemeMode { nxTheme->getThemeMode() }

{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXBreadcrumbBarDelegate::~NXBreadcrumbBarDelegate()
{
}

void NXBreadcrumbBarDelegate::paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  painter->save();
  painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
  QRect itemRect                = option.rect;
  QString breadcrumbDisplayData = index.data(Qt::DisplayRole).toString();
  QString breadcrumbUserData    = index.data(Qt::UserRole).toString();
  if (breadcrumbUserData != QStringLiteral("LastBreadcrumb")) {
    if (m_PressIndex == index) {
      //鼠标按下
      painter->setPen(NXThemeColor(m_ThemeMode, BasicTextPress));
    } else {
      //不为最后一个 且没有被鼠标覆盖
      if (!(option.state & QStyle::State_MouseOver)) {
        painter->setPen(NXThemeColor(m_ThemeMode, BasicTextNoFocus));
      }
    }
  }
  if (breadcrumbDisplayData != QStringLiteral(">")) {
    painter->drawText(itemRect, Qt::AlignVCenter | Qt::AlignLeft, breadcrumbDisplayData);
  } else {
    //分隔符
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(painter->font().pixelSize() * 0.785);
    painter->setFont(iconFont);
    itemRect.setX(itemRect.x() - itemRect.width() * 0.36);
    painter->drawText(itemRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::AngleRight));
  }
  painter->restore();
}
