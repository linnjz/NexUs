#ifndef T_ICONMODEL_H
#define T_ICONMODEL_H

#include <QAbstractListModel>
#include <QMetaEnum>

#include "NXProperty.h"

class T_IconModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY_CREATE(bool, IsSearchMode)

public:
  explicit T_IconModel(QObject *parent = nullptr);
  ~T_IconModel();
  int rowCount(QModelIndex const &parent = QModelIndex()) const override;
  void setSearchKeyList(QStringList const &list);
  QStringList getSearchKeyList();

  QVariant data(QModelIndex const &index, int role = Qt::DisplayRole) const override;
  QString getIconNameFromModelIndex(QModelIndex const &index) const;

private:
  int _rowCount;
  QStringList _searchKeyList;
  QMetaEnum _metaEnum;
};

#endif // T_ICONMODEL_H
