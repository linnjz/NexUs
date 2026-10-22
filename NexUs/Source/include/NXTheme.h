#ifndef NXTHEME_H
#define NXTHEME_H

#include <QColor>
#include <QObject>

#include "LinnSingleton.h"
#include "NXDef.h"
#include "NXProperty.h"
#pragma push_macro("Q_DISABLE_COPY")
#undef Q_DISABLE_COPY
#define Q_DISABLE_COPY(CLASS)

#define nxTheme                             NXTheme::getInstance()
#define NXThemeColor(themeMode, themeColor) nxTheme->getThemeColor(themeMode, NXThemeType::themeColor)
class QPainter;
class NXThemePrivate;

class NX_EXPORT NXTheme : public QObject
{
  Q_OBJECT
  Q_Q_CREATE(NXTheme)
  Q_SINGLETON_CREATE(QS_S_UNIQUE(NXTheme))
  Q_PROPERTY_CREATE_H(bool, IsFollowSystemTheme, QS_SIGNAL_PARAMS(bool))

private:
  explicit NXTheme(QObject *parent = nullptr);
  ~NXTheme();

public:
  void setThemeMode(NXThemeType::ThemeMode themeMode);
  NXThemeType::ThemeMode getThemeMode() const;

  void drawEffectShadow(QPainter *painter, QRect widgetRect, int shadowBorderWidth, int borderRadius, int maxAlpha = 32, int extendPixels = 1, const QColor &lightColor = QColor(0xC0, 0xC0, 0xC0), const QColor &darkColor = QColor(0x80, 0x80, 0x80));

  void setThemeColor(NXThemeType::ThemeMode themeMode, NXThemeType::ThemeColor themeColor, const QColor &newColor);
  QColor getThemeColor(NXThemeType::ThemeMode themeMode, NXThemeType::ThemeColor themeColor);
  Q_SIGNAL void themeModeChanged(NXThemeType::ThemeMode themeMode);
};

#pragma pop_macro("Q_DISABLE_COPY")
#endif // NXTHEME_H
