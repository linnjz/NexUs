#ifndef NXDXGI_H
#define NXDXGI_H

#include <QObject>
#ifdef Q_OS_WIN
#  include <atomic>
#  include <QElapsedTimer>
#  include <QMutex>
#  include <QPixmap>
#  include <QImage>

#  include <d3d11.h>
#  include <dxgi1_6.h>

#  include "NXProperty.h"

class NXDxgi : public QObject
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QRect, GrabArea)
  Q_PRIVATE_CREATE(QPS_SCREF(QStringList), DxDeviceList)
  Q_PRIVATE_CREATE(QPS_SCREF(QStringList), OutputDeviceList)
  Q_PRIVATE_CREATE(QPS_SCREF(QString), LastError)
  Q_PRIVATE_CREATE(int, DxDeviceID)
  Q_PRIVATE_CREATE(int, OutputDeviceID)
  Q_PRIVATE_CREATE(int, GrabFrameRate)
  Q_PRIVATE_CREATE(int, TimeoutMsValue)
  Q_PRIVATE_CREATE(bool, IsInitSuccess)
  Q_PRIVATE_CREATE(bool, IsGrabCenter)

public:
  explicit NXDxgi(QObject *parent = nullptr);
  ~NXDxgi();
  bool initialize(int dxID, int outputID);

  void setIsGrabActive(bool isGrabActive);
  bool getIsGrabActive() const;
  void setIsGrabStopped(bool isGrabStopped);
  bool getIsGrabStopped() const;

  QImage getGrabImage();

  Q_SLOT void onGrabScreen();
  Q_SIGNAL void grabScreenOver(QImage const &img);

private:
  std::atomic_bool m_IsGrabActive { false };
  std::atomic_bool m_IsGrabStopped { true };
  qreal m_LastGrabTime { 0 };
  qreal m_CpuSleepTime { 0 };
  QMutex m_GrabMutex;
  QElapsedTimer m_GrabTimer;
  IDXGIOutputDuplication *m_Duplication { nullptr };
  ID3D11Device *m_Device { nullptr };
  ID3D11DeviceContext *m_Context { nullptr };
  ID3D11Texture2D *m_Texture { nullptr }; // 同尺寸、同格式的 CPU 读取纹理在帧间复用。
  QImage m_GrabImage;                     // 持有独立像素的裁剪快照；发布后只读，访问由 m_GrabMutex 保护。
  QImage _captureFrame(IDXGIResource *desktopResource);

  void releaseInterface();
  static void waitElapsedTime(qreal usec);
};
#endif
#endif // NXDXGI_H
