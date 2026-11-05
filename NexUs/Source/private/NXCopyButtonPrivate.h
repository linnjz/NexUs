#ifndef NXCOPYBUTTONPRIVATE_H
#define NXCOPYBUTTONPRIVATE_H

#include <QColor>
#include <QObject>

#include "NXDef.h"

class QTimer;
class NXCopyButton;

class NXCopyButtonPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXCopyButton)
  Q_PROPERTY_CREATE_D(QString, CopyText)
  Q_PROPERTY_CREATE_D(QString, SuccessText)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, SuccessDuration)

public:
  explicit NXCopyButtonPrivate(QObject *parent = nullptr);
  ~NXCopyButtonPrivate();

private:
  friend class NXCopyButton;
  bool m_IsPressed { false };
  bool m_IsSuccess { false };
  int m_ShadowBorderWidth { 3 };
  NXThemeType::ThemeMode m_ThemeMode;
  NXIconType::IconName m_NormalIcon { NXIconType::Copy };
  NXIconType::IconName m_SuccessIcon { NXIconType::ClipboardCheck };
  qreal m_IconOpacity { 1.0 };
  QTimer *m_ResetTimer { nullptr };
};

#endif // NXCOPYBUTTONPRIVATE_H
