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
Q_PROPERTY_CREATE_CPP(NXCalendarPicker, QPS_PAEMIT(int), BorderRadius)

NXCalendarPicker::NXCalendarPicker(QWidget *parent)
    : QPushButton { parent }
    , d_ptr(new NXCalendarPickerPrivate())
{
  Q_D(NXCalendarPicker);
  d->q_ptr          = this;
  d->m_BorderRadius = 3;
  setFixedSize(120, 30);
  setObjectName("NXCalendarPicker");
  setMouseTracking(true);
  d->m_CalendarPickerContainer = new NXCalendarPickerContainer(this);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  d->m_CalendarPickerContainer->resize(305, 340);
#else
  d->m_CalendarPickerContainer->resize(317, 352);
#endif
  d->m_Calendar                 = new NXCalendar(d->m_CalendarPickerContainer);
  QVBoxLayout *containerLayout = new QVBoxLayout(d->m_CalendarPickerContainer);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  containerLayout->setContentsMargins(0, 0, 0, 0);
#else
  containerLayout->setContentsMargins(6, 6, 6, 6);
#endif
  containerLayout->addWidget(d->m_Calendar);
  d->m_CalendarPickerContainer->hide();
  connect(this, &QPushButton::clicked, d, &NXCalendarPickerPrivate::onCalendarPickerClicked);

  connect(d->m_Calendar, &NXCalendar::signalSelectedDateChanged, d, &NXCalendarPickerPrivate::onCalendarSelectedDateChanged);
  setSelectedDate(QDate::currentDate());

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXCalendarPicker::~NXCalendarPicker()
{
}

void NXCalendarPicker::setSelectedDate(QDate selectedDate)
{
  Q_D(NXCalendarPicker);
  d->m_Calendar->setSelectedDate(selectedDate);
  Q_EMIT selectedDateChanged(selectedDate);
}

QDate NXCalendarPicker::getSelectedDate() const noexcept(std::is_nothrow_copy_constructible_v<QDate>)
{
  Q_CONST_D(NXCalendarPicker);
  return d->m_Calendar->getSelectedDate();
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
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorder));
  painter.setBrush(underMouse() ? NXThemeColor(d->m_ThemeMode, BasicHover) : NXThemeColor(d->m_ThemeMode, BasicBase));
  painter.drawRoundedRect(baseRect, d->m_BorderRadius, d->m_BorderRadius);

  // 日期绘制
  QDate selectedDate = getSelectedDate();
  QString date =
      QString(QStringLiteral("%1/%2/%3")).arg(selectedDate.year()).arg(selectedDate.month()).arg(selectedDate.day());
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
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
