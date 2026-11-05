#include "T_LogModel.h"

T_LogModel::T_LogModel(QObject *parent)
    : QAbstractListModel { parent }
{
}

T_LogModel::~T_LogModel()
{
}

int T_LogModel::rowCount(QModelIndex const &parent) const
{
  return this->_logList.count();
}

QVariant
T_LogModel::data(QModelIndex const &index, int role) const
{
  if (role == Qt::DisplayRole) {
    return _logList[index.row()];
  }
  return QVariant();
}

void T_LogModel::setLogList(QStringList const &list)
{
  beginResetModel();
  this->_logList = list;
  endResetModel();
}

void T_LogModel::appendLogList(QString const &log)
{
  beginResetModel();
  this->_logList.append(log);
  endResetModel();
}

QStringList
T_LogModel::getLogList() const
{
  return this->_logList;
}
