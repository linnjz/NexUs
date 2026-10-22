#include "NXRadioButtonPrivate.h"

#include "NXRadioButton.h"
#include "NXTheme.h"

NXRadioButtonPrivate::NXRadioButtonPrivate(QObject *parent)
    : QObject(parent)
{
}

NXRadioButtonPrivate::~NXRadioButtonPrivate()
{
}

void NXRadioButtonPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXRadioButton);
  _themeMode       = themeMode;
  QPalette palette = q->palette();
  palette.setColor(QPalette::Text, NXThemeColor(_themeMode, BasicText));
  palette.setColor(QPalette::WindowText, NXThemeColor(themeMode, BasicText));
  q->setPalette(palette);
}
