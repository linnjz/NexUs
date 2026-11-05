#ifndef NXBREADCRUMBBARDELEGATE_H
#define NXBREADCRUMBBARDELEGATE_H

#include <QModelIndex>
#include <QStyledItemDelegate>

#include "NXDef.h"

class NXBreadcrumbBarDelegate : public QStyledItemDelegate
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QPS_SCREF(QModelIndex), PressIndex)

public:
  explicit NXBreadcrumbBarDelegate(QObject *parent = nullptr);
  ~NXBreadcrumbBarDelegate();

protected:
  void paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXBREADCRUMBBARDELEGATE_H
