#ifndef NXRIBBONGROUPPRIVATE_H
#define NXRIBBONGROUPPRIVATE_H

#include <QObject>

#include "NXDef.h"
#include "NXProperty.h"

class QHBoxLayout;
class NXRibbonGroup;
class NXToolButton;

class NXRibbonGroupPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXRibbonGroup)

public:
  explicit NXRibbonGroupPrivate(QObject *parent = nullptr);
  ~NXRibbonGroupPrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
  QHBoxLayout *m_ContentLayout { nullptr };
  QString m_Title;
};

#endif // NXRIBBONGROUPPRIVATE_H
