#ifndef NXMULTISELECTCOMBOBOXDELEGATE_H
#define NXMULTISELECTCOMBOBOXDELEGATE_H

#include <QStyledItemDelegate>

#include "NXDef.h"

class NXMultiSelectComboBoxDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  explicit NXMultiSelectComboBoxDelegate(QObject *parent = nullptr);
  ~NXMultiSelectComboBoxDelegate();

  void setItemSelection(QVector<bool> const &selection);

protected:
  void paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const override;
  QSize sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  QVector<bool> m_ItemSelection;
};

#endif // NXMULTISELECTCOMBOBOXDELEGATE_H
