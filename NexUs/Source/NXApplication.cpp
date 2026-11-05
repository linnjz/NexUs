#include "NXApplication.h"

#include <utility>

#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QFontDatabase>
#include <QFontInfo>
#include <QWidget>

#include "DeveloperComponents/NXWinShadowHelper.h"
#include "NXTheme.h"
#include "private/NXApplicationPrivate.h"

NXApplication::NXApplication(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXApplicationPrivate())
{
  Q_D(NXApplication);
  d->q_ptr               = this;
  d->m_FontPixelSize     = 13;
  d->m_NXMicaImagePath   = QStringLiteral(":/Resource/Image/MicaBase.png");
  d->m_WindowDisplayMode = NXApplicationType::Normal;
  d->m_ThemeMode          = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXApplicationPrivate::onThemeModeChanged);
}

NXApplication::~NXApplication()
{
}

void NXApplication::setWindowDisplayMode(NXApplicationType::WindowDisplayMode windowDisplayType)
{
  Q_D(NXApplication);
  auto lastDisplayMode = d->m_WindowDisplayMode;
  if (lastDisplayMode == windowDisplayType) {
    return;
  }
  if (lastDisplayMode == NXApplicationType::NXMica) {
    d->_resetAllMicaWidget();
  }
  switch (windowDisplayType) {
  case NXApplicationType::Normal : {
    break;
  }
  case NXApplicationType::NXMica : {
    d->m_WindowDisplayMode = windowDisplayType;
    d->_initMicaBaseImage(QImage(d->m_NXMicaImagePath));
    break;
  }
  default : {
    break;
  }
  }
#ifdef Q_OS_WIN
  for (auto widget : d->m_MicaWidgetList) {
    NXWinShadowHelper::getInstance()->setWindowDisplayMode(widget, windowDisplayType, lastDisplayMode);
    NXWinShadowHelper::getInstance()->setWindowThemeMode(widget->winId(), d->m_ThemeMode == NXThemeType::Light);
  }
#endif
  if (windowDisplayType != NXApplicationType::NXMica) {
    d->m_WindowDisplayMode = windowDisplayType;
    Q_EMIT signalWindowDisplayModeChanged();
  }
}

NXApplicationType::WindowDisplayMode NXApplication::getWindowDisplayMode() const noexcept(std::is_nothrow_copy_constructible_v<NXApplicationType::WindowDisplayMode>)
{
  Q_CONST_D(NXApplication);
  return d->m_WindowDisplayMode;
}

void NXApplication::setNXMicaImagePath(QString const &micaImagePath)
{
  Q_D(NXApplication);
  d->m_NXMicaImagePath = std::move(micaImagePath);
  d->_initMicaBaseImage(QImage(d->m_NXMicaImagePath));
  Q_EMIT signalNXMicaImagePathChanged();
}

QString NXApplication::getNXMicaImagePath() const noexcept(std::is_nothrow_copy_constructible_v<QString>)
{
  Q_CONST_D(NXApplication);
  return d->m_NXMicaImagePath;
}

void NXApplication::setFontPixelSize(int fontPixelSize)
{
  Q_D(NXApplication);
  d->m_FontPixelSize = fontPixelSize;
  QFont font         = qApp->font();
  font.setPixelSize(fontPixelSize);
  qApp->setFont(font);
  Q_EMIT signalFontPixelSizeChanged();
}

int NXApplication::getFontPixelSize() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXApplication);
  return d->m_FontPixelSize;
}

void NXApplication::init()
{
  Q_D(NXApplication);
  Q_INIT_RESOURCE(NexUs);
  QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
  QFontDatabase::addApplicationFont(QStringLiteral(":/Resource/Font/NXAwesome.ttf"));
  //默认字体 - 根据平台设置
  QFont font = qApp->font();
  font.setPixelSize(d->m_FontPixelSize);

  QStringList fontFamilies;
#ifdef Q_OS_WIN
  fontFamilies << QStringLiteral("Microsoft YaHei UI") << QStringLiteral("SimSun") << QStringLiteral("Arial");
#elif defined(Q_OS_MACOS)
  fontFamilies << QStringLiteral("PingFang SC") << QStringLiteral("Heiti SC") << QStringLiteral("STHeiti")
               << QStringLiteral("Helvetica");
#else
  fontFamilies << QStringLiteral("Noto Sans CJK SC") << QStringLiteral("Source Han Sans SC")
               << QStringLiteral("WenQuanYi Micro Hei") << QStringLiteral("DejaVu Sans");
#endif

  bool fontFound = false;
  for (QString const &family : fontFamilies) {
    QFont testFont(family);
    if (QFontInfo(testFont).family() == family) {
      font.setFamily(family);
      fontFound = true;
      break;
    }
  }

  if (!fontFound) {
    qWarning() << "No preferred fonts found, using system default font";
  }
  font.setHintingPreference(QFont::PreferNoHinting);
  qApp->setFont(font);
#ifdef Q_OS_WIN
  nxWinHelper->initWinAPI();
#endif
  d->syncSystemTheme();
}

void NXApplication::syncWindowDisplayMode(QWidget *widget, bool isSync)
{
  Q_D(NXApplication);
  if (!widget) {
    return;
  }
  if (isSync) {
    d->m_MicaWidgetList.append(widget);
    widget->installEventFilter(d);
  } else {
    d->m_MicaWidgetList.removeOne(widget);
    widget->removeEventFilter(d);
  }
  switch (d->m_WindowDisplayMode) {
  case NXApplicationType::Normal :
  case NXApplicationType::NXMica : {
    if (isSync) {
      if (d->m_WindowDisplayMode == NXApplicationType::WindowDisplayMode::NXMica) {
        d->_updateMica(widget, false);
      }
    }
    break;
  }
  default : {
#ifdef Q_OS_WIN
    if (isSync) {
      NXWinShadowHelper::getInstance()->setWindowDisplayMode(widget, d->m_WindowDisplayMode, NXApplicationType::Normal);
      NXWinShadowHelper::getInstance()->setWindowThemeMode(widget->winId(), d->m_ThemeMode == NXThemeType::Light);
    } else {
      NXWinShadowHelper::getInstance()->setWindowDisplayMode(widget, NXApplicationType::Normal, d->m_WindowDisplayMode);
      NXWinShadowHelper::getInstance()->setWindowThemeMode(widget->winId(), true);
    }
#endif
    break;
  }
  }
}

bool NXApplication::containsCursorToItem(QWidget *item)
{
  if (!item || !item->isVisible()) {
    return false;
  }
  auto itemRect = QRect(item->mapToGlobal(QPoint(0, 0)), item->size());
  if (itemRect.contains(QCursor::pos())) {
    return true;
  }
  return false;
}
