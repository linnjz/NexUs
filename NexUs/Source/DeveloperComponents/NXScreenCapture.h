#ifndef NXSCREENCAPTURE_H
#define NXSCREENCAPTURE_H

#include <QObject>
#ifdef Q_OS_MAC
#  include <QElapsedTimer>
#  include <QImage>

#  include "NXProperty.h"

class NXScreenCapture : public QObject
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QRect, GrabArea)
  Q_PRIVATE_CREATE(QPS_SCREF(QStringList), DisplayList)
  Q_PRIVATE_CREATE(QPS_SCREF(QString), LastError)
  Q_PRIVATE_CREATE(int, DisplayID)
  Q_PRIVATE_CREATE(int, GrabFrameRate)
  Q_PRIVATE_CREATE(bool, IsGrabActive)
  Q_PRIVATE_CREATE(bool, IsInitSuccess)
  Q_PRIVATE_CREATE(bool, IsGrabStoped)
  Q_PRIVATE_CREATE(bool, IsGrabCenter)

public:
  explicit NXScreenCapture(QObject *parent = nullptr);
  ~NXScreenCapture();
  bool initialize(int displayID);
  QImage getGrabImage() const;
  Q_SLOT void onGrabScreen();
  Q_SIGNAL void grabScreenOver(QImage const &img);

private:
  int m_DisplayWidth { 0 };
  int m_DisplayHeight { 0 };
  qint64 m_LastGrabTime { 0 };
  qint64 m_CpuSleepTime { 0 };
  QImage m_LastImage;
  Private *d { nullptr };
#endif
#endif // NXSCREENCAPTURE_H
