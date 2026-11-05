#ifndef NXTOASTPRIVATE_H
#define NXTOASTPRIVATE_H

#include <QObject>

#include "NXDef.h"
#include "NXToast.h"

class NXToastPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXToast)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, DisplayMsec)

public:
  explicit NXToastPrivate(QObject *parent = nullptr);
  ~NXToastPrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  int m_ShadowBorderWidth { 0 };
#else
  int m_ShadowBorderWidth { 6 };
#endif
  NXToast::ToastType m_ToastType { NXToast::Info };
  QString m_Text;
};

#endif // NXTOASTPRIVATE_H
