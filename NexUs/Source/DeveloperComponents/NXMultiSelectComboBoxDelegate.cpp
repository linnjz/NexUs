#include "NXMultiSelectComboBoxDelegate.h"

#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"

NXMultiSelectComboBoxDelegate::NXMultiSelectComboBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXMultiSelectComboBoxDelegate::~NXMultiSelectComboBoxDelegate()
{
}

void NXMultiSelectComboBoxDelegate::setItemSelection(QVector<bool> const &selection)
{
  m_ItemSelection = selection;
}

void NXMultiSelectComboBoxDelegate::paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  painter->save();
  painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

  int margin   = 2;
  QRect bgRect = option.rect;
  bgRect.adjust(margin, margin, -margin, -margin);
#ifndef Q_OS_WIN
  bgRect.adjust(6, 0, -6, 0);
#endif

  QPainterPath bgPath;
  bgPath.addRoundedRect(bgRect, 5, 5);

  bool isSelected = index.row() < m_ItemSelection.size() && m_ItemSelection[index.row()];

  painter->setPen(Qt::NoPen);
  if (isSelected) {
    if (option.state & QStyle::State_MouseOver) {
      painter->fillPath(bgPath, NXThemeColor(m_ThemeMode, BasicSelectedHoverAlpha));
    } else {
      painter->fillPath(bgPath, NXThemeColor(m_ThemeMode, BasicSelectedAlpha));
    }
  } else {
    if (option.state & QStyle::State_MouseOver) {
      painter->fillPath(bgPath, NXThemeColor(m_ThemeMode, BasicHoverAlpha));
    }
  }

  int checkBoxSize = 21;
  QRect checkBoxRect(bgRect.x() + 8, bgRect.y() + (bgRect.height() - checkBoxSize) / 2, checkBoxSize, checkBoxSize);
  checkBoxRect.adjust(1, 1, -1, -1);

  painter->setPen(Qt::NoPen);
  if (isSelected) {
    if (option.state & QStyle::State_MouseOver) {
      painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryHover));
    } else {
      painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
    }
  } else {
    painter->setPen(NXThemeColor(m_ThemeMode, BasicBorderDeep));
    if (option.state & QStyle::State_MouseOver) {
      painter->setBrush(NXThemeColor(m_ThemeMode, BasicHover));
    } else {
      painter->setBrush(NXThemeColor(m_ThemeMode, BasicBase));
    }
  }
  painter->drawRoundedRect(checkBoxRect, 2, 2);

  if (isSelected) {
    painter->setPen(NXThemeColor(NXThemeType::Dark, BasicText));
    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(checkBoxSize * 0.75);
    painter->setFont(iconFont);
    painter->drawText(checkBoxRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Check));
  }

  painter->setFont(option.font);
  painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
  QRect textRect(bgRect.x() + checkBoxSize + 18, bgRect.y(), bgRect.width() - checkBoxSize - 18, bgRect.height());
  painter->drawText(textRect, Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());

  painter->restore();
}

QSize NXMultiSelectComboBoxDelegate::sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  return QSize(option.rect.width(), 35);
}
