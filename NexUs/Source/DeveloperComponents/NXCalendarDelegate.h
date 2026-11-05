#ifndef NXCALENDARDELEGATE_H
#define NXCALENDARDELEGATE_H

#include <QDate>
#include <QStyledItemDelegate>

#include "NXCalendarModel.h"
#include "NXDef.h"

class NXCalendarDelegate : public QStyledItemDelegate
{
  Q_OBJECT
  Q_PRIVATE_CREATE(int, ItemWidth)
  Q_PRIVATE_CREATE(int, ItemHeight)
  Q_PRIVATE_CREATE(bool, IsTransparent)

public:
  explicit NXCalendarDelegate(NXCalendarModel *calendarModel, QObject *parent = nullptr);
  ~NXCalendarDelegate();

  Q_SLOT void onCalendarModelDisplayModeChanged();

  void paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const override;
  QSize sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  QDate m_NowDate;
  NXCalendarModel *m_CalendarModel { nullptr };
  void _drawYearOrMonth(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const;
  void _drawDays(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const;
};

#endif // NXCALENDARDELEGATE_H
