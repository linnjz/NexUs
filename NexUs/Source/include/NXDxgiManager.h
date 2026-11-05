#ifndef NXDXGIMANAGER_H
#define NXDXGIMANAGER_H

#include <QWidget>

#ifdef Q_OS_WIN
#  include "LinnSingleton.h"
#  include "NXProperty.h"

class NXDxgiManagerPrivate;

class NX_EXPORT NXDxgiManager : public QObject
{
  Q_OBJECT
  Q_Q_CREATE(NXDxgiManager)
  Q_SINGLETON_CREATE(QSS_UNIQUE(NXDxgiManager))

private:
  explicit NXDxgiManager(QObject *parent = nullptr);
  ~NXDxgiManager();

public:
  QStringList getDxDeviceList() const;
  QStringList getOutputDeviceList() const;
  QImage grabScreenToImage() const;
  void startGrabScreen();
  void stopGrabScreen();
  bool getIsGrabScreen() const;
  bool setDxDeviceID(int dxID);
  int getDxDeviceID() const;
  bool setOutputDeviceID(int deviceID);
  int getOutputDeviceID() const;
  void setGrabArea(int width, int height); //从屏幕中心向外延伸
  void setGrabArea(int x, int y, int width, int height);
  QRect getGrabArea() const;
  void setGrabFrameRate(int frameRateValue);
  int getGrabFrameRate() const;
  void setTimeoutMsValue(int timeoutValue);
  int getTimeoutMsValue() const;
  Q_SIGNAL void grabImageUpdate(QImage const &img);
};

#endif // Q_OS_WIN
#endif // NXDXGIMANAGER_H
