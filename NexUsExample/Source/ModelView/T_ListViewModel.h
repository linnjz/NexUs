#ifndef T_LISTVIEWMODEL_H
#define T_LISTVIEWMODEL_H

#include <QAbstractListModel>

class T_ListViewModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit T_ListViewModel(QObject *parent = nullptr);
  ~T_ListViewModel();

protected:
  int rowCount(QModelIndex const &parent = QModelIndex()) const override;
  QVariant data(QModelIndex const &index, int role) const override;

private:
  QStringList _dataList;
};

#endif // T_LISTVIEWMODEL_H
