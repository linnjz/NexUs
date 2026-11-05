#include "NXKeyBinderPrivate.h"

#include <QTimer>

#include "NXKeyBinder.h"
#include "NXTheme.h"

NXKeyBinderPrivate::NXKeyBinderPrivate(QObject *parent)
    : QObject(parent)
{
}

NXKeyBinderPrivate::~NXKeyBinderPrivate()
{
}

void NXKeyBinderPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXKeyBinder);
  m_ThemeMode       = themeMode;
  QPalette palette = q->palette();
  palette.setColor(QPalette::Text, NXThemeColor(m_ThemeMode, BasicText));
  palette.setColor(QPalette::WindowText, NXThemeColor(m_ThemeMode, BasicText));
  q->setPalette(palette);
}
