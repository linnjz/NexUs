#ifndef NXFRAMEWORK_NXROLLERPICKER_H
#define NXFRAMEWORK_NXROLLERPICKER_H

#include <QPushButton>

#include "NXProperty.h"

class NXRollerPickerPrivate;

class NX_EXPORT NXRollerPicker : public QPushButton
{
  Q_OBJECT
  Q_Q_CREATE(NXRollerPicker)
  Q_PROPERTY_CREATE_H(int, BorderRadius)

public:
  explicit NXRollerPicker(QWidget *parent = nullptr);
  ~NXRollerPicker();

  void addRoller(QStringList const &itemList, bool isEnableLoop = true);
  void removeRoller(int index);

  void setRollerItemList(int index, QStringList const &itemList);
  void setRollerWidth(int index, int width);

  void setCurrentData(int index, QString const &data);
  void setCurrentData(QStringList const &dataList);
  QString getCurrentData(int index) const;
  QStringList getCurrentData() const;

  void setCurrentIndex(int rollerIndex, int index);
  void setCurrentIndex(QList<int> const &indexList);
  int getCurrentIndex(int rollerIndex) const;
  QList<int> getCurrentIndex() const;
  Q_SIGNAL void currentDataChanged(QStringList const &dataList);
  Q_SIGNAL void currentDataSelectionChanged(QStringList const &dataList);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif //NXFRAMEWORK_NXROLLERPICKER_H
