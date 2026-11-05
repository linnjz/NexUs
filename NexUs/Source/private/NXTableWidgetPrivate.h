#ifndef NXTABLEWIDGETPRIVATE_H
#define NXTABLEWIDGETPRIVATE_H

#include <QObject>

#include "NXProperty.h"

class NXTableWidget;
class NXTableWidgetStyle;

class NXTableWidgetPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXTableWidget)

public:
  explicit NXTableWidgetPrivate(QObject *parent = nullptr);
  ~NXTableWidgetPrivate();

private:
  NXTableWidgetStyle *m_TableWidgetStyle { nullptr };
};

#endif // NXTABLEWIDGETPRIVATE_H
