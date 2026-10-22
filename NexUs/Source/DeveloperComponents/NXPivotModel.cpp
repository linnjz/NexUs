#include "NXPivotModel.h"

#include <QFontMetrics>
#include <QSize>

NXPivotModel::NXPivotModel(QObject *parent)
    : QAbstractListModel { parent }
{
}

NXPivotModel::~NXPivotModel()
{
}

void NXPivotModel::appendPivot(const QPixmap &pixmap)
{
  appendPivot(QString {}, pixmap);
}

void NXPivotModel::appendPivot(const QString &text, const QPixmap &pixmap)
{
  insertPivot(_pivotList.count(), text, pixmap);
}

void NXPivotModel::insertPivot(int index, const QPixmap &pixmap)
{
  insertPivot(index, QString {}, pixmap);
}

void NXPivotModel::insertPivot(int index, const QString &text, const QPixmap &pixmap)
{
  beginInsertRows(QModelIndex(), index, index);
  _pivotList.insert(index, { text, pixmap });
  endInsertRows();
}

void NXPivotModel::removePivot(int index)
{
  beginRemoveRows(QModelIndex(), index, index);
  _pivotList.removeAt(index);
  endRemoveRows();
}

void NXPivotModel::removePivot(const QString &text)
{
  for (int i = _pivotList.count() - 1; i >= 0; --i) {
    if (_pivotList[i].text == text) {
      removePivot(i);
      return;
    }
  }
}

void NXPivotModel::setPivot(int index, const QPixmap &pixmap)
{
  _pivotList[index].icon = pixmap;
  Q_EMIT dataChanged(this->index(index, 0), this->index(index, 0), { Qt::UserRole, Qt::DisplayRole });
}

void NXPivotModel::setPivot(int index, const QString &text, const QPixmap &pixmap)
{
  _pivotList[index].text = text;
  _pivotList[index].icon = pixmap;
  Q_EMIT dataChanged(this->index(index, 0), this->index(index, 0), { Qt::UserRole, Qt::DisplayRole });
}

int NXPivotModel::getPivotListCount() const
{
  return _pivotList.count();
}

void NXPivotModel::clearPivot()
{
  beginResetModel();
  _pivotList.clear();
  endResetModel();
}

const QList<NXPivotModel::PivotItem> & NXPivotModel::getPivotList() const
{
  return _pivotList;
}

int NXPivotModel::getPivotIndex(const QString &text) const
{
  for (int i = 0; i < _pivotList.count(); ++i) {
    if (_pivotList[i].text == text) {
      return i;
    }
  }
  return -1;
}

int NXPivotModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return _pivotList.count();
}

QVariant NXPivotModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() || index.row() < 0 || index.row() >= _pivotList.count()) {
    return QVariant {};
  }

  const PivotItem &item = _pivotList[index.row()];
  if (role == Qt::DisplayRole) {
    return item.text;
  }
  if (role == Qt::UserRole) {
    return QVariant::fromValue(item);
  }
  return QVariant {};
}
