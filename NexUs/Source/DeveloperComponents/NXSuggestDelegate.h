#ifndef NXSUGGESTDELEGATE_H
#define NXSUGGESTDELEGATE_H

#include <QStyledItemDelegate>

#include "NXDef.h"

class NXSuggestDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  explicit NXSuggestDelegate(QObject *parent = nullptr);
  ~NXSuggestDelegate();

protected:
  void paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const override;
  QSize sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXSUGGESTDELEGATE_H
