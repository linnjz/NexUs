#include "NXLCDNumberPrivate.h"

#include "NXLCDNumber.h"
#include "NXTheme.h"

NXLCDNumberPrivate::NXLCDNumberPrivate(QObject *parent)
    : QObject(parent)
{
}

NXLCDNumberPrivate::~NXLCDNumberPrivate()
{
}

void NXLCDNumberPrivate::onThemeModeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXLCDNumber);
  m_ThemeMode       = themeMode;
  QPalette palette = q->palette();
  palette.setColor(QPalette::Text, NXThemeColor(m_ThemeMode, BasicText));
  palette.setColor(QPalette::WindowText, NXThemeColor(themeMode, BasicText));
  q->setPalette(palette);
}
