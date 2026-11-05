#ifndef NXFRAMEWORK_NXROLLERPICKERPRIVATE_H
#define NXFRAMEWORK_NXROLLERPICKERPRIVATE_H
#include <QObject>

#include "DeveloperComponents/NXRollerPickerContainer.h"
#include "NXDef.h"
#include "NXRoller.h"
class QHBoxLayout;
class NXRollerPicker;

class NXRollerPickerPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXRollerPicker)
  Q_PROPERTY_CREATE_D(int, BorderRadius)

public:
  explicit NXRollerPickerPrivate(QObject *parent = nullptr);
  ~NXRollerPickerPrivate();
  Q_SLOT void onRollerPickerClicked();
  Q_SLOT void onOverButtonClicked();
  Q_SLOT void onCancelButtonClicked();

private:
  int m_PickerRollerHeight { 245 };
  NXThemeType::ThemeMode m_ThemeMode;
  QHBoxLayout *m_ContainerLayout { nullptr };
  NXRollerPickerContainer *m_RollerPickerContainer { nullptr };
  int _getRollerTotalWidth() const;
};

#endif //NXFRAMEWORK_NXROLLERPICKERPRIVATE_H
