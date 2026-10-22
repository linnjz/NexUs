#include "NXCalendarPicker.h"

#include <QDate>
#include <QHBoxLayout>
#include <QPainter>
#include <QVBoxLayout>

#include "NXApplication.h"
#include "DeveloperComponents/NXCalendarPickerContainer.h"
#include "NXCalendar.h"
#include "NXTheme.h"
#include "private/NXCalendarPickerPrivate.h"
Q_PROPERTY_CREATE_CPP(NXCalendarPicker, int, BorderRadius)

NXCalendarPicker::NXCalendarPicker(QWidget *parent)
    : QPushButton { parent }
    , d_ptr(new NXCalendarPickerPrivate())
{
  Q_D(NXCalendarPicker);
  d->q_ptr          = this;
  d->_pBorderRadius = 3;
  setFixedSize(120, 30);
  setObjectName("NXCalendarPicker");
  setMouseTracking(true);
  d->_calendarPickerContainer = new NXCalendarPickerContainer(this);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  d->_calendarPickerContainer->resize(305, 340);
#else
  d->_calendarPickerContainer->resize(317, 352);
#endif
  d->_calendar                 = new NXCalendar(d->_calendarPickerContainer);
  QVBoxLayout *containerLayout = new QVBoxLayout(d->_calendarPickerContainer);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  containerLayout->setContentsMargins(0, 0, 0, 0);
#else
  containerLayout->setContentsMargins(6, 6, 6, 6);
#endif
  containerLayout->addWidget(d->_calendar);
  d->_calendarPickerContainer->hide();
  connect(this, &QPushButton::clicked, d, &NXCalendarPickerPrivate::onCalendarPickerClicked);

  connect(d->_calendar, &NXCalendar::pSelectedDateChanged, d, &NXCalendarPickerPrivate::onCalendarSelectedDateChanged);
  setSelectedDate(QDate::currentDate());

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXCalendarPicker::~NXCalendarPicker()
{
}

void NXCalendarPicker::setSelectedDate(QDate selectedDate)
{
  Q_D(NXCalendarPicker);
  d->_calendar->setSelectedDate(selectedDate);
  Q_EMIT selectedDateChanged(selectedDate);
}

QDate NXCalendarPicker::getSelectedDate() const
{
  Q_D(const NXCalendarPicker);
  return d->_calendar->getSelectedDate();
}

void NXCalendarPicker::paintEvent(QPaintEvent *event)
{
  Q_D(NXCalendarPicker);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  // 边框和背景绘制
  QRect baseRect = rect();
  baseRect.adjust(1, 1, -1, -1);
  painter.setPen(NXThemeColor(d->_themeMode, BasicBorder));
  painter.setBrush(underMouse() ? NXThemeColor(d->_themeMode, BasicHover) : NXThemeColor(d->_themeMode, BasicBase));
  painter.drawRoundedRect(baseRect, d->_pBorderRadius, d->_pBorderRadius);

  // 日期绘制
  QDate selectedDate = getSelectedDate();
  QString date =
      QString(QStringLiteral("%1/%2/%3")).arg(selectedDate.year()).arg(selectedDate.month()).arg(selectedDate.day());
  painter.setPen(NXThemeColor(d->_themeMode, BasicText));
  QRect textRect = baseRect;
  textRect.adjust(10, 0, 0, 0);
  painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, date);

  // 图标绘制
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(nxApp->getFontPixelSize() + 4);
  painter.setFont(iconFont);
  painter.drawText(QRect(baseRect.right() - 25, 0, 15, height()), Qt::AlignVCenter | Qt::AlignRight, QChar((unsigned short) NXIconType::CalendarRange));
  painter.restore();
}
