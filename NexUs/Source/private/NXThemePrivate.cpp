#include "NXThemePrivate.h"

#include <QApplication>
#include <QEvent>
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>

#include "NXTheme.h"

NXThemePrivate::NXThemePrivate(QObject *parent)
    : QObject { parent }
{
}

NXThemePrivate::~NXThemePrivate()
{
}

bool NXThemePrivate::eventFilter(QObject *watched, QEvent *event)
{
  if (_pIsFollowSystemTheme && event->type() == QEvent::ApplicationPaletteChange) {
    _applySystemTheme();
  }
  return QObject::eventFilter(watched, event);
}

NXThemeType::ThemeMode NXThemePrivate::_detectSystemThemeMode() const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
  const Qt::ColorScheme scheme = QGuiApplication::styleHints()->colorScheme();
  if (scheme == Qt::ColorScheme::Dark) {
    return NXThemeType::Dark;
  }
  if (scheme == Qt::ColorScheme::Light) {
    return NXThemeType::Light;
  }
#endif
  const QColor windowColor = QApplication::palette().color(QPalette::Window);
  return windowColor.lightness() < 128 ? NXThemeType::Dark : NXThemeType::Light;
}

void NXThemePrivate::_applySystemTheme()
{
  Q_Q(NXTheme);
  const NXThemeType::ThemeMode mode = _detectSystemThemeMode();
  if (_themeMode != mode) {
    q->setThemeMode(mode);
  }
}

void NXThemePrivate::_initThemeColor()
{
  // NXTabBar
  _lightThemeColorList[NXThemeType::TabBarBase] = QColor(0xEA, 0xEA, 0xED);
  _darkThemeColorList[NXThemeType::TabBarBase]  = QColor(0x1C, 0x20, 0x27);

  _lightThemeColorList[NXThemeType::TabBarSelected] = QColor(0xFF, 0xFF, 0xFF);
  _darkThemeColorList[NXThemeType::TabBarSelected]  = QColor(0x38, 0x3B, 0x43);

  _lightThemeColorList[NXThemeType::TabBarHover] = QColor(0xD2, 0xD2, 0xD6);
  _darkThemeColorList[NXThemeType::TabBarHover]  = QColor(0x3B, 0x47, 0x5E);

  _lightThemeColorList[NXThemeType::TabBarCloseButtonHover] = QColor(0xBF, 0xBF, 0xC3);
  _darkThemeColorList[NXThemeType::TabBarCloseButtonHover]  = QColor(0x5A, 0x64, 0x77);

  _lightThemeColorList[NXThemeType::TabBarSelectedCloseButtonHover] = QColor(0xE7, 0xE7, 0xE8);
  _darkThemeColorList[NXThemeType::TabBarSelectedCloseButtonHover]  = QColor(0x57, 0x5A, 0x60);

  // NXScrollBar
  _lightThemeColorList[NXThemeType::ScrollBarHandle] = QColor(0x8A, 0x8A, 0x8A);
  _darkThemeColorList[NXThemeType::ScrollBarHandle]  = QColor(0x9F, 0x9F, 0x9F);

  // NXToggleSwitch
  _lightThemeColorList[NXThemeType::ToggleSwitchNoToggledCenter] = QColor(0x6A, 0x6A, 0x6A);
  _darkThemeColorList[NXThemeType::ToggleSwitchNoToggledCenter]  = QColor(0xD0, 0xD0, 0xD0);

  // 主题颜色
  _lightThemeColorList[NXThemeType::PrimaryNormal] = QColor(0x00, 0x67, 0xC0);
  _darkThemeColorList[NXThemeType::PrimaryNormal]  = QColor(0x4A, 0xBC, 0xF7);
  _lightThemeColorList[NXThemeType::PrimaryHover]  = QColor(0x19, 0x75, 0xC5);
  _darkThemeColorList[NXThemeType::PrimaryHover]   = QColor(0x47, 0xB1, 0xE8);
  _lightThemeColorList[NXThemeType::PrimaryPress]  = QColor(0x31, 0x83, 0xCA);
  _darkThemeColorList[NXThemeType::PrimaryPress]   = QColor(0x42, 0xA1, 0xD2);

  // 通用颜色
  // 普通窗体
  _lightThemeColorList[NXThemeType::WindowBase]             = QColor(0xEC, 0xEC, 0xEC);
  _darkThemeColorList[NXThemeType::WindowBase]              = QColor(0x20, 0x20, 0x20);
  _lightThemeColorList[NXThemeType::WindowCentralStackBase] = QColor(0xF8, 0xF8, 0xF8, 90);
  _darkThemeColorList[NXThemeType::WindowCentralStackBase]  = QColor(0x3E, 0x3E, 0x3E, 60);

  // 浮动窗体
  _lightThemeColorList[NXThemeType::PopupBorder]      = QColor(0xC8, 0xC8, 0xC8);
  _darkThemeColorList[NXThemeType::PopupBorder]       = QColor(0x47, 0x47, 0x47);
  _lightThemeColorList[NXThemeType::PopupBorderHover] = QColor(0xBC, 0xBC, 0xBC);
  _darkThemeColorList[NXThemeType::PopupBorderHover]  = QColor(0x54, 0x54, 0x54);
  _lightThemeColorList[NXThemeType::PopupBase]        = QColor(0xF6, 0xF6, 0xF6);
  _darkThemeColorList[NXThemeType::PopupBase]         = QColor(0x2C, 0x2C, 0x2C);
  _lightThemeColorList[NXThemeType::PopupHover]       = QColor(0xEE, 0xEE, 0xEE);
  _darkThemeColorList[NXThemeType::PopupHover]        = QColor(0x38, 0x38, 0x38);

  // Dialog窗体
  _lightThemeColorList[NXThemeType::DialogBase]       = QColor(0xF7, 0xF7, 0xF7);
  _darkThemeColorList[NXThemeType::DialogBase]        = QColor(0x1F, 0x1F, 0x1F);
  _lightThemeColorList[NXThemeType::DialogLayoutArea] = QColor(0xED, 0xED, 0xED);
  _darkThemeColorList[NXThemeType::DialogLayoutArea]  = QColor(0x20, 0x20, 0x20);

  // 基础颜色
  _lightThemeColorList[NXThemeType::BasicText]          = Qt::black;
  _darkThemeColorList[NXThemeType::BasicText]           = Qt::white;
  _lightThemeColorList[NXThemeType::BasicTextInvert]    = Qt::white;
  _darkThemeColorList[NXThemeType::BasicTextInvert]     = Qt::black;
  _lightThemeColorList[NXThemeType::BasicDetailsText]   = QColor(0x87, 0x87, 0x87);
  _darkThemeColorList[NXThemeType::BasicDetailsText]    = QColor(0xAD, 0xAD, 0xB0);
  _lightThemeColorList[NXThemeType::BasicTextNoFocus]   = QColor(0x86, 0x86, 0x8A);
  _darkThemeColorList[NXThemeType::BasicTextNoFocus]    = QColor(0x86, 0x86, 0x8A);
  _lightThemeColorList[NXThemeType::BasicTextDisable]   = QColor(0xB6, 0xB6, 0xB6);
  _darkThemeColorList[NXThemeType::BasicTextDisable]    = QColor(0xA7, 0xA7, 0xA7);
  _lightThemeColorList[NXThemeType::BasicTextPress]     = QColor(0x5A, 0x5A, 0x5D);
  _darkThemeColorList[NXThemeType::BasicTextPress]      = QColor(0xBB, 0xBB, 0xBF);
  _lightThemeColorList[NXThemeType::BasicTextCategory]  = QColor(0x5C, 0x5C, 0x5F);
  _darkThemeColorList[NXThemeType::BasicTextCategory]   = QColor(0xCC, 0xCC, 0xCE);
  _lightThemeColorList[NXThemeType::BasicBorder]        = QColor(0xD6, 0xD6, 0xD6);
  _darkThemeColorList[NXThemeType::BasicBorder]         = QColor(0x4B, 0x4B, 0x4B);
  _lightThemeColorList[NXThemeType::BasicBorderDeep]    = QColor(0x9A, 0x9A, 0x9A);
  _darkThemeColorList[NXThemeType::BasicBorderDeep]     = QColor(0x5C, 0x5C, 0x5C);
  _lightThemeColorList[NXThemeType::BasicBorderHover]   = QColor(0xC8, 0xC8, 0xC8);
  _darkThemeColorList[NXThemeType::BasicBorderHover]    = QColor(0x57, 0x57, 0x57);
  _lightThemeColorList[NXThemeType::BasicBase]          = QColor(0xFD, 0xFD, 0xFD);
  _darkThemeColorList[NXThemeType::BasicBase]           = QColor(0x34, 0x34, 0x34);
  _lightThemeColorList[NXThemeType::BasicBaseDeep]      = QColor(0xE6, 0xE6, 0xE6);
  _darkThemeColorList[NXThemeType::BasicBaseDeep]       = QColor(0x61, 0x61, 0x61);
  _lightThemeColorList[NXThemeType::BasicDisable]       = QColor(0xF5, 0xF5, 0xF5);
  _darkThemeColorList[NXThemeType::BasicDisable]        = QColor(0x2A, 0x2A, 0x2A);
  _lightThemeColorList[NXThemeType::BasicHover]         = QColor(0xF3, 0xF3, 0xF3);
  _darkThemeColorList[NXThemeType::BasicHover]          = QColor(0x40, 0x40, 0x40);
  _lightThemeColorList[NXThemeType::BasicPress]         = QColor(0xF7, 0xF7, 0xF7);
  _darkThemeColorList[NXThemeType::BasicPress]          = QColor(0x3A, 0x3A, 0x3A);
  _lightThemeColorList[NXThemeType::BasicSelectedHover] = QColor(0xEB, 0xEB, 0xEB);
  _darkThemeColorList[NXThemeType::BasicSelectedHover]  = QColor(0x38, 0x38, 0x38);
  _lightThemeColorList[NXThemeType::BasicBaseLine]      = QColor(0xD1, 0xD1, 0xD1);
  _darkThemeColorList[NXThemeType::BasicBaseLine]       = QColor(0x45, 0x45, 0x45);
  _lightThemeColorList[NXThemeType::BasicHemline]       = QColor(0x86, 0x86, 0x86);
  _darkThemeColorList[NXThemeType::BasicHemline]        = QColor(0x9A, 0x9A, 0x9A);
  _lightThemeColorList[NXThemeType::BasicIndicator]     = QColor(0x75, 0x7C, 0x87);
  _darkThemeColorList[NXThemeType::BasicIndicator]      = QColor(0x75, 0x7C, 0x87);
  _lightThemeColorList[NXThemeType::BasicChute]         = QColor(0xB3, 0xB3, 0xB3);
  _darkThemeColorList[NXThemeType::BasicChute]          = QColor(0x63, 0x63, 0x63);

  // 基础透明
  _lightThemeColorList[NXThemeType::BasicAlternating]        = QColor(0xEF, 0xEF, 0xEF, 160);
  _darkThemeColorList[NXThemeType::BasicAlternating]         = QColor(0x45, 0x45, 0x45, 125);
  _lightThemeColorList[NXThemeType::BasicBaseAlpha]          = QColor(0xFA, 0xFA, 0xFA, 160);
  _darkThemeColorList[NXThemeType::BasicBaseAlpha]           = QColor(0x2D, 0x2D, 0x2D, 95);
  _lightThemeColorList[NXThemeType::BasicBaseDeepAlpha]      = QColor(0xC8, 0xC8, 0xC8, 160);
  _darkThemeColorList[NXThemeType::BasicBaseDeepAlpha]       = QColor(0x72, 0x72, 0x72, 95);
  _lightThemeColorList[NXThemeType::BasicHoverAlpha]         = QColor(0xCE, 0xCE, 0xCE, 110);
  _darkThemeColorList[NXThemeType::BasicHoverAlpha]          = QColor(0x4B, 0x4B, 0x4B, 75);
  _lightThemeColorList[NXThemeType::BasicPressAlpha]         = QColor(0xD0, 0xD0, 0xD0, 80);
  _darkThemeColorList[NXThemeType::BasicPressAlpha]          = QColor(0x4B, 0x4B, 0x4B, 55);
  _lightThemeColorList[NXThemeType::BasicSelectedAlpha]      = QColor(0xCE, 0xCE, 0xCE, 110);
  _darkThemeColorList[NXThemeType::BasicSelectedAlpha]       = QColor(0x4B, 0x4B, 0x4B, 75);
  _lightThemeColorList[NXThemeType::BasicSelectedHoverAlpha] = QColor(0xD0, 0xD0, 0xD0, 80);
  _darkThemeColorList[NXThemeType::BasicSelectedHoverAlpha]  = QColor(0x4B, 0x4B, 0x4B, 55);

  // 状态颜色
  _lightThemeColorList[NXThemeType::StatusDanger] = QColor(0xE8, 0x11, 0x23);
  _darkThemeColorList[NXThemeType::StatusDanger]  = QColor(0xE8, 0x11, 0x23);

  // Win10顶部边框颜色
  _lightThemeColorList[NXThemeType::Win10BorderActive]   = QColor(0x6E, 0x6E, 0x6E);
  _darkThemeColorList[NXThemeType::Win10BorderActive]    = QColor(0x33, 0x33, 0x33);
  _lightThemeColorList[NXThemeType::Win10BorderInactive] = QColor(0xA7, 0xA7, 0xA7);
  _darkThemeColorList[NXThemeType::Win10BorderInactive]  = QColor(0x3D, 0x3D, 0x3D);
}
