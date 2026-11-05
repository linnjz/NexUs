#ifndef NXTREEVIEWPRIVATE_H
#define NXTREEVIEWPRIVATE_H

#include <QObject>

#include "NXProperty.h"

class NXTreeView;
class NXTreeViewStyle;

class NXTreeViewPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXTreeView)

public:
  explicit NXTreeViewPrivate(QObject *parent = nullptr);
  ~NXTreeViewPrivate();

private:
  NXTreeViewStyle *m_TreeViewStyle { nullptr };
};

#endif // NXTREEVIEWPRIVATE_H
