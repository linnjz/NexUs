#ifndef NXBREADCRUMBBARMODEL_H
#define NXBREADCRUMBBARMODEL_H

#include <QAbstractListModel>

class NXBreadcrumbBarModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit NXBreadcrumbBarModel(QObject *parent = nullptr);
  ~NXBreadcrumbBarModel();

  void appendBreadcrumb(const QString &breadcrumb);
  void removeBreadcrumb(const QString &breadcrumb);
  void removeBreadcrumb(int index);

  void setBreadcrumbList(const QStringList &breadcrumbList);

  int getBreadcrumbListCount() const;
  QStringList getBreadcrumbList() const;

protected:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;

private:
  QStringList _breadcrumbList;
};

#endif // NXBREADCRUMBBARMODEL_H
