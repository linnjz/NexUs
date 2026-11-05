#ifndef NXTHEMEPRIVATE_H
#define NXTHEMEPRIVATE_H

#include <QColor>
#include <QMap>
#include <QMetaObject>
#include <QObject>

#include "NXDef.h"
class NXTheme;

class NXThemePrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXTheme)
  Q_PROPERTY_CREATE_D(bool, IsFollowSystemTheme, false)

public:
  explicit NXThemePrivate(QObject *parent = nullptr);
  ~NXThemePrivate();

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  NXThemeType::ThemeMode m_ThemeMode { NXThemeType::Light };
  QMetaObject::Connection m_ColorSchemeConnection;
  QColor m_LightThemeColorList[48];
  QColor m_DarkThemeColorList[48];
  void _initThemeColor();

  NXThemeType::ThemeMode _detectSystemThemeMode() const;
  void _applySystemTheme();
};

#endif // NXTHEMEPRIVATE_H
