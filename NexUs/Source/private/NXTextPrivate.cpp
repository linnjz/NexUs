#include "NXTextPrivate.h"

#include "NXText.h"
#include "NXTheme.h"

NXTextPrivate::NXTextPrivate(QObject *parent)
    : QObject { parent }
{
}

NXTextPrivate::~NXTextPrivate()
{
}

void NXTextPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXText);
  _themeMode       = themeMode;
  QPalette palette = q->palette();
  palette.setColor(QPalette::Text, NXThemeColor(_themeMode, BasicText));
  palette.setColor(QPalette::WindowText, NXThemeColor(_themeMode, BasicText));
  q->setPalette(palette);
}
