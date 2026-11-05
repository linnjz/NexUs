#ifndef NXCALENDARPRIVATE_H
#define NXCALENDARPRIVATE_H

#include <QDate>
#include <QObject>
#include <QPixmap>

#include "NXDef.h"
class NXCalendar;
class NXBaseListView;
class NXCalendarModel;
class NXCalendarDelegate;
class NXToolButton;

class NXCalendarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXCalendar)
  Q_PROPERTY_CREATE_D(QDate, SelectedDate)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ZoomRatio)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), PixOpacity)
  Q_PROPERTY_CREATE_D(int, BorderRaiuds)

public:
  explicit NXCalendarPrivate(QObject *parent = nullptr);
  ~NXCalendarPrivate();
  Q_SLOT void onSwitchButtonClicked();
  Q_SLOT void onCalendarViewClicked(QModelIndex const &index);
  Q_SLOT void onUpButtonClicked();
  Q_SLOT void onDownButtonClicked();

private:
  bool m_IsSwitchAnimationFinished { true };
  bool m_IsDrawNewPix { false };
  int m_LastSelectedYear { 0 };
  int m_LastSelectedMonth { 1 };
  NXThemeType::ThemeMode m_ThemeMode;
  qreal m_BorderWidth { 1.5 };
  QPixmap m_OldCalendarViewPix;
  QPixmap m_NewCalendarViewPix;
  NXBaseListView *m_CalendarView { nullptr };
  NXCalendarModel *m_CalendarModel { nullptr };
  NXCalendarDelegate *m_CalendarDelegate { nullptr };
  NXBaseListView *m_CalendarTitleView { nullptr };
  NXToolButton *m_ModeSwitchButton { nullptr };
  NXToolButton *m_UpButton { nullptr };
  NXToolButton *m_DownButton { nullptr };
  void _scrollToDate(QDate date);
  void _doSwitchAnimation(bool isZoomIn);
  void _updateSwitchButtonText();
};

#endif // NXCALENDARPRIVATE_H
