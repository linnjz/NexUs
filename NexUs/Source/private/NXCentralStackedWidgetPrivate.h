#ifndef NXCENTRALSTACKEDWIDGET_PAIVATE_H
#define NXCENTRALSTACKEDWIDGET_PAIVATE_H

#include <QObject>
#include <QPixmap>
#include "NXDef.h"

class QWidget;
class NXLazyStackedWidget;
class QVBoxLayout;
class QGraphicsBlurEffect;
class NXCentralStackedWidget;

class NXCentralStackedWidgetPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXCentralStackedWidget)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), LastTargetIndex)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), BlurAnimationRadius)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), PopupAnimationYOffset)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ScaleAnimationRatio)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ScaleAnimationPixOpacity)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), FlipAnimationRatio)

public:
  explicit NXCentralStackedWidgetPrivate(QObject *parent = nullptr);
  ~NXCentralStackedWidgetPrivate();

  Q_SLOT void onThemeModeChanged(NXThemeType::ThemeMode themeMode);

private:
  NXWindowType::StackSwitchMode m_StackSwitchMode { NXWindowType::StackSwitchMode::Popup };
  NXThemeType::ThemeMode m_ThemeMode;
  QPixmap m_TargetStackPix;
  QPixmap m_CurrentStackPix;
  QGraphicsBlurEffect *m_BlurEffect { nullptr };
  bool m_IsTransparent { false };
  QVBoxLayout *m_MainLayout { nullptr };
  QWidget *m_CustomWidget { nullptr };
  NXLazyStackedWidget *m_ContainerStackedWidget { nullptr };
  bool m_IsHasRadius { true };
  bool m_IsDrawNewPix { false };
  void _getTargetStackPix();
  void _getCurrentStackPix();
};

#endif // NXCENTRALSTACKEDWIDGET_PAIVATE_H
