#ifndef NXTOOLBARPRIVATE_H
#define NXTOOLBARPRIVATE_H
#include <QObject>

#include "NXDef.h"
class NXToolBarStyle;
class NXToolBar;

class NXToolBarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXToolBar)

public:
  explicit NXToolBarPrivate(QObject *parent = nullptr);
  ~NXToolBarPrivate();

private:
  NXToolBarStyle *m_ToolBarStyle { nullptr };
  NXThemeType::ThemeMode m_ThemeMode;
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  int m_ShadowBorderWidth { 0 };
#else
  int m_ShadowBorderWidth { 6 };
#endif
};

#endif // NXTOOLBARPRIVATE_H
