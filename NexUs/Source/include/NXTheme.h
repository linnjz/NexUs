#ifndef NXTHEME_H
#define NXTHEME_H

#include <QColor>
#include <QObject>

#include "LinnSingleton.h"
#include "NXDef.h"
#include "NXProperty.h"

#define nxTheme                             NXTheme::getInstance()
#define NXThemeColor(themeMode, themeColor) nxTheme->getThemeColor(themeMode, NXThemeType::themeColor)
class QPainter;
class NXThemePrivate;

class NX_EXPORT NXTheme : public QObject
{
  Q_OBJECT
  Q_Q_CREATE(NXTheme)
  Q_SINGLETON_CREATE(QSS_UNIQUE(NXTheme))
  Q_PROPERTY_CREATE_H(bool, IsFollowSystemTheme)

private:
  explicit NXTheme(QObject *parent = nullptr);
  ~NXTheme();

public:
  void setThemeMode(NXThemeType::ThemeMode themeMode);
  NXThemeType::ThemeMode getThemeMode() const;

  void drawEffectShadow(QPainter *painter, QRect widgetRect, int shadowBorderWidth, int borderRadius, int maxAlpha = 32, int extendPixels = 1, QColor const &lightColor = QColor(0xC0, 0xC0, 0xC0), QColor const &darkColor = QColor(0x80, 0x80, 0x80));

  void setThemeColor(NXThemeType::ThemeMode themeMode, NXThemeType::ThemeColor themeColor, QColor const &newColor);
  QColor getThemeColor(NXThemeType::ThemeMode themeMode, NXThemeType::ThemeColor themeColor);
  Q_SIGNAL void themeModeChanged(NXThemeType::ThemeMode themeMode);
};

#endif // NXTHEME_H
