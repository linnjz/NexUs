#include "NXCalendarPickerContainer.h"

#include <QPainter>

#include "NXTheme.h"

NXCalendarPickerContainer::NXCalendarPickerContainer(QWidget *parent)
    : QWidget { parent }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  setWindowFlags(Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setContentsMargins(0, 0, 0, 0);
  setObjectName("NXCalendarPickerContainer");
  setStyleSheet(QStringLiteral("#NXCalendarPickerContainer{background-color:transparent}"));
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
    update();
  });
}

NXCalendarPickerContainer::~NXCalendarPickerContainer()
{
}

void NXCalendarPickerContainer::paintEvent(QPaintEvent *event)
{
#if !(defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0))
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing);
  nxTheme->drawEffectShadow(&painter, rect(), 6, 5);
  painter.restore();
#endif
}
