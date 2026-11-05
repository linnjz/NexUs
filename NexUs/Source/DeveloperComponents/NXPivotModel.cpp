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

void NXPivotModel::appendPivot(QPixmap const &pixmap)
{
  appendPivot(QString {}, pixmap);
}

void NXPivotModel::appendPivot(QString const &text, QPixmap const &pixmap)
{
  insertPivot(m_PivotList.count(), text, pixmap);
}

void NXPivotModel::insertPivot(int index, QPixmap const &pixmap)
{
  insertPivot(index, QString {}, pixmap);
}

void NXPivotModel::insertPivot(int index, QString const &text, QPixmap const &pixmap)
{
  beginInsertRows(QModelIndex(), index, index);
  m_PivotList.insert(index, { text, pixmap });
  endInsertRows();
}

void NXPivotModel::removePivot(int index)
{
  beginRemoveRows(QModelIndex(), index, index);
  m_PivotList.removeAt(index);
  endRemoveRows();
}

void NXPivotModel::removePivot(QString const &text)
{
  for (int i = m_PivotList.count() - 1; i >= 0; --i) {
    if (m_PivotList[i].text == text) {
      removePivot(i);
      return;
    }
  }
}

void NXPivotModel::setPivot(int index, QPixmap const &pixmap)
{
  m_PivotList[index].icon = pixmap;
  Q_EMIT dataChanged(this->index(index, 0), this->index(index, 0), { Qt::UserRole, Qt::DisplayRole });
}

void NXPivotModel::setPivot(int index, QString const &text, QPixmap const &pixmap)
{
  m_PivotList[index].text = text;
  m_PivotList[index].icon = pixmap;
  Q_EMIT dataChanged(this->index(index, 0), this->index(index, 0), { Qt::UserRole, Qt::DisplayRole });
}

int NXPivotModel::getPivotListCount() const
{
  return m_PivotList.count();
}

void NXPivotModel::clearPivot()
{
  beginResetModel();
  m_PivotList.clear();
  endResetModel();
}

QList<NXPivotModel::PivotItem> const &NXPivotModel::getPivotList() const
{
  return m_PivotList;
}

int NXPivotModel::getPivotIndex(QString const &text) const
{
  for (int i = 0; i < m_PivotList.count(); ++i) {
    if (m_PivotList[i].text == text) {
      return i;
    }
  }
  return -1;
}

int NXPivotModel::rowCount(QModelIndex const &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return m_PivotList.count();
}

QVariant NXPivotModel::data(QModelIndex const &index, int role) const
{
  if (!index.isValid() || index.row() < 0 || index.row() >= m_PivotList.count()) {
    return QVariant {};
  }

  PivotItem const &item = m_PivotList[index.row()];
  if (role == Qt::DisplayRole) {
    return item.text;
  }
  if (role == Qt::UserRole) {
    return QVariant::fromValue(item);
  }
  return QVariant {};
}
