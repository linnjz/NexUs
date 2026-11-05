#ifndef NXWIZARDPRIVATE_H
#define NXWIZARDPRIVATE_H

#include <QObject>
#include <QStringList>

#include "NXDef.h"

class NXWizard;
class QStackedWidget;

class NXWizardPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXWizard)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, CurrentStep)

public:
  NXThemeType::ThemeMode m_ThemeMode;
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  int m_ShadowBorderWidth { 0 };
#else
  int m_ShadowBorderWidth { 10 };
#endif
  QRect m_CancelButtonRect;
  QRect m_PrevButtonRect;
  QRect m_NextButtonRect;
  QStringList m_StepTitles;
  QList<QWidget *> m_StepPages;
  QStackedWidget *m_StackedWidget { nullptr };
  explicit NXWizardPrivate(QObject *parent = nullptr);
  ~NXWizardPrivate();
};

#endif // NXWIZARDPRIVATE_H
