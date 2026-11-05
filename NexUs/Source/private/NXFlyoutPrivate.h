#ifndef NXFLYOUTPRIVATE_H
#define NXFLYOUTPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXFlyout;
class QVBoxLayout;
class QGraphicsOpacityEffect;

class NXFlyoutPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXFlyout)
  Q_PROPERTY_CREATE_D(QString, Title)
  Q_PROPERTY_CREATE_D(QString, Content)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(bool, IsLightDismiss)

public:
  explicit NXFlyoutPrivate(QObject *parent = nullptr);
  ~NXFlyoutPrivate();

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  int m_ShadowBorderWidth { 0 };
#else
  int m_ShadowBorderWidth { 8 };
#endif
  QVBoxLayout *m_MainLayout { nullptr };
  QWidget *m_ContentWidget { nullptr };
  QGraphicsOpacityEffect *m_OpacityEffect { nullptr };
  void _doShowAnimation();
};

#endif // NXFLYOUTPRIVATE_H
