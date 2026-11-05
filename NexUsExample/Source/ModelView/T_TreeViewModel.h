#ifndef T_TREEVIEWMODEL_H
#define T_TREEVIEWMODEL_H

#include <QAbstractItemModel>
#include <QObject>

class T_TreeItem;

class T_TreeViewModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  explicit T_TreeViewModel(QObject *parent = nullptr);
  ~T_TreeViewModel();
  QModelIndex parent(QModelIndex const &child) const override;
  QModelIndex index(int row, int column, QModelIndex const &parent = QModelIndex()) const override;
  int rowCount(QModelIndex const &parent = QModelIndex()) const override;
  int columnCount(QModelIndex const &parent = QModelIndex()) const override;
  QVariant data(QModelIndex const &index, int role = Qt::DisplayRole) const override;
  bool setData(QModelIndex const &index, QVariant const &value, int role = Qt::EditRole) override;
  Qt::ItemFlags flags(QModelIndex const &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  int getItemCount() const;

private:
  QMap<QString, T_TreeItem *> _itemsMap;
  T_TreeItem *_rootItem { nullptr };
};

#endif // T_TREEVIEWMODEL_H
