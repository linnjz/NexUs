#ifndef T_LOGMODEL_H
#define T_LOGMODEL_H

#include <QAbstractListModel>

class T_LogModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit T_LogModel(QObject *parent = nullptr);
  ~T_LogModel();
  void setLogList(QStringList const &list);
  void appendLogList(QString const &log);
  QStringList getLogList() const;

protected:
  int rowCount(QModelIndex const &parent = QModelIndex()) const override;
  QVariant data(QModelIndex const &index, int role) const override;

private:
  QStringList _logList;
};

#endif // T_LOGMODEL_H
