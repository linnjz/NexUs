#include "NXWinShadowHelper.h"

#ifdef Q_OS_WIN
#  include <QDebug>
#  include <QWidget>

NXWinShadowHelper::NXWinShadowHelper(QObject *parent)
    : QObject { parent }
    , m_IsWinVersionGreater10 { true }
    , m_IsWinVersionGreater11 { true }
{
  HMODULE module = LoadLibraryW(L"ntdll.dll");
  if (module) {
    auto pRtlGetVersion = reinterpret_cast<RtlGetVersionFunc>(GetProcAddress(module, "RtlGetVersion"));
    Q_ASSERT(pRtlGetVersion);
    m_WindowsVersion.dwOSVersionInfoSize = sizeof(m_WindowsVersion);
    pRtlGetVersion(&m_WindowsVersion);
    m_IsWinVersionGreater10 = compareWindowsVersion(Win10_Origin);
    m_IsWinVersionGreater11 = compareWindowsVersion(Win11_Origin);
  }
}

NXWinShadowHelper::~NXWinShadowHelper()
{
}

bool NXWinShadowHelper::initWinAPI()
{
  HMODULE dwmModule = LoadLibraryW(L"dwmapi.dll");
  if (dwmModule) {
    if (!m_DwmExtendFrameIntoClientArea) {
      m_DwmExtendFrameIntoClientArea =
          reinterpret_cast<DwmExtendFrameIntoClientAreaFunc>(GetProcAddress(dwmModule, "DwmExtendFrameIntoClientArea"));
    }
    if (!m_DwmSetWindowAttribute) {
      m_DwmSetWindowAttribute =
          reinterpret_cast<DwmSetWindowAttributeFunc>(GetProcAddress(dwmModule, "DwmSetWindowAttribute"));
    }
    if (!m_DwmIsCompositionEnabled) {
      m_DwmIsCompositionEnabled =
          reinterpret_cast<DwmIsCompositionEnabledFunc>(GetProcAddress(dwmModule, "DwmIsCompositionEnabled"));
    }
    if (!m_DwmEnableBlurBehindWindow) {
      m_DwmEnableBlurBehindWindow =
          reinterpret_cast<DwmEnableBlurBehindWindowFunc>(GetProcAddress(dwmModule, "DwmEnableBlurBehindWindow"));
    }
    if (!(m_DwmExtendFrameIntoClientArea && m_DwmSetWindowAttribute && m_DwmIsCompositionEnabled &&
          m_DwmEnableBlurBehindWindow)) {
      qCritical() << "Dwm Func Init Incomplete!";
      return false;
    }
  } else {
    qCritical() << "dwmapi.dll Load Fail!";
    return false;
  }
  HMODULE user32Module = LoadLibraryW(L"user32.dll");
  if (user32Module) {
    if (!m_SetWindowCompositionAttribute) {
      m_SetWindowCompositionAttribute = reinterpret_cast<SetWindowCompositionAttributeFunc>(
          GetProcAddress(user32Module, "SetWindowCompositionAttribute"));
    }
    if (!m_GetDpiForWindow) {
      m_GetDpiForWindow = reinterpret_cast<GetDpiForWindowFunc>(GetProcAddress(user32Module, "GetDpiForWindow"));
    }
    if (!m_GetSystemMetricsForDpi) {
      m_GetSystemMetricsForDpi =
          reinterpret_cast<GetSystemMetricsForDpiFunc>(GetProcAddress(user32Module, "GetSystemMetricsForDpi"));
    }
    if (!(m_SetWindowCompositionAttribute && m_GetDpiForWindow && m_GetSystemMetricsForDpi)) {
      qCritical() << "User32 Func Init Incomplete!";
      return false;
    }
  } else {
    qCritical() << "user32.dll Load Fail!";
    return false;
  }

  HMODULE shCoreModule = LoadLibraryW(L"SHCore.dll");
  if (shCoreModule) {
    if (!m_GetDpiForMonitor) {
      m_GetDpiForMonitor = reinterpret_cast<GetDpiForMonitorFunc>(GetProcAddress(shCoreModule, "GetDpiForMonitor"));
    }
    if (!(m_GetDpiForMonitor)) {
      qCritical() << "SHCore Func Init Incomplete!";
      return false;
    }
  } else {
    qCritical() << "SHCore.dll Load Fail!";
    return false;
  }
  return true;
}

void NXWinShadowHelper::setWindowShadow(quint64 hwnd)
{
  static const MARGINS shadow = { 1, 0, 0, 0 };
  m_DwmExtendFrameIntoClientArea((HWND) hwnd, &shadow);
}

void NXWinShadowHelper::setWindowThemeMode(quint64 hwnd, bool isLightMode)
{
  if (!compareWindowsVersion(Win10_1809) || !m_DwmSetWindowAttribute) {
    return;
  }
  BOOL bIsLightMode = !isLightMode;
  _DWMWINDOWATTRIBUTE dwAttritube =
      compareWindowsVersion(Win10_20H1) ? _DWMWA_USE_IMMERSIVE_DARK_MODE : _DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1;
  m_DwmSetWindowAttribute((HWND) hwnd, dwAttritube, &bIsLightMode, sizeof(bIsLightMode));
}

void NXWinShadowHelper::setWindowDisplayMode(QWidget *widget, NXApplicationType::WindowDisplayMode displayMode, NXApplicationType::WindowDisplayMode lastDisplayMode)
{
  HWND winHwnd = (HWND) widget->winId();
  switch (lastDisplayMode) {
  case NXApplicationType::Mica : {
    if (!compareWindowsVersion(Win11_Origin)) {
      break;
    }
    if (compareWindowsVersion(Win11_22H2)) {
      const _DWM_SYSTEMBACKDROP_TYPE backdropType = _DWMSBT_AUTO;
      m_DwmSetWindowAttribute(winHwnd, _DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
    } else {
      const BOOL isEnable = FALSE;
      m_DwmSetWindowAttribute(winHwnd, _DWMWA_MICA_EFFECT, &isEnable, sizeof(isEnable));
    }
    break;
  }
  case NXApplicationType::MicaAlt : {
    if (!compareWindowsVersion(Win11_22H2)) {
      break;
    }
    const _DWM_SYSTEMBACKDROP_TYPE backdropType = _DWMSBT_AUTO;
    m_DwmSetWindowAttribute(winHwnd, _DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
    break;
  }
  case NXApplicationType::Acrylic : {
    if (!compareWindowsVersion(Win11_Origin)) {
      break;
    }
    const _DWM_SYSTEMBACKDROP_TYPE backdropType = _DWMSBT_AUTO;
    m_DwmSetWindowAttribute(winHwnd, _DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
    break;
  }
  case NXApplicationType::DWMBlur : {
    if (compareWindowsVersion(Win7_Origin)) {
      _ACCENT_POLICY policy {};
      policy.dwAccentState = _ACCENT_DISABLED;
      policy.dwAccentFlags = _ACCENT_NONE;
      _WINDOWCOMPOSITIONATTRIBDATA wcad {};
      wcad.Attrib = _WCA_ACCENT_POLICY;
      wcad.pvData = &policy;
      wcad.cbData = sizeof(policy);
      m_SetWindowCompositionAttribute(winHwnd, &wcad);
    } else {
      DWM_BLURBEHIND bb {};
      bb.fEnable = FALSE;
      bb.dwFlags = DWM_BB_ENABLE;
      m_DwmEnableBlurBehindWindow(winHwnd, &bb);
    }
    break;
  }
  default : {
    break;
  }
  }

  switch (displayMode) {
  case NXApplicationType::Mica : {
    if (!compareWindowsVersion(Win11_Origin)) {
      break;
    }
    _externWindowMargins(winHwnd);
    if (compareWindowsVersion(Win11_22H2)) {
      const _DWM_SYSTEMBACKDROP_TYPE backdropType = _DWMSBT_MAINWINDOW;
      m_DwmSetWindowAttribute(winHwnd, _DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
    } else {
      const BOOL enable = TRUE;
      m_DwmSetWindowAttribute(winHwnd, _DWMWA_MICA_EFFECT, &enable, sizeof(enable));
    }
    break;
  }
  case NXApplicationType::MicaAlt : {
    if (!compareWindowsVersion(Win11_22H2)) {
      break;
    }
    _externWindowMargins(winHwnd);
    const _DWM_SYSTEMBACKDROP_TYPE backdropType = _DWMSBT_TABBEDWINDOW;
    m_DwmSetWindowAttribute(winHwnd, _DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
    break;
  }
  case NXApplicationType::Acrylic : {
    if (!compareWindowsVersion(Win11_Origin)) {
      break;
    }
    _externWindowMargins(winHwnd);
    const _DWM_SYSTEMBACKDROP_TYPE backdropType = _DWMSBT_TRANSIENTWINDOW;
    m_DwmSetWindowAttribute(winHwnd, _DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
    break;
  }
  case NXApplicationType::DWMBlur : {
    MARGINS windowMargins = { 0, 1, 0, 0 };
    m_DwmExtendFrameIntoClientArea(winHwnd, &windowMargins);
    if (compareWindowsVersion(Win7_Origin)) {
      _ACCENT_POLICY policy {};
      policy.dwAccentState = _ACCENT_ENABLE_BLURBEHIND;
      policy.dwAccentFlags = _ACCENT_NONE;
      _WINDOWCOMPOSITIONATTRIBDATA wcad {};
      wcad.Attrib = _WCA_ACCENT_POLICY;
      wcad.pvData = &policy;
      wcad.cbData = sizeof(policy);
      m_SetWindowCompositionAttribute(winHwnd, &wcad);
    } else {
      DWM_BLURBEHIND bb {};
      bb.fEnable = TRUE;
      bb.dwFlags = DWM_BB_ENABLE;
      m_DwmEnableBlurBehindWindow(winHwnd, &bb);
    }
    break;
  }
  default : {
    break;
  }
  }
}

bool NXWinShadowHelper::getIsCompositionEnabled() const
{
  BOOL isCompositionEnabled = false;
  if (m_DwmIsCompositionEnabled) {
    m_DwmIsCompositionEnabled(&isCompositionEnabled);
  }
  return isCompositionEnabled;
}

bool NXWinShadowHelper::getIsFullScreen(const HWND &hwnd)
{
  RECT windowRect {};
  ::GetWindowRect(hwnd, &windowRect);
  RECT rcMonitor = getMonitorForWindow(hwnd).rcMonitor;
  return windowRect.top == rcMonitor.top && windowRect.left == rcMonitor.left && windowRect.right == rcMonitor.right &&
         windowRect.bottom == rcMonitor.bottom;
}

MONITORINFOEXW NXWinShadowHelper::getMonitorForWindow(const HWND &hwnd)
{
  HMONITOR monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFOEXW monitorInfo {};
  monitorInfo.cbSize = sizeof(monitorInfo);
  ::GetMonitorInfoW(monitor, &monitorInfo);
  return monitorInfo;
}

quint32 NXWinShadowHelper::getResizeBorderThickness(const HWND &hwnd)
{
  return getSystemMetricsForDpi(hwnd, SM_CXSIZEFRAME) + getSystemMetricsForDpi(hwnd, SM_CXPADDEDBORDER);
}

quint32 NXWinShadowHelper::getDpiForWindow(const HWND &hwnd)
{
  if (m_GetDpiForWindow) {
    return m_GetDpiForWindow(hwnd);
  } else if (m_GetDpiForMonitor) {
    HMONITOR monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    UINT dpiX { 0 };
    UINT dpiY { 0 };
    m_GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    return dpiX;
  } else {
    HDC hdc        = ::GetDC(nullptr);
    int const dpiX = ::GetDeviceCaps(hdc, LOGPIXELSX);
    ::ReleaseDC(nullptr, hdc);
    return quint32(dpiX);
  }
}

int NXWinShadowHelper::getSystemMetricsForDpi(const HWND &hwnd, int const index)
{
  quint32 const dpi = getDpiForWindow(hwnd);
  if (m_GetSystemMetricsForDpi) {
    return m_GetSystemMetricsForDpi(index, dpi);
  }
  int const result = ::GetSystemMetrics(index);
  if (dpi != USER_DEFAULT_SCREEN_DPI) {
    return result;
  }
  qreal const dpr = qreal(dpi) / qreal(USER_DEFAULT_SCREEN_DPI);
  return qRound(qreal(result) / dpr);
}

bool NXWinShadowHelper::compareWindowsVersion(QString const &windowsVersion) const
{
  QStringList versionList = windowsVersion.split(QStringLiteral("."));
  if (versionList.count() != 3) {
    return false;
  }
  return (m_WindowsVersion.dwMajorVersion > versionList[0].toUInt()) ||
         (m_WindowsVersion.dwMajorVersion == versionList[0].toUInt() &&
          (m_WindowsVersion.dwMinorVersion > versionList[1].toUInt() ||
           m_WindowsVersion.dwBuildNumber >= versionList[2].toUInt()));
}

void NXWinShadowHelper::_externWindowMargins(HWND hwnd)
{
  static const MARGINS margins = { 65536, 0, 0, 0 };
  if (m_DwmExtendFrameIntoClientArea) {
    m_DwmExtendFrameIntoClientArea(hwnd, &margins);
  }
}
#endif
