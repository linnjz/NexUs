#ifndef NXPIVOTMODEL_H
#define NXPIVOTMODEL_H

#include <QAbstractListModel>
#include <QPixmap>

#include "NXDef.h"

class NXPivotModel : public QAbstractListModel
{
  Q_OBJECT

public:
  struct PivotItem
  {
    QString text;
    QPixmap icon;
  };

  explicit NXPivotModel(QObject *parent = nullptr);
  ~NXPivotModel() override;

  void appendPivot(QPixmap const &pixmap);
  void appendPivot(QString const &text, QPixmap const &pixmap = {});

  void insertPivot(int index, QPixmap const &pixmap);
  void insertPivot(int index, QString const &text, QPixmap const &pixmap = {});

  void removePivot(int index);
  void removePivot(QString const &text);

  void setPivot(int index, QPixmap const &pixmap);
  void setPivot(int index, QString const &text, QPixmap const &pixmap = {});

  void clearPivot();

  int getPivotListCount() const;
  QList<PivotItem> const &getPivotList() const;
  int getPivotIndex(QString const &text) const;

  int rowCount(QModelIndex const &parent = QModelIndex()) const override;
  QVariant data(QModelIndex const &index, int role) const override;

private:
  QList<PivotItem> m_PivotList;
};

Q_DECLARE_METATYPE(NXPivotModel::PivotItem);

#endif // NXPIVOTMODEL_H
