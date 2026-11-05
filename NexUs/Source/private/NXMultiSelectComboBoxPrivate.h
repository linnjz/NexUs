#ifndef NXMULTISELECTCOMBOBOXPRIVATE_H
#define NXMULTISELECTCOMBOBOXPRIVATE_H
#include <QObject>
#include <QVector>

#include "NXDef.h"
class NXComboBoxStyle;
class NXComboBoxView;
class NXMultiSelectComboBox;
class NXMultiSelectComboBoxDelegate;

class NXMultiSelectComboBoxPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXMultiSelectComboBox)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ExpandIconRotate)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ExpandMarkWidth)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(bool, ShowCheckBox)

public:
  explicit NXMultiSelectComboBoxPrivate(QObject *parent = nullptr);
  ~NXMultiSelectComboBoxPrivate();
  Q_SLOT void onItemPressed(QModelIndex const &index);

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  bool m_IsFirstPopup { false };
  bool m_IsAllowHidePopup { false };
  NXThemeType::ThemeMode m_ThemeMode;
  QVector<bool> m_ItemSelection;
  QString m_CurrentText;
  QStringList m_SelectedTextList;
  NXComboBoxStyle *m_ComboBoxStyle { nullptr };
  NXComboBoxView *m_ComboView { nullptr };
  NXMultiSelectComboBoxDelegate *m_Delegate { nullptr };
  void _refreshCurrentIndexs();
  void _adjustSelectedVector();
};

#endif // NXMULTISELECTCOMBOBOXPRIVATE_H
