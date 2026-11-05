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
  m_ThemeMode       = themeMode;
  QPalette palette = q->palette();
  palette.setColor(QPalette::Text, NXThemeColor(m_ThemeMode, BasicText));
  palette.setColor(QPalette::WindowText, NXThemeColor(m_ThemeMode, BasicText));
  q->setPalette(palette);
}
