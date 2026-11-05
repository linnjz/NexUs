#ifndef NXSUGGESTMODEL_H
#define NXSUGGESTMODEL_H

#include <QAbstractListModel>

#include "NXDef.h"
class NXSuggestion;

class NXSuggestModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit NXSuggestModel(QObject *parent = nullptr);
  ~NXSuggestModel();
  int rowCount(QModelIndex const &parent) const;
  QVariant data(QModelIndex const &index, int role) const;
  void setSearchSuggestion(QVector<NXSuggestion *> const &suggestionVector);
  void clearSearchNode();
  NXSuggestion *getSearchSuggestion(int row);

private:
  QVector<NXSuggestion *> m_SuggestionVector; //符合搜索的节点
};

#endif // NXSUGGESTMODEL_H
