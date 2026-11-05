#ifndef NXPOPCONFIRMPRIVATE_H
#define NXPOPCONFIRMPRIVATE_H

#include <QObject>

#include "NXDef.h"
class QVBoxLayout;
class QGraphicsOpacityEffect;
class NXPushButton;
class NXPopconfirm;

class NXPopconfirmPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXPopconfirm)
  Q_PROPERTY_CREATE_D(QString, Title)
  Q_PROPERTY_CREATE_D(QString, Content)
  Q_PROPERTY_CREATE_D(QString, ConfirmButtonText)
  Q_PROPERTY_CREATE_D(QString, CancelButtonText)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(NXIconType::IconName, Icon)
  Q_PROPERTY_CREATE_D(bool, IsLightDismiss)

public:
  explicit NXPopconfirmPrivate(QObject *parent = nullptr);
  ~NXPopconfirmPrivate();

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  friend class NXPopconfirm;
  NXThemeType::ThemeMode m_ThemeMode;
  int m_ShadowBorderWidth { 8 };
  QVBoxLayout *m_MainLayout { nullptr };
  QGraphicsOpacityEffect *m_OpacityEffect { nullptr };
  NXPushButton *m_ConfirmButton { nullptr };
  NXPushButton *m_CancelButton { nullptr };
  void _doShowAnimation();
};

#endif // NXPOPCONFIRMPRIVATE_H
