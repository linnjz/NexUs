#include "NXColorDisplayModel.h"

NXColorDisplayModel::NXColorDisplayModel(QObject *parent)
    : QAbstractListModel { parent }
{
}

NXColorDisplayModel::~NXColorDisplayModel()
{
}

void NXColorDisplayModel::appendDisplayColor(QList<QColor> const &colorList)
{
  beginResetModel();
  m_DisplayColorList.append(colorList);
  endResetModel();
}

void NXColorDisplayModel::appendDisplayColor(QColor const &color)
{
  beginResetModel();
  m_DisplayColorList.append(color);
  endResetModel();
}

void NXColorDisplayModel::removeDisplayColor(int index)
{
  if (index < 0 || index >= m_DisplayColorList.count()) {
    return;
  }
  beginResetModel();
  m_DisplayColorList.removeAt(index);
  endResetModel();
}

void NXColorDisplayModel::replaceDisplayColor(QColor const &color, int index)
{
  if (index < 0 || index >= m_DisplayColorList.count()) {
    return;
  }
  beginResetModel();
  m_DisplayColorList[index] = color;
  endResetModel();
}

QList<QColor> NXColorDisplayModel::getDisplayColorList() const
{
  return m_DisplayColorList;
}

QColor NXColorDisplayModel::getDisplayColor(int index) const
{
  if (index < 0 || index >= m_DisplayColorList.count()) {
    return QColor();
  }
  return m_DisplayColorList[index];
}

int NXColorDisplayModel::rowCount(QModelIndex const &parent) const
{
  return m_DisplayColorList.count();
}

QVariant NXColorDisplayModel::data(QModelIndex const &index, int role) const
{
  if (role == Qt::UserRole) {
    return QVariant::fromValue(m_DisplayColorList[index.row()]);
  }
  return QVariant();
}
