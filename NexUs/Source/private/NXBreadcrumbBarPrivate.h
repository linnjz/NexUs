#ifndef NXBREADCRUMBBARPRIVATE_H
#define NXBREADCRUMBBARPRIVATE_H

#include <QObject>

#include "NXProperty.h"
class NXBaseListView;
class NXBreadcrumbBarModel;
class NXBreadcrumbBarDelegate;

class NXBreadcrumbBar;

class NXBreadcrumbBarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXBreadcrumbBar)
  Q_PROPERTY_CREATE_D(int, TextPixelSize)
  Q_PROPERTY_CREATE_D(bool, IsAutoRemove)

public:
  explicit NXBreadcrumbBarPrivate(QObject *parent = nullptr);
  ~NXBreadcrumbBarPrivate();

private:
  NXBreadcrumbBarModel *m_ListModel { nullptr };
  NXBreadcrumbBarDelegate *m_ListDelegate { nullptr };
  NXBaseListView *m_ListView { nullptr };
};

#endif // NXBREADCRUMBBARPRIVATE_H
