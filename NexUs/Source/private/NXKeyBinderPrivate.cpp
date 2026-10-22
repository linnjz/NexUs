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
  _themeMode       = themeMode;
  QPalette palette = q->palette();
  palette.setColor(QPalette::Text, NXThemeColor(_themeMode, BasicText));
  palette.setColor(QPalette::WindowText, NXThemeColor(_themeMode, BasicText));
  q->setPalette(palette);
}
