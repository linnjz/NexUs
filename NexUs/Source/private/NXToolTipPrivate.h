#ifndef NXTOOLTIPPRIVATE_H
#define NXTOOLTIPPRIVATE_H

#include <QObject>
#include <QPoint>
#include <QTimer>

#include "NXDef.h"
class NXToolTip;
class QVBoxLayout;
class NXText;

class NXToolTipPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXToolTip)
  Q_PROPERTY_CREATE_D(QWidget *, CustomWidget)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), Opacity)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, DisplayMsec)
  Q_PROPERTY_CREATE_D(int, ShowDelayMsec)
  Q_PROPERTY_CREATE_D(int, HideDelayMsec)
  Q_PROPERTY_CREATE_D(int, OffSetX)
  Q_PROPERTY_CREATE_D(int, OffSetY)
  Q_PROPERTY_CREATE_D(bool, IsMoveEnable)

public:
  explicit NXToolTipPrivate(QObject *parent = nullptr);
  ~NXToolTipPrivate();

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private Q_SLOTS:
  void onShowTimeout();
  void onHideTimeout();
  void onAutoHideTimeout();

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_ShadowBorderWidth { 6 };
  NXText *m_ToolTipText { nullptr };
  QVBoxLayout *m_MainLayout { nullptr };

  QTimer *m_ShowTimer;
  QTimer *m_HideTimer;
  QTimer *m_AutoHideTimer;

  void _doShowAnimation();
  void _updatePos();
  void _stopAllTimers();
};

#endif // NXTOOLTIPPRIVATE_H
