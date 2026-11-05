#include "NXCalendarTitleModel.h"

NXCalendarTitleModel::NXCalendarTitleModel(QObject *parent)
    : QAbstractListModel { parent }
{
}

NXCalendarTitleModel::~NXCalendarTitleModel()
{
}

int NXCalendarTitleModel::rowCount(QModelIndex const &parent) const
{
  return 7;
}

QVariant NXCalendarTitleModel::data(QModelIndex const &index, int role) const
{
  if (role == Qt::UserRole) {
    switch (index.row()) {
    case 0 : return tr("日");
    case 1 : return tr("一");
    case 2 : return tr("二");
    case 3 : return tr("三");
    case 4 : return tr("四");
    case 5 : return tr("五");
    case 6 : return tr("六");
    }
  }
  return QVariant();
}
