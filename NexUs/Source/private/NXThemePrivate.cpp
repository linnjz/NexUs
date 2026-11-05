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
  if (m_IsFollowSystemTheme && event->type() == QEvent::ApplicationPaletteChange) {
    _applySystemTheme();
  }
  return QObject::eventFilter(watched, event);
}

NXThemeType::ThemeMode NXThemePrivate::_detectSystemThemeMode() const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
  Qt::ColorScheme const scheme = QGuiApplication::styleHints()->colorScheme();
  if (scheme == Qt::ColorScheme::Dark) {
    return NXThemeType::Dark;
  }
  if (scheme == Qt::ColorScheme::Light) {
    return NXThemeType::Light;
  }
#endif
  QColor const windowColor = QApplication::palette().color(QPalette::Window);
  return windowColor.lightness() < 128 ? NXThemeType::Dark : NXThemeType::Light;
}

void NXThemePrivate::_applySystemTheme()
{
  Q_Q(NXTheme);
  NXThemeType::ThemeMode const mode = _detectSystemThemeMode();
  if (m_ThemeMode != mode) {
    q->setThemeMode(mode);
  }
}

void NXThemePrivate::_initThemeColor()
{
  // NXTabBar
  m_LightThemeColorList[NXThemeType::TabBarBase] = QColor(0xEA, 0xEA, 0xED);
  m_DarkThemeColorList[NXThemeType::TabBarBase]  = QColor(0x1C, 0x20, 0x27);

  m_LightThemeColorList[NXThemeType::TabBarSelected] = QColor(0xFF, 0xFF, 0xFF);
  m_DarkThemeColorList[NXThemeType::TabBarSelected]  = QColor(0x38, 0x3B, 0x43);

  m_LightThemeColorList[NXThemeType::TabBarHover] = QColor(0xD2, 0xD2, 0xD6);
  m_DarkThemeColorList[NXThemeType::TabBarHover]  = QColor(0x3B, 0x47, 0x5E);

  m_LightThemeColorList[NXThemeType::TabBarCloseButtonHover] = QColor(0xBF, 0xBF, 0xC3);
  m_DarkThemeColorList[NXThemeType::TabBarCloseButtonHover]  = QColor(0x5A, 0x64, 0x77);

  m_LightThemeColorList[NXThemeType::TabBarSelectedCloseButtonHover] = QColor(0xE7, 0xE7, 0xE8);
  m_DarkThemeColorList[NXThemeType::TabBarSelectedCloseButtonHover]  = QColor(0x57, 0x5A, 0x60);

  // NXScrollBar
  m_LightThemeColorList[NXThemeType::ScrollBarHandle] = QColor(0x8A, 0x8A, 0x8A);
  m_DarkThemeColorList[NXThemeType::ScrollBarHandle]  = QColor(0x9F, 0x9F, 0x9F);

  // NXToggleSwitch
  m_LightThemeColorList[NXThemeType::ToggleSwitchNoToggledCenter] = QColor(0x6A, 0x6A, 0x6A);
  m_DarkThemeColorList[NXThemeType::ToggleSwitchNoToggledCenter]  = QColor(0xD0, 0xD0, 0xD0);

  // 主题颜色
  m_LightThemeColorList[NXThemeType::PrimaryNormal] = QColor(0x00, 0x67, 0xC0);
  m_DarkThemeColorList[NXThemeType::PrimaryNormal]  = QColor(0x4A, 0xBC, 0xF7);
  m_LightThemeColorList[NXThemeType::PrimaryHover]  = QColor(0x19, 0x75, 0xC5);
  m_DarkThemeColorList[NXThemeType::PrimaryHover]   = QColor(0x47, 0xB1, 0xE8);
  m_LightThemeColorList[NXThemeType::PrimaryPress]  = QColor(0x31, 0x83, 0xCA);
  m_DarkThemeColorList[NXThemeType::PrimaryPress]   = QColor(0x42, 0xA1, 0xD2);

  // 通用颜色
  // 普通窗体
  m_LightThemeColorList[NXThemeType::WindowBase]             = QColor(0xEC, 0xEC, 0xEC);
  m_DarkThemeColorList[NXThemeType::WindowBase]              = QColor(0x20, 0x20, 0x20);
  m_LightThemeColorList[NXThemeType::WindowCentralStackBase] = QColor(0xF8, 0xF8, 0xF8, 90);
  m_DarkThemeColorList[NXThemeType::WindowCentralStackBase]  = QColor(0x3E, 0x3E, 0x3E, 60);

  // 浮动窗体
  m_LightThemeColorList[NXThemeType::PopupBorder]      = QColor(0xC8, 0xC8, 0xC8);
  m_DarkThemeColorList[NXThemeType::PopupBorder]       = QColor(0x47, 0x47, 0x47);
  m_LightThemeColorList[NXThemeType::PopupBorderHover] = QColor(0xBC, 0xBC, 0xBC);
  m_DarkThemeColorList[NXThemeType::PopupBorderHover]  = QColor(0x54, 0x54, 0x54);
  m_LightThemeColorList[NXThemeType::PopupBase]        = QColor(0xF6, 0xF6, 0xF6);
  m_DarkThemeColorList[NXThemeType::PopupBase]         = QColor(0x2C, 0x2C, 0x2C);
  m_LightThemeColorList[NXThemeType::PopupHover]       = QColor(0xEE, 0xEE, 0xEE);
  m_DarkThemeColorList[NXThemeType::PopupHover]        = QColor(0x38, 0x38, 0x38);

  // Dialog窗体
  m_LightThemeColorList[NXThemeType::DialogBase]       = QColor(0xF7, 0xF7, 0xF7);
  m_DarkThemeColorList[NXThemeType::DialogBase]        = QColor(0x1F, 0x1F, 0x1F);
  m_LightThemeColorList[NXThemeType::DialogLayoutArea] = QColor(0xED, 0xED, 0xED);
  m_DarkThemeColorList[NXThemeType::DialogLayoutArea]  = QColor(0x20, 0x20, 0x20);

  // 基础颜色
  m_LightThemeColorList[NXThemeType::BasicText]          = Qt::black;
  m_DarkThemeColorList[NXThemeType::BasicText]           = Qt::white;
  m_LightThemeColorList[NXThemeType::BasicTextInvert]    = Qt::white;
  m_DarkThemeColorList[NXThemeType::BasicTextInvert]     = Qt::black;
  m_LightThemeColorList[NXThemeType::BasicDetailsText]   = QColor(0x87, 0x87, 0x87);
  m_DarkThemeColorList[NXThemeType::BasicDetailsText]    = QColor(0xAD, 0xAD, 0xB0);
  m_LightThemeColorList[NXThemeType::BasicTextNoFocus]   = QColor(0x86, 0x86, 0x8A);
  m_DarkThemeColorList[NXThemeType::BasicTextNoFocus]    = QColor(0x86, 0x86, 0x8A);
  m_LightThemeColorList[NXThemeType::BasicTextDisable]   = QColor(0xB6, 0xB6, 0xB6);
  m_DarkThemeColorList[NXThemeType::BasicTextDisable]    = QColor(0xA7, 0xA7, 0xA7);
  m_LightThemeColorList[NXThemeType::BasicTextPress]     = QColor(0x5A, 0x5A, 0x5D);
  m_DarkThemeColorList[NXThemeType::BasicTextPress]      = QColor(0xBB, 0xBB, 0xBF);
  m_LightThemeColorList[NXThemeType::BasicTextCategory]  = QColor(0x5C, 0x5C, 0x5F);
  m_DarkThemeColorList[NXThemeType::BasicTextCategory]   = QColor(0xCC, 0xCC, 0xCE);
  m_LightThemeColorList[NXThemeType::BasicBorder]        = QColor(0xD6, 0xD6, 0xD6);
  m_DarkThemeColorList[NXThemeType::BasicBorder]         = QColor(0x4B, 0x4B, 0x4B);
  m_LightThemeColorList[NXThemeType::BasicBorderDeep]    = QColor(0x9A, 0x9A, 0x9A);
  m_DarkThemeColorList[NXThemeType::BasicBorderDeep]     = QColor(0x5C, 0x5C, 0x5C);
  m_LightThemeColorList[NXThemeType::BasicBorderHover]   = QColor(0xC8, 0xC8, 0xC8);
  m_DarkThemeColorList[NXThemeType::BasicBorderHover]    = QColor(0x57, 0x57, 0x57);
  m_LightThemeColorList[NXThemeType::BasicBase]          = QColor(0xFD, 0xFD, 0xFD);
  m_DarkThemeColorList[NXThemeType::BasicBase]           = QColor(0x34, 0x34, 0x34);
  m_LightThemeColorList[NXThemeType::BasicBaseDeep]      = QColor(0xE6, 0xE6, 0xE6);
  m_DarkThemeColorList[NXThemeType::BasicBaseDeep]       = QColor(0x61, 0x61, 0x61);
  m_LightThemeColorList[NXThemeType::BasicDisable]       = QColor(0xF5, 0xF5, 0xF5);
  m_DarkThemeColorList[NXThemeType::BasicDisable]        = QColor(0x2A, 0x2A, 0x2A);
  m_LightThemeColorList[NXThemeType::BasicHover]         = QColor(0xF3, 0xF3, 0xF3);
  m_DarkThemeColorList[NXThemeType::BasicHover]          = QColor(0x40, 0x40, 0x40);
  m_LightThemeColorList[NXThemeType::BasicPress]         = QColor(0xF7, 0xF7, 0xF7);
  m_DarkThemeColorList[NXThemeType::BasicPress]          = QColor(0x3A, 0x3A, 0x3A);
  m_LightThemeColorList[NXThemeType::BasicSelectedHover] = QColor(0xEB, 0xEB, 0xEB);
  m_DarkThemeColorList[NXThemeType::BasicSelectedHover]  = QColor(0x38, 0x38, 0x38);
  m_LightThemeColorList[NXThemeType::BasicBaseLine]      = QColor(0xD1, 0xD1, 0xD1);
  m_DarkThemeColorList[NXThemeType::BasicBaseLine]       = QColor(0x45, 0x45, 0x45);
  m_LightThemeColorList[NXThemeType::BasicHemline]       = QColor(0x86, 0x86, 0x86);
  m_DarkThemeColorList[NXThemeType::BasicHemline]        = QColor(0x9A, 0x9A, 0x9A);
  m_LightThemeColorList[NXThemeType::BasicIndicator]     = QColor(0x75, 0x7C, 0x87);
  m_DarkThemeColorList[NXThemeType::BasicIndicator]      = QColor(0x75, 0x7C, 0x87);
  m_LightThemeColorList[NXThemeType::BasicChute]         = QColor(0xB3, 0xB3, 0xB3);
  m_DarkThemeColorList[NXThemeType::BasicChute]          = QColor(0x63, 0x63, 0x63);

  // 基础透明
  m_LightThemeColorList[NXThemeType::BasicAlternating]        = QColor(0xEF, 0xEF, 0xEF, 160);
  m_DarkThemeColorList[NXThemeType::BasicAlternating]         = QColor(0x45, 0x45, 0x45, 125);
  m_LightThemeColorList[NXThemeType::BasicBaseAlpha]          = QColor(0xFA, 0xFA, 0xFA, 160);
  m_DarkThemeColorList[NXThemeType::BasicBaseAlpha]           = QColor(0x2D, 0x2D, 0x2D, 95);
  m_LightThemeColorList[NXThemeType::BasicBaseDeepAlpha]      = QColor(0xC8, 0xC8, 0xC8, 160);
  m_DarkThemeColorList[NXThemeType::BasicBaseDeepAlpha]       = QColor(0x72, 0x72, 0x72, 95);
  m_LightThemeColorList[NXThemeType::BasicHoverAlpha]         = QColor(0xCE, 0xCE, 0xCE, 110);
  m_DarkThemeColorList[NXThemeType::BasicHoverAlpha]          = QColor(0x4B, 0x4B, 0x4B, 75);
  m_LightThemeColorList[NXThemeType::BasicPressAlpha]         = QColor(0xD0, 0xD0, 0xD0, 80);
  m_DarkThemeColorList[NXThemeType::BasicPressAlpha]          = QColor(0x4B, 0x4B, 0x4B, 55);
  m_LightThemeColorList[NXThemeType::BasicSelectedAlpha]      = QColor(0xCE, 0xCE, 0xCE, 110);
  m_DarkThemeColorList[NXThemeType::BasicSelectedAlpha]       = QColor(0x4B, 0x4B, 0x4B, 75);
  m_LightThemeColorList[NXThemeType::BasicSelectedHoverAlpha] = QColor(0xD0, 0xD0, 0xD0, 80);
  m_DarkThemeColorList[NXThemeType::BasicSelectedHoverAlpha]  = QColor(0x4B, 0x4B, 0x4B, 55);

  // 状态颜色
  m_LightThemeColorList[NXThemeType::StatusDanger] = QColor(0xE8, 0x11, 0x23);
  m_DarkThemeColorList[NXThemeType::StatusDanger]  = QColor(0xE8, 0x11, 0x23);

  // Win10顶部边框颜色
  m_LightThemeColorList[NXThemeType::Win10BorderActive]   = QColor(0x6E, 0x6E, 0x6E);
  m_DarkThemeColorList[NXThemeType::Win10BorderActive]    = QColor(0x33, 0x33, 0x33);
  m_LightThemeColorList[NXThemeType::Win10BorderInactive] = QColor(0xA7, 0xA7, 0xA7);
  m_DarkThemeColorList[NXThemeType::Win10BorderInactive]  = QColor(0x3D, 0x3D, 0x3D);
}
