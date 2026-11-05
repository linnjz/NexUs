#ifndef NXSCREENCAPTUREMANAGERPRIVATE_H
#define NXSCREENCAPTUREMANAGERPRIVATE_H
#include <QObject>
#ifdef Q_OS_MAC
#  include <QImage>
#  include "NXProperty.h"
class NXScreenCapture;
class NXScreenCaptureManager;

class NXScreenCaptureManagerPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXScreenCaptureManager)

public:
  explicit NXScreenCaptureManagerPrivate(QObject *parent = nullptr);
  ~NXScreenCaptureManagerPrivate();

private:
  Q_SIGNAL void grabScreen();
  bool m_IsAllowedGrabScreen { false };
  NXScreenCapture *m_ScreenCapture { nullptr };
  QThread *m_CaptureThread { nullptr };
};

class NXScreenCaptureScreen;

class NXScreenCaptureScreenPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXScreenCaptureScreen)
  Q_PROPERTY_CREATE_D(int, BorderRadius)

public:
  explicit NXScreenCaptureScreenPrivate(QObject *parent = nullptr);
  ~NXScreenCaptureScreenPrivate();

private:
  bool m_IsSyncGrabSize { false };
  QImage m_Img;
  NXScreenCaptureManager *m_CaptureManager { nullptr };
};
#endif
#endif // NXSCREENCAPTUREMANAGERPRIVATE_H
