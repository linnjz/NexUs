#ifndef NXBREADCRUMBBAR_H
#define NXBREADCRUMBBAR_H
#include <QWidget>

#include "NXProperty.h"

class NXBreadcrumbBarPrivate;

class NX_EXPORT NXBreadcrumbBar : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXBreadcrumbBar)
  Q_PROPERTY_CREATE_H(int, TextPixelSize)
  Q_PROPERTY_CREATE_H(bool, IsAutoRemove)

public:
  explicit NXBreadcrumbBar(QWidget *parent = nullptr);
  ~NXBreadcrumbBar();
  void setBreadcrumbList(QStringList const &breadcrumbList);
  QStringList appendBreadcrumb(QString const &breadcrumb);
  QStringList removeBreadcrumb(QString const &breadcrumb);

  int getBreadcrumbListCount() const;
  QStringList getBreadcrumbList() const;
  Q_SIGNAL void breadcrumbClicked(QString const &breadcrumb, QStringList const &lastBreadcrumbList);
};

#endif // NXBREADCRUMBBAR_H
