#ifndef NXLISTVIEWPRIVATE_H
#define NXLISTVIEWPRIVATE_H

#include <QObject>

#include "NXProperty.h"

class NXListView;
class NXListViewStyle;

class NXListViewPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXListView)

public:
  explicit NXListViewPrivate(QObject *parent = nullptr);
  ~NXListViewPrivate();

private:
  NXListViewStyle *m_ListViewStyle { nullptr };
};

#endif // NXLISTVIEWPRIVATE_H
