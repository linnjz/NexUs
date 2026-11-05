#ifndef NXCALENDARPICKERPRIVATE_H
#define NXCALENDARPICKERPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXCalendar;
class NXCalendarPicker;
class NXCalendarPickerContainer;

class NXCalendarPickerPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXCalendarPicker)
  Q_PROPERTY_CREATE_D(int, BorderRadius)

public:
  explicit NXCalendarPickerPrivate(QObject *parent = nullptr);
  ~NXCalendarPickerPrivate();
  Q_SLOT void onCalendarPickerClicked();
  Q_SLOT void onCalendarSelectedDateChanged();

private:
  NXThemeType::ThemeMode m_ThemeMode;
  NXCalendar *m_Calendar { nullptr };
  NXCalendarPickerContainer *m_CalendarPickerContainer { nullptr };
};

#endif // NXCALENDARPICKERPRIVATE_H
