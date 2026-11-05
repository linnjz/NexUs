#ifndef T_TABLEVIEWMODEL_H
#define T_TABLEVIEWMODEL_H

#include <QAbstractTableModel>

class T_TableViewModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit T_TableViewModel(QObject *parent = nullptr);
  ~T_TableViewModel();
  int rowCount(QModelIndex const &parent = QModelIndex()) const override;
  int columnCount(QModelIndex const &parent = QModelIndex()) const override;

  QVariant data(QModelIndex const &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
  QStringList _header;
  QList<QStringList> _dataList;
  QList<QIcon> _iconList;
};

#endif // T_TABLEVIEWMODEL_H
