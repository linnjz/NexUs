#ifndef NXSTACKBARPRIVATE_H
#define NXSTACKBARPRIVATE_H

#include <QObject>

#include "NXDef.h"
#include "NXSnackbar.h"

class NXSnackbarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXSnackbar)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, DisplayMsec)

public:
  explicit NXSnackbarPrivate(QObject *parent = nullptr);
  ~NXSnackbarPrivate();

private:
  bool m_IsActionHover { false };
  bool m_IsActionPressed { false };
  bool m_IsClosing { false };
  NXThemeType::ThemeMode m_ThemeMode;
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  int m_ShadowBorderWidth { 0 };
#else
  int m_ShadowBorderWidth { 6 };
#endif
  NXSnackbar::SnackbarType m_SnackbarType { NXSnackbar::Info };
  int m_BaseBottomY { 0 };
  int m_CenterX { 0 };
  QRect m_ActionRect;
  QString m_Text;
  QString m_ActionText;
  std::function<void()> m_DoClose;
};

#endif // NXSTACKBARPRIVATE_H
