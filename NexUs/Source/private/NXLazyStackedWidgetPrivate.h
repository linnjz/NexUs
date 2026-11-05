#ifndef NXLAZYSTACKEDWIDGETPRIVATE_H
#define NXLAZYSTACKEDWIDGETPRIVATE_H

#include <QObject>

#include "NXProperty.h"

class NXLazyStackedLayout;
class NXLazyStackedWidget;

class NXLazyStackedWidgetPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXLazyStackedWidget)

public:
  explicit NXLazyStackedWidgetPrivate(QObject *parent = nullptr);
  ~NXLazyStackedWidgetPrivate() override;

private:
  NXLazyStackedLayout *m_Layout { nullptr };
};

#endif // NXLAZYSTACKEDWIDGETPRIVATE_H
