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
  d->_pFontPixelSize     = 13;
  d->_pNXMicaImagePath   = QStringLiteral(":/Resource/Image/MicaBase.png");
  d->_pWindowDisplayMode = NXApplicationType::Normal;
  d->_themeMode          = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXApplicationPrivate::onThemeModeChanged);
}

NXApplication::~NXApplication()
{
}

void NXApplication::setWindowDisplayMode(NXApplicationType::WindowDisplayMode windowDisplayType)
{
  Q_D(NXApplication);
  auto lastDisplayMode = d->_pWindowDisplayMode;
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
    d->_pWindowDisplayMode = windowDisplayType;
    d->_initMicaBaseImage(QImage(d->_pNXMicaImagePath));
    break;
  }
  default : {
    break;
  }
  }
#ifdef Q_OS_WIN
  for (auto widget : d->_micaWidgetList) {
    NXWinShadowHelper::getInstance()->setWindowDisplayMode(widget, windowDisplayType, lastDisplayMode);
    NXWinShadowHelper::getInstance()->setWindowThemeMode(widget->winId(), d->_themeMode == NXThemeType::Light);
  }
#endif
  if (windowDisplayType != NXApplicationType::NXMica) {
    d->_pWindowDisplayMode = windowDisplayType;
    Q_EMIT pWindowDisplayModeChanged();
  }
}

NXApplicationType::WindowDisplayMode NXApplication::getWindowDisplayMode() const
{
  Q_D(const NXApplication);
  return d->_pWindowDisplayMode;
}

void NXApplication::setNXMicaImagePath(const QString &micaImagePath)
{
  Q_D(NXApplication);
  d->_pNXMicaImagePath = std::move(micaImagePath);
  d->_initMicaBaseImage(QImage(d->_pNXMicaImagePath));
  Q_EMIT pNXMicaImagePathChanged();
}

QString NXApplication::getNXMicaImagePath() const
{
  Q_D(const NXApplication);
  return d->_pNXMicaImagePath;
}

void NXApplication::setFontPixelSize(int fontPixelSize)
{
  Q_D(NXApplication);
  d->_pFontPixelSize = fontPixelSize;
  QFont font         = qApp->font();
  font.setPixelSize(fontPixelSize);
  qApp->setFont(font);
  Q_EMIT pFontPixelSizeChanged();
}

int NXApplication::getFontPixelSize() const
{
  Q_D(const NXApplication);
  return d->_pFontPixelSize;
}

void NXApplication::init()
{
  Q_D(NXApplication);
  Q_INIT_RESOURCE(NexUs);
  QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
  QFontDatabase::addApplicationFont(QStringLiteral(":/Resource/Font/NXAwesome.ttf"));
  //默认字体 - 根据平台设置
  QFont font = qApp->font();
  font.setPixelSize(d->_pFontPixelSize);

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
  for (const QString &family : fontFamilies) {
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
    d->_micaWidgetList.append(widget);
    widget->installEventFilter(d);
  } else {
    d->_micaWidgetList.removeOne(widget);
    widget->removeEventFilter(d);
  }
  switch (d->_pWindowDisplayMode) {
  case NXApplicationType::Normal :
  case NXApplicationType::NXMica : {
    if (isSync) {
      if (d->_pWindowDisplayMode == NXApplicationType::WindowDisplayMode::NXMica) {
        d->_updateMica(widget, false);
      }
    }
    break;
  }
  default : {
#ifdef Q_OS_WIN
    if (isSync) {
      NXWinShadowHelper::getInstance()->setWindowDisplayMode(widget, d->_pWindowDisplayMode, NXApplicationType::Normal);
      NXWinShadowHelper::getInstance()->setWindowThemeMode(widget->winId(), d->_themeMode == NXThemeType::Light);
    } else {
      NXWinShadowHelper::getInstance()->setWindowDisplayMode(widget, NXApplicationType::Normal, d->_pWindowDisplayMode);
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
