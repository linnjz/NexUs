#include "NXColorDisplayModel.h"

NXColorDisplayModel::NXColorDisplayModel(QObject *parent)
    : QAbstractListModel { parent }
{
}

NXColorDisplayModel::~NXColorDisplayModel()
{
}

void NXColorDisplayModel::appendDisplayColor(const QList<QColor> &colorList)
{
  beginResetModel();
  _displayColorList.append(colorList);
  endResetModel();
}

void NXColorDisplayModel::appendDisplayColor(const QColor &color)
{
  beginResetModel();
  _displayColorList.append(color);
  endResetModel();
}

void NXColorDisplayModel::removeDisplayColor(int index)
{
  if (index < 0 || index >= _displayColorList.count()) {
    return;
  }
  beginResetModel();
  _displayColorList.removeAt(index);
  endResetModel();
}

void NXColorDisplayModel::replaceDisplayColor(const QColor &color, int index)
{
  if (index < 0 || index >= _displayColorList.count()) {
    return;
  }
  beginResetModel();
  _displayColorList[index] = color;
  endResetModel();
}

QList<QColor> NXColorDisplayModel::getDisplayColorList() const
{
  return _displayColorList;
}

QColor NXColorDisplayModel::getDisplayColor(int index) const
{
  if (index < 0 || index >= _displayColorList.count()) {
    return QColor();
  }
  return _displayColorList[index];
}

int NXColorDisplayModel::rowCount(const QModelIndex &parent) const
{
  return _displayColorList.count();
}

QVariant NXColorDisplayModel::data(const QModelIndex &index, int role) const
{
  if (role == Qt::UserRole) {
    return QVariant::fromValue(_displayColorList[index.row()]);
  }
  return QVariant();
}
