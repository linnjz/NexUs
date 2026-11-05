#include "NXSuggestModel.h"

NXSuggestModel::NXSuggestModel(QObject *parent)
    : QAbstractListModel { parent }
{
}

NXSuggestModel::~NXSuggestModel()
{
}

int NXSuggestModel::rowCount(QModelIndex const &parent) const
{
  Q_UNUSED(parent)
  return m_SuggestionVector.count();
}

QVariant NXSuggestModel::data(QModelIndex const &index, int role) const
{
  return QVariant();
}

void NXSuggestModel::setSearchSuggestion(QVector<NXSuggestion *> const &suggestionVector)
{
  if (suggestionVector.count() == 0) {
    return;
  }
  beginResetModel();
  m_SuggestionVector = suggestionVector;
  endResetModel();
}

void NXSuggestModel::clearSearchNode()
{
  this->m_SuggestionVector.clear();
}

NXSuggestion *NXSuggestModel::getSearchSuggestion(int row)
{
  if (row >= m_SuggestionVector.count()) {
    return nullptr;
  }
  return m_SuggestionVector[row];
}
