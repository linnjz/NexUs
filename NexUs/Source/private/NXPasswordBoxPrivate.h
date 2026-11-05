#ifndef NXPASSWORDBOXPRIVATE_H
#define NXPASSWORDBOXPRIVATE_H

#include <QObject>

#include "NXDef.h"
class QAction;
class NXEvent;
class NXPasswordBox;

class NXPasswordBoxPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXPasswordBox)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ExpandMarkWidth)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(bool, IsPasswordVisible)

public:
  explicit NXPasswordBoxPrivate(QObject *parent = nullptr);
  ~NXPasswordBoxPrivate();
  Q_INVOKABLE void onWMWindowClickedEvent(QVariantMap const &data);
  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  NXThemeType::ThemeMode m_ThemeMode;
  qreal m_TextSpacing { 0.5 };
  NXEvent *m_FocusEvent { nullptr };
  QAction *m_ToggleAction { nullptr };
};

#endif // NXPASSWORDBOXPRIVATE_H
