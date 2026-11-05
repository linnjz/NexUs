#ifndef NXCALENDARPICKERCONTAINER_H
#define NXCALENDARPICKERCONTAINER_H

#include <QWidget>

#include "NXDef.h"

class NXCalendarPickerContainer : public QWidget
{
  Q_OBJECT

public:
  explicit NXCalendarPickerContainer(QWidget *parent = nullptr);
  ~NXCalendarPickerContainer();

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXCALENDARPICKERCONTAINER_H
