#ifndef NXCALENDARTITLEMODEL_H
#define NXCALENDARTITLEMODEL_H

#include <QAbstractListModel>

class NXCalendarTitleModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit NXCalendarTitleModel(QObject *parent = nullptr);
  ~NXCalendarTitleModel();

protected:
  virtual int rowCount(QModelIndex const &parent = QModelIndex()) const override;
  QVariant data(QModelIndex const &index, int role) const override;
};

#endif // NXCALENDARTITLEMODEL_H
