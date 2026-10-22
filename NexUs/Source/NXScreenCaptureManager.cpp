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
  d->_captureThread = new QThread(this);
  d->_screenCapture = new NXScreenCapture();

  QScreen *screen  = QGuiApplication::primaryScreen();
  QSize screenSize = screen ? screen->size() : QSize(1920, 1080);
  setGrabArea(0, 0, screenSize.width(), screenSize.height());

  bool ret = d->_screenCapture->initialize(0);
  if (!ret) {
    for (int i = 1; i < d->_screenCapture->getDisplayList().count(); i++) {
      ret = d->_screenCapture->initialize(i);
      if (ret) {
        break;
      }
    }
  }
  if (!ret) {
    d->_screenCapture->initialize(0);
    qCritical() << "No available screenshot devices";
  }
  d->_screenCapture->moveToThread(d->_captureThread);
  d->_captureThread->start();
  connect(d, &NXScreenCaptureManagerPrivate::grabScreen, d->_screenCapture, &NXScreenCapture::onGrabScreen);
  connect(d->_screenCapture, &NXScreenCapture::grabScreenOver, this, &NXScreenCaptureManager::grabImageUpdate);
}

NXScreenCaptureManager::~NXScreenCaptureManager()
{
  Q_D(NXScreenCaptureManager);
  if (d->_screenCapture) {
    d->_screenCapture->setIsGrabActive(false);
  }
  if (d->_captureThread->isRunning()) {
    d->_captureThread->quit();
    d->_captureThread->wait();
  }
  delete d->_screenCapture;
}

QStringList NXScreenCaptureManager::getDisplayList() const
{
  Q_D(const NXScreenCaptureManager);
  return d->_screenCapture->getDisplayList();
}

QImage NXScreenCaptureManager::grabScreenToImage() const
{
  Q_D(const NXScreenCaptureManager);
  if (!d->_screenCapture->getIsInitSuccess()) {
    return QImage();
  }
  return d->_screenCapture->getGrabImage();
}

void NXScreenCaptureManager::startGrabScreen()
{
  Q_D(NXScreenCaptureManager);
  d->_isAllowedGrabScreen = true;
  if (!d->_screenCapture->getIsGrabActive()) {
    d->_screenCapture->setIsGrabActive(true);
    Q_EMIT d->grabScreen();
  }
}

void NXScreenCaptureManager::stopGrabScreen()
{
  Q_D(NXScreenCaptureManager);
  d->_isAllowedGrabScreen = false;
  d->_screenCapture->setIsGrabActive(false);
}

bool NXScreenCaptureManager::getIsGrabScreen() const
{
  Q_D(const NXScreenCaptureManager);
  return d->_screenCapture->getIsGrabActive();
}

bool NXScreenCaptureManager::setDisplayID(int displayID)
{
  Q_D(NXScreenCaptureManager);
  if (displayID < 0 || d->_screenCapture->getDisplayList().count() <= displayID) {
    return false;
  }
  d->_screenCapture->setIsGrabActive(false);
  while (!d->_screenCapture->getIsGrabStoped()) {
    QApplication::processEvents();
  }
  if (d->_screenCapture->initialize(displayID)) {
    if (d->_isAllowedGrabScreen) {
      d->_screenCapture->setIsGrabActive(true);
      Q_EMIT d->grabScreen();
    }
    return true;
  }
  return false;
}

int NXScreenCaptureManager::getDisplayID() const
{
  Q_D(const NXScreenCaptureManager);
  return d->_screenCapture->getDisplayID();
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
  d->_screenCapture->setIsGrabCenter(true);
  d->_screenCapture->setGrabArea(QRect(0, 0, width, height));
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
  d->_screenCapture->setIsGrabCenter(false);
  d->_screenCapture->setGrabArea(QRect(x, y, width, height));
}

QRect NXScreenCaptureManager::getGrabArea() const
{
  Q_D(const NXScreenCaptureManager);
  return d->_screenCapture->getGrabArea();
}

void NXScreenCaptureManager::setGrabFrameRate(int frameRateValue)
{
  Q_D(NXScreenCaptureManager);
  if (frameRateValue > 0) {
    d->_screenCapture->setGrabFrameRate(frameRateValue);
  }
}

int NXScreenCaptureManager::getGrabFrameRate() const
{
  Q_D(const NXScreenCaptureManager);
  return d->_screenCapture->getGrabFrameRate();
}

Q_PROPERTY_CREATE_CPP(NXScreenCaptureScreen, int, BorderRadius)

NXScreenCaptureScreen::NXScreenCaptureScreen(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXScreenCaptureScreenPrivate())
{
  Q_D(NXScreenCaptureScreen);
  d->q_ptr           = this;
  d->_pBorderRadius  = 5;
  d->_captureManager = NXScreenCaptureManager::getInstance();
  setFixedSize(700, 500);
  connect(d->_captureManager, &NXScreenCaptureManager::grabImageUpdate, this, [=](QImage img) {
    if (isVisible()) {
      d->_img = std::move(img);
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
  if (d->_captureManager->getIsGrabScreen()) {
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect(), d->_pBorderRadius, d->_pBorderRadius);
    painter.drawImage(rect(), d->_img);
    painter.restore();
  }
}

void NXScreenCaptureScreen::setIsSyncGrabSize(bool isSyncGrabSize)
{
  Q_D(NXScreenCaptureScreen);
  if (isSyncGrabSize) {
    setFixedSize(d->_captureManager->getGrabArea().size());
  }
}

bool NXScreenCaptureScreen::getIsSyncGrabSize() const
{
  Q_D(const NXScreenCaptureScreen);
  return d->_isSyncGrabSize;
}
#endif
