#include "NXGroupBoxPrivate.h"

#include "NXGroupBox.h"
#include "NXTheme.h"

NXGroupBoxPrivate::NXGroupBoxPrivate(QObject *parent)
    : QObject(parent)
    , m_BorderRadius { 6 }
{
}

NXGroupBoxPrivate::~NXGroupBoxPrivate()
{
}

void NXGroupBoxPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXGroupBox);
  m_ThemeMode       = themeMode;
  QPalette palette = q->palette();
  palette.setColor(QPalette::WindowText, NXThemeColor(themeMode, BasicText));
  q->setPalette(palette);
}
