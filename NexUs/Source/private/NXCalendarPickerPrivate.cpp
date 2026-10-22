#include "NXCalendarPickerPrivate.h"

#include <QDate>
#include <QPropertyAnimation>

#include "DeveloperComponents/NXCalendarPickerContainer.h"
#include "NXCalendar.h"
#include "NXCalendarPicker.h"

NXCalendarPickerPrivate::NXCalendarPickerPrivate(QObject *parent)
    : QObject { parent }
{
}

NXCalendarPickerPrivate::~NXCalendarPickerPrivate()
{
}

void NXCalendarPickerPrivate::onCalendarPickerClicked()
{
  Q_Q(NXCalendarPicker);
  QPoint endPoint(q->mapToGlobal(QPoint((q->width() - _calendarPickerContainer->width()) / 2, q->height() + 5)));
  QPropertyAnimation *showAnimation = new QPropertyAnimation(_calendarPickerContainer, "pos");
  showAnimation->setEasingCurve(QEasingCurve::OutCubic);
  showAnimation->setDuration(250);
  showAnimation->setStartValue(QPoint(endPoint.x(), endPoint.y() - 10));
  showAnimation->setEndValue(endPoint);
  _calendarPickerContainer->show();
  showAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXCalendarPickerPrivate::onCalendarSelectedDateChanged()
{
  Q_Q(NXCalendarPicker);
  Q_EMIT q->selectedDateChanged(_calendar->getSelectedDate());
  q->update();
  if (_calendarPickerContainer->isVisible()) {
    _calendarPickerContainer->hide();
  }
}
