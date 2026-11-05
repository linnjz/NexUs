#ifndef NXMULTISELECTCOMBOBOX_H
#define NXMULTISELECTCOMBOBOX_H
#include <QComboBox>

#include "NXProperty.h"
class NXMultiSelectComboBoxPrivate;

class NX_EXPORT NXMultiSelectComboBox : public QComboBox
{
  Q_OBJECT
  Q_Q_CREATE(NXMultiSelectComboBox)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(bool, ShowCheckBox)

public:
  explicit NXMultiSelectComboBox(QWidget *parent = nullptr);
  ~NXMultiSelectComboBox();
  void setCurrentSelection(QString const &selection);
  void setCurrentSelection(QStringList const &selection);
  void setCurrentSelection(int index);
  void setCurrentSelection(QList<int> const &selectionIndex);
  QStringList getCurrentSelection() const;
  QList<int> getCurrentSelectionIndex() const;
  Q_SIGNAL void itemSelectionChanged(QVector<bool> const &itemSelection);
  Q_SIGNAL void currentTextListChanged(QStringList const &selectedTextList);

protected:
  void paintEvent(QPaintEvent *e) override;
  void showPopup() override;
  void hidePopup() override;

private:
  friend class NXMultiSelectComboBoxPrivate;
  void _resetIndicatorAnimations();
};

#endif // NXMULTISELECTCOMBOBOX_H
