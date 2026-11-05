#ifndef NXCALENDARTITLEDELEGATE_H
#define NXCALENDARTITLEDELEGATE_H

#include <QStyledItemDelegate>

#include "NXDef.h"

class NXCalendarTitleDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  explicit NXCalendarTitleDelegate(QObject *parent = nullptr);
  ~NXCalendarTitleDelegate();

  void paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const override;
  QSize sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXCALENDARTITLEDELEGATE_H
