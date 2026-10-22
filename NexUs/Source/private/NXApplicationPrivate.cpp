#include "NXApplicationPrivate.h"

#include <QApplication>
#include <QEvent>
#include <QImage>
#include <QPalette>
#include <QScreen>
#include <QThread>
#include <QWidget>
#include <QtMath>

#include "DeveloperComponents/NXMicaBaseInitObject.h"
#include "DeveloperComponents/NXWinShadowHelper.h"
#include "NXApplication.h"
#include "NXTheme.h"

NXApplicationPrivate::NXApplicationPrivate(QObject *parent)
    : QObject { parent }
{
}

NXApplicationPrivate::~NXApplicationPrivate()
{
}

void NXApplicationPrivate::onThemeModeChanged(NXThemeType::ThemeMode themeMode)
{
  _themeMode = themeMode;
  switch (_pWindowDisplayMode) {
  case NXApplicationType::Normal : {
    break;
  }
  case NXApplicationType::NXMica : {
    _updateAllMicaWidget();
    break;
  }
  default : {
#ifdef Q_OS_WIN
    for (auto widget : _micaWidgetList) {
      NXWinShadowHelper::getInstance()->setWindowThemeMode(widget->winId(), _themeMode == NXThemeType::Light);
    }
#endif
    break;
  }
  }
}

bool NXApplicationPrivate::eventFilter(QObject *watched, QEvent *event)
{
  switch (event->type()) {
  case QEvent::Show : {
    if (_pWindowDisplayMode == NXApplicationType::WindowDisplayMode::NXMica) {
      QWidget *widget = qobject_cast<QWidget *>(watched);
      if (widget) {
        _updateMica(widget);
      }
    } else if (_pWindowDisplayMode != NXApplicationType::WindowDisplayMode::Normal) {
#ifdef Q_OS_WIN
      QWidget *widget = qobject_cast<QWidget *>(watched);
      if (widget) {
        NXWinShadowHelper::getInstance()->setWindowDisplayMode(widget, _pWindowDisplayMode, _pWindowDisplayMode);
      }
#endif
    }
    break;
  }
  case QEvent::Move :
  case QEvent::Resize : {
    if (_pWindowDisplayMode == NXApplicationType::WindowDisplayMode::NXMica) {
      QWidget *widget = qobject_cast<QWidget *>(watched);
      if (widget) {
        _updateMica(widget);
      }
    }
    break;
  }
  case QEvent::Destroy : {
    QWidget *widget = qobject_cast<QWidget *>(watched);
    if (widget) {
      _micaWidgetList.removeOne(widget);
    }
    break;
  }
  case QEvent::ApplicationPaletteChange : {
    onSystemPaletteChanged();
    break;
  }
  default : {
    break;
  }
  }
  return QObject::eventFilter(watched, event);
}

void NXApplicationPrivate::_initMicaBaseImage(const QImage &img)
{
  Q_Q(NXApplication);
  if (img.isNull()) {
    return;
  }
  QThread *initThread              = new QThread();
  NXMicaBaseInitObject *initObject = new NXMicaBaseInitObject(this);
  connect(initThread, &QThread::finished, initObject, &NXMicaBaseInitObject::deleteLater);
  connect(initObject, &NXMicaBaseInitObject::initFinished, initThread, [=]() {
    Q_EMIT q->pWindowDisplayModeChanged();
    _updateAllMicaWidget();
    initThread->quit();
    initThread->wait();
    initThread->deleteLater();
  });
  initObject->moveToThread(initThread);
  initThread->start();
  connect(this, &NXApplicationPrivate::initMicaBase, initObject, &NXMicaBaseInitObject::onInitMicaBase);
  Q_EMIT initMicaBase(img);
}

QRect NXApplicationPrivate::_calculateWindowVirtualGeometry(QWidget *widget)
{
  QRect geometry    = widget->geometry();
  qreal xImageRatio = 1, yImageRatio = 1;
  QRect relativeGeometry;
  if (qApp->screens().count() > 1) {
    QScreen *currentScreen = qApp->screenAt(geometry.topLeft());
    if (currentScreen) {
      QRect screenGeometry = currentScreen->availableGeometry();
      xImageRatio          = (qreal) _lightBaseImage.width() / screenGeometry.width();
      yImageRatio          = (qreal) _lightBaseImage.height() / screenGeometry.height();
      relativeGeometry =
          QRect((geometry.x() - screenGeometry.x()) * xImageRatio, (geometry.y() - screenGeometry.y()) * yImageRatio, geometry.width() * xImageRatio, geometry.height() * yImageRatio);
      return relativeGeometry;
    }
  }
  QRect primaryScreenGeometry = qApp->primaryScreen()->availableGeometry();
  xImageRatio                 = (qreal) _lightBaseImage.width() / primaryScreenGeometry.width();
  yImageRatio                 = (qreal) _lightBaseImage.height() / primaryScreenGeometry.height();
  relativeGeometry            = QRect((geometry.x() - primaryScreenGeometry.x()) * xImageRatio, (geometry.y() - primaryScreenGeometry.y()) * yImageRatio, geometry.width() * xImageRatio, geometry.height() * yImageRatio);
  return relativeGeometry;
}

void NXApplicationPrivate::_updateMica(QWidget *widget, bool isProcessEvent)
{
  if (widget->isVisible()) {
    QPalette palette = widget->palette();
    if (_themeMode == NXThemeType::Light) {
      palette.setBrush(QPalette::Window, _lightBaseImage.copy(_calculateWindowVirtualGeometry(widget)).scaled(widget->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
      palette.setBrush(QPalette::Window, _darkBaseImage.copy(_calculateWindowVirtualGeometry(widget)).scaled(widget->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    widget->setPalette(palette);
    if (isProcessEvent) {
      QApplication::processEvents();
    }
  }
}

void NXApplicationPrivate::_updateAllMicaWidget()
{
  if (_pWindowDisplayMode == NXApplicationType::WindowDisplayMode::NXMica) {
    for (auto widget : _micaWidgetList) {
      _updateMica(widget, false);
    }
  }
}

void NXApplicationPrivate::_resetAllMicaWidget()
{
  for (auto widget : _micaWidgetList) {
    QPalette palette = widget->palette();
    palette.setBrush(QPalette::Window, Qt::transparent);
    widget->setPalette(palette);
  }
}

void NXApplicationPrivate::onSystemPaletteChanged()
{
  syncSystemTheme();
}

void NXApplicationPrivate::syncSystemTheme()
{
  bool systemIsDark                  = _isSystemDarkMode();
  NXThemeType::ThemeMode currentMode = nxTheme->getThemeMode();
  NXThemeType::ThemeMode targetMode  = systemIsDark ? NXThemeType::Dark : NXThemeType::Light;

  if (currentMode != targetMode) {
    nxTheme->setThemeMode(targetMode);
  }
}

bool NXApplicationPrivate::_isSystemDarkMode() const
{
  QPalette palette   = qApp->palette();
  QColor windowColor = palette.color(QPalette::Window);
  QColor textColor   = palette.color(QPalette::WindowText);

  qreal windowLuminance = 0.299 * windowColor.red() + 0.587 * windowColor.green() + 0.114 * windowColor.blue();

  qreal textLuminance = 0.299 * textColor.red() + 0.587 * textColor.green() + 0.114 * textColor.blue();
  return windowLuminance < textLuminance;
}
