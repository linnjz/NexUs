#ifndef NXCALENDARMODEL_H
#define NXCALENDARMODEL_H

#include <QAbstractListModel>
#include <QDate>
#include <QMetaType>

#include "NXProperty.h"

enum NXCalendarType
{
  YearMode  = 0x00'01,
  MonthMode = 0x00'02,
  DayMode   = 0x00'03,
};

struct NXCalendarData : public QObjectData
{
public:
  NXCalendarData() { };
  ~NXCalendarData() { };
  NXCalendarData(int year, int month, int day, QString const &desText = {})
      : year(year)
      , month(month)
      , day(day)
      , desText(desText) { };

  NXCalendarData(NXCalendarData const &other)
  {
    year    = other.year;
    month   = other.month;
    day     = other.day;
    desText = other.desText;
  }

  int year  = 1924;
  int month = 1;
  int day   = 1;
  QString desText { "" };
};

Q_DECLARE_METATYPE(NXCalendarData);

class NXCalendarModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PRIVATE_CREATE_H(QDate, MinimumDate)
  Q_PRIVATE_CREATE_H(QDate, MaximumDate)

public:
  explicit NXCalendarModel(QObject *parent = nullptr);
  ~NXCalendarModel();

  void setDisplayMode(NXCalendarType displayType);
  NXCalendarType getDisplayMode() const;

  QModelIndex getIndexFromDate(QDate date);
  QDate getDateFromIndex(QModelIndex const &index) const;
  QVariant data(QModelIndex const &index, int role) const override;
  Q_SIGNAL void currentYearMonthChanged(QString const &date);
  Q_SIGNAL void displayModeChanged();

protected:
  virtual int rowCount(QModelIndex const &parent = QModelIndex()) const override;

private:
  int m_Offset { 0 };
  QDate m_MinimumDate;
  QDate m_MaximumDate;
  NXCalendarType m_DisplayMode { NXCalendarType::DayMode };
  int m_DayRowCount { 0 };
  void _initRowCount();
  int _getCurrentDay(int row) const;
};

#endif // NXCALENDARMODEL_H
