#include "NXCalendarModel.h"

NXCalendarModel::NXCalendarModel(QObject *parent)
    : QAbstractListModel { parent }
{
  m_MinimumDate.setDate(1924, 1, 1);
  m_MaximumDate.setDate(2124, 12, 31);
  _initRowCount();
}

NXCalendarModel::~NXCalendarModel()
{
}

void NXCalendarModel::setMinimumDate(QDate minimudate)
{
  m_MinimumDate = minimudate;
  _initRowCount();
}

QDate NXCalendarModel::getMinimumDate() const noexcept(std::is_nothrow_copy_constructible_v<QDate>)
{
  return m_MinimumDate;
}

void NXCalendarModel::setMaximumDate(QDate maximumDate)
{
  m_MaximumDate = maximumDate;
  _initRowCount();
}

QDate NXCalendarModel::getMaximumDate() const noexcept(std::is_nothrow_copy_constructible_v<QDate>)
{
  return m_MaximumDate;
}

void NXCalendarModel::setDisplayMode(NXCalendarType displayMode)
{
  beginResetModel();
  m_DisplayMode = displayMode;
  endResetModel();
  Q_EMIT displayModeChanged();
}

NXCalendarType NXCalendarModel::getDisplayMode() const
{
  return m_DisplayMode;
}

QModelIndex NXCalendarModel::getIndexFromDate(QDate date)
{
  switch (m_DisplayMode) {
  case YearMode : {
    return this->index(date.year() - m_MinimumDate.year());
  }
  case MonthMode : {
    return this->index((date.year() - m_MinimumDate.year()) * 12 + date.month() - 1);
  }
  case DayMode : {
    return this->index(m_MinimumDate.daysTo(date) + m_Offset);
  }
  }
  return QModelIndex();
}

QDate NXCalendarModel::getDateFromIndex(QModelIndex const &index) const
{
  if (!index.isValid() || index.row() < m_Offset) {
    return QDate();
  }
  return m_MinimumDate.addDays(index.row() - m_Offset);
}

QVariant NXCalendarModel::data(QModelIndex const &index, int role) const
{
  if (role == Qt::UserRole) {
    switch (m_DisplayMode) {
    case YearMode : {
      return QVariant::fromValue<NXCalendarData>(NXCalendarData(m_MinimumDate.year() + index.row(), 1, 1));
    }
    case MonthMode : {
      int year  = m_MinimumDate.year() + index.row() / 12;
      int month = index.row() % 12 + 1;
      if (month == 1) {
        return QVariant::fromValue<NXCalendarData>(NXCalendarData(year, month, 1, QString::number(year)));
      } else {
        return QVariant::fromValue<NXCalendarData>(NXCalendarData(m_MinimumDate.year() + index.row() / 12, month, 1));
      }
    }
    case DayMode : {
      if (index.row() >= m_Offset) {
        QDate date = getDateFromIndex(index);
        if (date.day() == 1) {
          return QVariant::fromValue<NXCalendarData>(
              NXCalendarData(date.year(), date.month(), date.day(), QString(QStringLiteral("%1月")).arg(date.month())));
        } else {
          return QVariant::fromValue<NXCalendarData>(NXCalendarData(date.year(), date.month(), date.day()));
        }
      }
    }
    }
  }
  return QVariant();
}

int NXCalendarModel::rowCount(QModelIndex const &parent) const
{
  switch (m_DisplayMode) {
  case YearMode : {
    return m_MaximumDate.year() - m_MinimumDate.year() + 1;
  }
  case MonthMode : {
    return (m_MaximumDate.year() - m_MinimumDate.year() + 1) * 12;
  }
  case DayMode : {
    return m_DayRowCount;
  }
  }
  return 0;
}

void NXCalendarModel::_initRowCount()
{
  m_DayRowCount = m_MinimumDate.daysTo(m_MaximumDate);
  m_Offset      = m_MinimumDate.dayOfWeek();
  m_DayRowCount += m_Offset + 1;
}

int NXCalendarModel::_getCurrentDay(int row) const
{
  return m_MinimumDate.addDays(row - m_Offset).day();
}
