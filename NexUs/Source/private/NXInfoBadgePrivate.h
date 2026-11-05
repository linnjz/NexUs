#ifndef NXINFOBADGEPRIVATE_H
#define NXINFOBADGEPRIVATE_H

#include <QObject>

#include "NXDef.h"
#include "NXInfoBadge.h"

class NXInfoBadgePrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXInfoBadge)
  Q_PROPERTY_CREATE_D(int, Value)
  Q_PROPERTY_CREATE_D(NXIconType::IconName, NXIcon)

public:
  explicit NXInfoBadgePrivate(QObject *parent = nullptr);
  ~NXInfoBadgePrivate();

private:
  NXInfoBadge::BadgeMode m_BadgeMode { NXInfoBadge::Dot };
  NXInfoBadge::Severity m_Severity { NXInfoBadge::Attention };
  int m_MaxValue { 99 };
  NXThemeType::ThemeMode m_ThemeMode;
  QWidget *m_Target { nullptr };
  void _updatePosition();
  QColor _getSeverityColor() const;
};

#endif // NXINFOBADGEPRIVATE_H
