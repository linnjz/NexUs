#ifndef NXTABLEVIEWPRIVATE_H
#define NXTABLEVIEWPRIVATE_H

#include <QObject>

#include "NXProperty.h"

class NXTableView;
class NXTableViewStyle;

class NXTableViewPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXTableView)

public:
  explicit NXTableViewPrivate(QObject *parent = nullptr);
  ~NXTableViewPrivate();

private:
  NXTableViewStyle *m_TableViewStyle { nullptr };
};

#endif // NXTABLEVIEWPRIVATE_H
