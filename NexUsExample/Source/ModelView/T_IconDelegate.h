#ifndef T_ICONDELEGATE_H
#define T_ICONDELEGATE_H

#include <QStyledItemDelegate>

#include "NXDef.h"

class T_IconDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  explicit T_IconDelegate(QObject *parent = nullptr);
  ~T_IconDelegate();

protected:
  void paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const override;
  QSize sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const override;

private:
  NXThemeType::ThemeMode _themeMode;
};

#endif // T_ICONDELEGATE_H
