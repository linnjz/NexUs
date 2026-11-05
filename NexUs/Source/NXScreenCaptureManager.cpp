#include "NXScreenCaptureManager.h"
#ifdef Q_OS_MAC
#  include <QApplication>
#  include <QDebug>
#  include <QPainter>
#  include <QPainterPath>
#  include <QScreen>
#  include <QThread>

#  include "NXScreenCapture.h"
#  include "NXScreenCaptureManagerPrivate.h"

NXScreenCaptureManager::NXScreenCaptureManager(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXScreenCaptureManagerPrivate())
{
  Q_D(NXScreenCaptureManager);
  d->q_ptr          = this;
  d->m_CaptureThread = new QThread(this);
  d->m_ScreenCapture = new NXScreenCapture();

  QScreen *screen  = QGuiApplication::primaryScreen();
  QSize screenSize = screen ? screen->size() : QSize(1920, 1080);
  setGrabArea(0, 0, screenSize.width(), screenSize.height());

  bool ret = d->m_ScreenCapture->initialize(0);
  if (!ret) {
    for (int i = 1; i < d->m_ScreenCapture->getDisplayList().count(); i++) {
      ret = d->m_ScreenCapture->initialize(i);
      if (ret) {
        break;
      }
    }
  }
  if (!ret) {
    d->m_ScreenCapture->initialize(0);
    qCritical() << "No available screenshot devices";
  }
  d->m_ScreenCapture->moveToThread(d->m_CaptureThread);
  d->m_CaptureThread->start();
  connect(d, &NXScreenCaptureManagerPrivate::grabScreen, d->m_ScreenCapture, &NXScreenCapture::onGrabScreen);
  connect(d->m_ScreenCapture, &NXScreenCapture::grabScreenOver, this, &NXScreenCaptureManager::grabImageUpdate);
}

NXScreenCaptureManager::~NXScreenCaptureManager()
{
  Q_D(NXScreenCaptureManager);
  if (d->m_ScreenCapture) {
    d->m_ScreenCapture->setIsGrabActive(false);
  }
  if (d->m_CaptureThread->isRunning()) {
    d->m_CaptureThread->quit();
    d->m_CaptureThread->wait();
  }
  delete d->m_ScreenCapture;
}

QStringList NXScreenCaptureManager::getDisplayList() const
{
  Q_CONST_D(NXScreenCaptureManager);
  return d->m_ScreenCapture->getDisplayList();
}

QImage NXScreenCaptureManager::grabScreenToImage() const
{
  Q_CONST_D(NXScreenCaptureManager);
  if (!d->m_ScreenCapture->getIsInitSuccess()) {
    return QImage();
  }
  return d->m_ScreenCapture->getGrabImage();
}

void NXScreenCaptureManager::startGrabScreen()
{
  Q_D(NXScreenCaptureManager);
  d->m_IsAllowedGrabScreen = true;
  if (!d->m_ScreenCapture->getIsGrabActive()) {
    d->m_ScreenCapture->setIsGrabActive(true);
    Q_EMIT d->grabScreen();
  }
}

void NXScreenCaptureManager::stopGrabScreen()
{
  Q_D(NXScreenCaptureManager);
  d->m_IsAllowedGrabScreen = false;
  d->m_ScreenCapture->setIsGrabActive(false);
}

bool NXScreenCaptureManager::getIsGrabScreen() const
{
  Q_CONST_D(NXScreenCaptureManager);
  return d->m_ScreenCapture->getIsGrabActive();
}

bool NXScreenCaptureManager::setDisplayID(int displayID)
{
  Q_D(NXScreenCaptureManager);
  if (displayID < 0 || d->m_ScreenCapture->getDisplayList().count() <= displayID) {
    return false;
  }
  d->m_ScreenCapture->setIsGrabActive(false);
  while (!d->m_ScreenCapture->getIsGrabStoped()) {
    QApplication::processEvents();
  }
  if (d->m_ScreenCapture->initialize(displayID)) {
    if (d->m_IsAllowedGrabScreen) {
      d->m_ScreenCapture->setIsGrabActive(true);
      Q_EMIT d->grabScreen();
    }
    return true;
  }
  return false;
}

int NXScreenCaptureManager::getDisplayID() const
{
  Q_CONST_D(NXScreenCaptureManager);
  return d->m_ScreenCapture->getDisplayID();
}

void NXScreenCaptureManager::setGrabArea(int width, int height)
{
  Q_D(NXScreenCaptureManager);
  QScreen *screen = QGuiApplication::primaryScreen();
  int maxWidth    = screen ? screen->size().width() : 1920;
  int maxHeight   = screen ? screen->size().height() : 1080;
  if (width <= 0 || width > maxWidth) {
    width = maxWidth;
  }
  if (height <= 0 || height > maxHeight) {
    height = maxHeight;
  }
  d->m_ScreenCapture->setIsGrabCenter(true);
  d->m_ScreenCapture->setGrabArea(QRect(0, 0, width, height));
}

void NXScreenCaptureManager::setGrabArea(int x, int y, int width, int height)
{
  Q_D(NXScreenCaptureManager);
  QScreen *screen = QGuiApplication::primaryScreen();
  int maxWidth    = screen ? screen->size().width() : 1920;
  int maxHeight   = screen ? screen->size().height() : 1080;
  if (width <= 0 || width > maxWidth) {
    width = maxWidth;
  }
  if (height <= 0 || height > maxHeight) {
    height = maxHeight;
  }
  d->m_ScreenCapture->setIsGrabCenter(false);
  d->m_ScreenCapture->setGrabArea(QRect(x, y, width, height));
}

QRect NXScreenCaptureManager::getGrabArea() const
{
  Q_CONST_D(NXScreenCaptureManager);
  return d->m_ScreenCapture->getGrabArea();
}

void NXScreenCaptureManager::setGrabFrameRate(int frameRateValue)
{
  Q_D(NXScreenCaptureManager);
  if (frameRateValue > 0) {
    d->m_ScreenCapture->setGrabFrameRate(frameRateValue);
  }
}

int NXScreenCaptureManager::getGrabFrameRate() const
{
  Q_CONST_D(NXScreenCaptureManager);
  return d->m_ScreenCapture->getGrabFrameRate();
}

Q_PROPERTY_CREATE_CPP(NXScreenCaptureScreen, QPS_PAEMIT(int), BorderRadius)

NXScreenCaptureScreen::NXScreenCaptureScreen(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXScreenCaptureScreenPrivate())
{
  Q_D(NXScreenCaptureScreen);
  d->q_ptr           = this;
  d->m_BorderRadius  = 5;
  d->m_CaptureManager = NXScreenCaptureManager::getInstance();
  setFixedSize(700, 500);
  connect(d->m_CaptureManager, &NXScreenCaptureManager::grabImageUpdate, this, [=](QImage img) {
    if (isVisible()) {
      d->m_Img = std::move(img);
      update();
    }
  });
}

NXScreenCaptureScreen::~NXScreenCaptureScreen()
{
}

void NXScreenCaptureScreen::paintEvent(QPaintEvent *event)
{
  Q_D(NXScreenCaptureScreen);
  if (d->m_CaptureManager->getIsGrabScreen()) {
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect(), d->m_BorderRadius, d->m_BorderRadius);
    painter.drawImage(rect(), d->m_Img);
    painter.restore();
  }
}

void NXScreenCaptureScreen::setIsSyncGrabSize(bool isSyncGrabSize)
{
  Q_D(NXScreenCaptureScreen);
  if (isSyncGrabSize) {
    setFixedSize(d->m_CaptureManager->getGrabArea().size());
  }
}

bool NXScreenCaptureScreen::getIsSyncGrabSize() const
{
  Q_CONST_D(NXScreenCaptureScreen);
  return d->m_IsSyncGrabSize;
}
#endif
