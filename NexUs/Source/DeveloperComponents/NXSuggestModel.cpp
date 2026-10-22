#include "NXSuggestModel.h"

NXSuggestModel::NXSuggestModel(QObject *parent)
    : QAbstractListModel { parent }
{
}

NXSuggestModel::~NXSuggestModel()
{
}

int NXSuggestModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)
  return _suggestionVector.count();
}

QVariant NXSuggestModel::data(const QModelIndex &index, int role) const
{
  return QVariant();
}

void NXSuggestModel::setSearchSuggestion(const QVector<NXSuggestion *> &suggestionVector)
{
  if (suggestionVector.count() == 0) {
    return;
  }
  beginResetModel();
  _suggestionVector = suggestionVector;
  endResetModel();
}

void NXSuggestModel::clearSearchNode()
{
  this->_suggestionVector.clear();
}

NXSuggestion * NXSuggestModel::getSearchSuggestion(int row)
{
  if (row >= _suggestionVector.count()) {
    return nullptr;
  }
  return _suggestionVector[row];
}
