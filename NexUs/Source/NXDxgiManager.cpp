#include "NXDxgiManager.h"
#ifdef Q_OS_WIN
#  include <d3d11.h>
#  include <dxgi1_6.h>
#  include <windows.h>

#  include <QApplication>
#  include <QDebug>
#  include <QPainter>
#  include <QPainterPath>
#  include <QThread>
#  include <QUrl>

#  include "NXDxgi.h"
#  include "NXDxgiManagerPrivate.h"

NXDxgiManager::NXDxgiManager(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXDxgiManagerPrivate())
{
  Q_D(NXDxgiManager);
  d->q_ptr       = this;
  d->m_DxgiThread = new QThread(this);
  d->m_Dxgi       = new NXDxgi();
  setGrabArea(0, 0, GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
  bool ret = d->m_Dxgi->initialize(0, 0);
  if (!ret) {
    for (int i = 1; i < d->m_Dxgi->getDxDeviceList().count(); i++) {
      ret = d->m_Dxgi->initialize(i, 0);
      if (ret) {
        break;
      }
    }
  }
  if (!ret) {
    d->m_Dxgi->initialize(0, 0);
    qCritical() << "No available screenshot devices";
  }
  d->m_Dxgi->moveToThread(d->m_DxgiThread);
  d->m_DxgiThread->start();
  connect(d, &NXDxgiManagerPrivate::grabScreen, d->m_Dxgi, &NXDxgi::onGrabScreen);
  connect(d->m_Dxgi, &NXDxgi::grabScreenOver, this, &NXDxgiManager::grabImageUpdate);
  connect(d->m_DxgiThread, &QThread::finished, d->m_Dxgi, &NXDxgi::deleteLater);
}

NXDxgiManager::~NXDxgiManager()
{
  Q_D(NXDxgiManager);
  if (d->m_Dxgi) {
    d->m_Dxgi->setIsGrabActive(false);
  }
  if (d->m_DxgiThread->isRunning()) {
    d->m_DxgiThread->quit();
    d->m_DxgiThread->wait();
  }
  delete d->m_DxgiThread;
}

QStringList NXDxgiManager::getDxDeviceList() const
{
  Q_CONST_D(NXDxgiManager);
  return d->m_Dxgi->getDxDeviceList();
}

QStringList NXDxgiManager::getOutputDeviceList() const
{
  Q_CONST_D(NXDxgiManager);
  return d->m_Dxgi->getOutputDeviceList();
}

QImage NXDxgiManager::grabScreenToImage() const
{
  Q_CONST_D(NXDxgiManager);
  return d->m_Dxgi->getGrabImage();
}

void NXDxgiManager::startGrabScreen()
{
  Q_D(NXDxgiManager);
  d->m_IsAllowedGrabScreen = true;
  if (!d->m_Dxgi->getIsGrabActive()) {
    d->m_Dxgi->setIsGrabActive(true);
    Q_EMIT d->grabScreen();
  }
}

void NXDxgiManager::stopGrabScreen()
{
  Q_D(NXDxgiManager);
  d->m_IsAllowedGrabScreen = false;
  d->m_Dxgi->setIsGrabActive(false);
}

bool NXDxgiManager::getIsGrabScreen() const
{
  Q_CONST_D(NXDxgiManager);
  return d->m_Dxgi->getIsGrabActive();
}

bool NXDxgiManager::setDxDeviceID(int dxID)
{
  Q_D(NXDxgiManager);
  if (dxID < 0 || d->m_Dxgi->getDxDeviceList().count() <= dxID) {
    return false;
  }
  d->m_Dxgi->setIsGrabActive(false);
  while (!d->m_Dxgi->getIsGrabStopped()) {
    //等待任务结束
    QApplication::processEvents();
  }
  if (d->m_Dxgi->initialize(dxID, d->m_Dxgi->getOutputDeviceID())) {
    if (d->m_IsAllowedGrabScreen) {
      d->m_Dxgi->setIsGrabActive(true);
      Q_EMIT d->grabScreen();
    }
    return true;
  }
  return false;
}

int NXDxgiManager::getDxDeviceID() const
{
  Q_CONST_D(NXDxgiManager);
  return d->m_Dxgi->getDxDeviceID();
}

bool NXDxgiManager::setOutputDeviceID(int deviceID)
{
  Q_D(NXDxgiManager);
  if (deviceID < 0 || d->m_Dxgi->getOutputDeviceList().count() <= deviceID) {
    return false;
  }

  d->m_Dxgi->setIsGrabActive(false);
  while (!d->m_Dxgi->getIsGrabStopped()) {
    //等待任务结束
    QApplication::processEvents();
  }
  if (d->m_Dxgi->initialize(d->m_Dxgi->getDxDeviceID(), deviceID)) {
    if (d->m_IsAllowedGrabScreen) {
      d->m_Dxgi->setIsGrabActive(true);
      Q_EMIT d->grabScreen();
    }
    return true;
  }
  return false;
}

int NXDxgiManager::getOutputDeviceID() const
{
  Q_CONST_D(NXDxgiManager);
  return d->m_Dxgi->getOutputDeviceID();
}

void NXDxgiManager::setGrabArea(int width, int height)
{
  Q_D(NXDxgiManager);
  int maxWidth  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  int maxHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  if (width <= 0 || width > maxWidth) {
    width = maxWidth;
  }
  if (height <= 0 || height > maxHeight) {
    height = maxHeight;
  }
  d->m_Dxgi->setIsGrabCenter(true);
  d->m_Dxgi->setGrabArea(QRect(0, 0, width, height));
}

void NXDxgiManager::setGrabArea(int x, int y, int width, int height)
{
  Q_D(NXDxgiManager);
  int maxWidth  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  int maxHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  if (width <= 0 || width > maxWidth) {
    width = maxWidth;
  }
  if (height <= 0 || height > maxHeight) {
    height = maxHeight;
  }
  d->m_Dxgi->setIsGrabCenter(false);
  d->m_Dxgi->setGrabArea(QRect(x, y, width, height));
}

QRect NXDxgiManager::getGrabArea() const
{
  Q_CONST_D(NXDxgiManager);
  return d->m_Dxgi->getGrabArea();
}

void NXDxgiManager::setGrabFrameRate(int frameRateValue)
{
  Q_D(NXDxgiManager);
  if (frameRateValue > 0) {
    d->m_Dxgi->setGrabFrameRate(frameRateValue);
  }
}

int NXDxgiManager::getGrabFrameRate() const
{
  Q_CONST_D(NXDxgiManager);
  return d->m_Dxgi->getGrabFrameRate();
}

void NXDxgiManager::setTimeoutMsValue(int timeoutValue)
{
  Q_D(NXDxgiManager);
  if (timeoutValue > 0) {
    d->m_Dxgi->setTimeoutMsValue(timeoutValue);
  }
}

int NXDxgiManager::getTimeoutMsValue() const
{
  Q_CONST_D(NXDxgiManager);
  return d->m_Dxgi->getTimeoutMsValue();
}

#endif
