#ifndef NXSPOTLIGHTPRIVATE_H
#define NXSPOTLIGHTPRIVATE_H

#include <QObject>
#include <QRectF>

#include "NXDef.h"
#include "NXSpotlight.h"
class QPropertyAnimation;
class NXText;
class NXPushButton;
class NXSpotlight;

class NXSpotlightPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXSpotlight)
  Q_PROPERTY_CREATE(QPS_PAEMIT(QRectF), SpotlightRect)
  Q_PROPERTY_CREATE_D(QString, Title)
  Q_PROPERTY_CREATE_D(QString, Content)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), Opacity)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, Padding)
  Q_PROPERTY_CREATE_D(int, OverlayAlpha)
  Q_PROPERTY_CREATE_D(bool, IsCircle)

public:
  explicit NXSpotlightPrivate(QObject *parent = nullptr);
  ~NXSpotlightPrivate();

  void _showStep(int index, bool animate = true);
  void _updateOverlay();
  void _updateTipPosition();
  QRectF _getTargetRect(QWidget *target) const;

private:
  friend class NXSpotlight;
  bool m_IsActive { false };
  NXThemeType::ThemeMode m_ThemeMode;
  int m_CurrentStep { -1 };
  int m_MaskAlpha { 0 };
  QList<NXSpotlight::SpotlightStep> m_Steps;
  QWidget *m_CurrentTarget { nullptr };
  QWidget *m_TipWidget { nullptr };
  NXText *m_TipTitle { nullptr };
  NXText *m_TipContent { nullptr };
  NXPushButton *m_PrevButton { nullptr };
  NXPushButton *m_NextButton { nullptr };
  NXText *m_StepIndicator { nullptr };
};

#endif // NXSPOTLIGHTPRIVATE_H
