#ifndef NXTEACHINGTIPPRIVATE_H
#define NXTEACHINGTIPPRIVATE_H

#include <functional>

#include <QObject>

#include "NXDef.h"
#include "NXTeachingTip.h"
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QPropertyAnimation;

class NXTeachingTipPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXTeachingTip)
  Q_PROPERTY_CREATE_D(QString, Title)
  Q_PROPERTY_CREATE_D(QString, SubTitle)
  Q_PROPERTY_CREATE_D(QString, Content)
  Q_PROPERTY_CREATE_D(QPixmap, HeroImage)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), Opacity)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(NXIconType::IconName, TipIcon)
  Q_PROPERTY_CREATE_D(bool, IsLightDismiss)

public:
  explicit NXTeachingTipPrivate(QObject *parent = nullptr);
  ~NXTeachingTipPrivate();

private:
  struct ActionItem
  {
    QString text;
    std::function<void()> callback;
  };

  bool m_CloseButtonVisible { true };
  NXTeachingTip::TailPosition m_TailPosition { NXTeachingTip::Auto };
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  int m_ShadowBorderWidth { 0 };
#else
  int m_ShadowBorderWidth { 8 };
#endif
  int m_TailSize { 8 };
  NXThemeType::ThemeMode m_ThemeMode;
  QRect m_CloseButtonRect;
  QList<ActionItem> m_Actions;

  QWidget *m_Target { nullptr };


  QPoint _calculatePosition() const;
  NXTeachingTip::TailPosition _calculateAutoTailPosition() const;
  void _doShowAnimation();
};

#endif // NXTEACHINGTIPPRIVATE_H
