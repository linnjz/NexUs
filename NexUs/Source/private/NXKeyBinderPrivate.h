#ifndef NEXUS_SRC_PRIVATE_NXKEYBINDERPRIVATE_H_
#define NEXUS_SRC_PRIVATE_NXKEYBINDERPRIVATE_H_

#include <QObject>

#include "NXDef.h"
class NXKeyBinder;
class NXContentDialog;
class NXKeyBinderContainer;

class NXKeyBinderPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXKeyBinder)
  Q_PROPERTY_CREATE_D(int, BorderRadius)

public:
  explicit NXKeyBinderPrivate(QObject *parent = nullptr);
  ~NXKeyBinderPrivate();
  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  NXThemeType::ThemeMode m_ThemeMode;
  NXContentDialog *m_BinderDialog { nullptr };
  NXKeyBinderContainer *m_BinderContainer { nullptr };
};

#endif //NEXUS_SRC_PRIVATE_NXKEYBINDERPRIVATE_H_
