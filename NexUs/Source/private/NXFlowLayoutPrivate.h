#ifndef NXFLOWLAYOUTPRIVATE_H
#define NXFLOWLAYOUTPRIVATE_H

#include <QLayout>
#include <QMap>
#include <QObject>
#include <QStyle>

#include "NXProperty.h"
class NXFlowLayout;

class NXFlowLayoutPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXFlowLayout)

public:
  explicit NXFlowLayoutPrivate(QObject *parent = nullptr);
  ~NXFlowLayoutPrivate();

private:
  int _doLayout(QRect rect, bool testOnly) const;
  int _smartSpacing(QStyle::PixelMetric pm) const;
  mutable QMap<QLayoutItem *, QPoint> m_LastGeometryMap;
  QList<QLayoutItem *> m_ItemList;
  bool m_IsAnimation { false };
  int m_HSpacing;
  int m_VSpacing;
};

#endif // NXFLOWLAYOUTPRIVATE_H
