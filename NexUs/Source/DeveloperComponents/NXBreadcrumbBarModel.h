#ifndef NXBREADCRUMBBARMODEL_H
#define NXBREADCRUMBBARMODEL_H

#include <QAbstractListModel>

class NXBreadcrumbBarModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit NXBreadcrumbBarModel(QObject *parent = nullptr);
  ~NXBreadcrumbBarModel();

  void appendBreadcrumb(QString const &breadcrumb);
  void removeBreadcrumb(QString const &breadcrumb);
  void removeBreadcrumb(int index);

  void setBreadcrumbList(QStringList const &breadcrumbList);

  int getBreadcrumbListCount() const;
  QStringList getBreadcrumbList() const;

protected:
  int rowCount(QModelIndex const &parent = QModelIndex()) const override;
  QVariant data(QModelIndex const &index, int role) const override;

private:
  QStringList m_BreadcrumbList;
};

#endif // NXBREADCRUMBBARMODEL_H
