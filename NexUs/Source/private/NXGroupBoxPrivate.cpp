#include "NXGroupBoxPrivate.h"

#include "NXGroupBox.h"
#include "NXTheme.h"

NXGroupBoxPrivate::NXGroupBoxPrivate(QObject *parent)
    : QObject(parent)
{
  _pBorderRadius = 6;
}

NXGroupBoxPrivate::~NXGroupBoxPrivate()
{
}

void NXGroupBoxPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXGroupBox);
  _themeMode       = themeMode;
  QPalette palette = q->palette();
  palette.setColor(QPalette::WindowText, NXThemeColor(themeMode, BasicText));
  q->setPalette(palette);
}
