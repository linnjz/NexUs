#ifndef NXRIBBONBARPRIVATE_H
#define NXRIBBONBARPRIVATE_H

#include <QObject>
#include <QPointer>

#include "NXDef.h"
#include "NXProperty.h"

class QStackedWidget;
class QTimer;
class QVariantAnimation;
class NXRibbonBar;
class NXRibbonTabBar;

class NXRibbonBarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXRibbonBar)
  Q_PROPERTY_CREATE_D(int, CurrentIndex)

public:
  explicit NXRibbonBarPrivate(QObject *parent = nullptr);
  ~NXRibbonBarPrivate();

  NXRibbonTabBar *activeTabBar() const;

private:
  bool m_IsCollapsed { false };
  bool m_IsPinned { false };
  NXThemeType::ThemeMode m_ThemeMode;
  int m_AnimationDuration { 200 };
  QPointer<NXRibbonTabBar> m_ExternalTabBar;
  QStackedWidget *m_Stack { nullptr };
  NXRibbonTabBar *m_InternalTabBar { nullptr };
  QVariantAnimation *m_HeightAnim { nullptr };
  QTimer *m_AutoHideTimer { nullptr };
};

#endif // NXRIBBONBARPRIVATE_H
