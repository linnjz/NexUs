#include "NXDxgi.h"

#ifdef Q_OS_WIN
#  include <QDebug>
#  include <QElapsedTimer>
#  include <QScopeGuard>

#  include <limits>

NXDxgi::NXDxgi(QObject *parent)
    : QObject { parent }
    , m_DxDeviceID { 0 }
    , m_OutputDeviceID { 0 }
    , m_GrabFrameRate { 120 }
    , m_TimeoutMsValue { 50 }
    , m_IsInitSuccess { false }
    , m_IsGrabCenter { false }
{
}

NXDxgi::~NXDxgi()
{
  releaseInterface();
}

bool NXDxgi::initialize(int dxID, int outputID)
{
  m_IsInitSuccess  = false;
  m_DxDeviceID     = dxID;
  m_OutputDeviceID = outputID;
  releaseInterface();
  ID3D11Device *d3dDevice         = nullptr;
  ID3D11DeviceContext *d3dContext = nullptr;
  D3D_FEATURE_LEVEL feat          = D3D_FEATURE_LEVEL_11_0;
  HRESULT hr                      = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &d3dDevice, &feat, &d3dContext);
  if (FAILED(hr)) {
    m_LastError = "Failed to D3D11CreateDevice ErrorCode = " + QString::number(uint(hr), 16);
    qDebug() << m_LastError;
    if (d3dDevice) {
      d3dDevice->Release();
    }
    if (d3dContext) {
      d3dContext->Release();
    }
    return false;
  }

  IDXGIFactory *dxgiFactory = nullptr;
  hr                        = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void **>(&dxgiFactory));
  if (FAILED(hr)) {
    m_LastError = "创建 DXGI 工厂失败，错误码：" + QString::number(static_cast<uint>(hr), 16);
    qWarning() << m_LastError;
    d3dContext->Release();
    d3dDevice->Release();
    return false;
  }
  IDXGIAdapter *dxgiAdapter = nullptr;
  QVector<IDXGIAdapter *> dxgiAdapterVector;
  for (uint i = 0; dxgiFactory->EnumAdapters(i, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
    dxgiAdapterVector.append(dxgiAdapter);
  }
  dxgiFactory->Release();
  dxgiAdapter = nullptr;
  m_DxDeviceList.clear();
  for (int i = 0; i < dxgiAdapterVector.count(); i++) {
    IDXGIAdapter *adapt = dxgiAdapterVector.at(i);
    DXGI_ADAPTER_DESC desc;
    adapt->GetDesc(&desc);
    m_DxDeviceList.append(QString::fromWCharArray(desc.Description));
  }
  if (dxID >= 0 && dxID < dxgiAdapterVector.count()) {
    dxgiAdapter = dxgiAdapterVector.at(dxID);
  }

  if (!dxgiAdapter) {
    m_LastError = "Failed to found gpu";
    qDebug() << m_LastError;
    d3dDevice->Release();
    d3dContext->Release();
    for (auto adapter : dxgiAdapterVector) {
      adapter->Release();
    }
    return false;
  }

  IDXGIOutput *dxgiOutput = nullptr;
  QVector<IDXGIOutput *> dxgiOutputVector;
  for (uint i = 0; dxgiAdapter->EnumOutputs(i, &dxgiOutput) != DXGI_ERROR_NOT_FOUND; ++i) {
    dxgiOutputVector.append(dxgiOutput);
  }
  for (auto adapter : dxgiAdapterVector) {
    adapter->Release();
  }
  dxgiOutput = nullptr;
  m_OutputDeviceList.clear();
  for (int i = 0; i < dxgiOutputVector.count(); i++) {
    auto const &output = dxgiOutputVector.at(i);
    DXGI_OUTPUT_DESC desc;
    output->GetDesc(&desc);
    m_OutputDeviceList.append(QString::fromWCharArray(desc.DeviceName));
  }
  if (outputID >= 0 && outputID < dxgiOutputVector.count()) {
    dxgiOutput = dxgiOutputVector.at(outputID);
  }

  if (!dxgiOutput) {
    m_LastError = "Failed to found screen!";
    qDebug() << m_LastError;
    d3dDevice->Release();
    d3dContext->Release();
    for (auto output : dxgiOutputVector) {
      output->Release();
    }
    return false;
  }
  IDXGIOutput6 *dxgiOutput6 = nullptr;
  hr                        = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput6), reinterpret_cast<void **>(&dxgiOutput6));
  for (auto output : dxgiOutputVector) {
    output->Release();
  }
  if (FAILED(hr)) {
    m_LastError = "Failed to QueryInterface IDXGIOutput6 ErrorCode = " + QString::number(uint(hr), 16);
    qDebug() << m_LastError;
    d3dDevice->Release();
    d3dContext->Release();
    return false;
  }
  UINT supportedFormatsCount      = 1; // 支持的格式数
  DXGI_FORMAT supportedFormats[1] = { DXGI_FORMAT_B8G8R8A8_UNORM };
  hr                              = dxgiOutput6->DuplicateOutput1(d3dDevice, 0, supportedFormatsCount, supportedFormats, &m_Duplication);
  dxgiOutput6->Release();
  if (FAILED(hr)) {
    m_LastError = "Failed to DuplicateOutput ErrorCode = " + QString::number(uint(hr), 16);
    qDebug() << m_LastError;
    d3dDevice->Release();
    d3dContext->Release();
    if (m_Duplication) {
      m_Duplication->Release();
      m_Duplication = nullptr;
    }
    return false;
  }
  m_Device         = d3dDevice;
  m_Context        = d3dContext;
  m_IsInitSuccess = true;
  return true;
}

void NXDxgi::setIsGrabActive(bool isGrabActive)
{
  m_IsGrabActive = isGrabActive;
}

bool NXDxgi::getIsGrabActive() const
{
  return m_IsGrabActive;
}

void NXDxgi::setIsGrabStopped(bool isGrabStopped)
{
  m_IsGrabStopped.store(isGrabStopped, std::memory_order_release);
}

bool NXDxgi::getIsGrabStopped() const
{
  return m_IsGrabStopped.load(std::memory_order_acquire);
}

QImage NXDxgi::getGrabImage()
{
  QMutexLocker grabLocker(&m_GrabMutex);
  return m_GrabImage;
}

void NXDxgi::onGrabScreen()
{
  if (!m_Duplication || !m_Device || !m_Context) {
    setIsGrabStopped(true);
    return;
  }
  setIsGrabStopped(false);
  auto const stoppedGuard = qScopeGuard([this]() {
    setIsGrabActive(false);
    setIsGrabStopped(true);
  });
  while (m_IsGrabActive) {
    m_GrabTimer.start();
    IDXGIResource *desktopRes = nullptr;
    DXGI_OUTDUPL_FRAME_INFO frameInfo {};
    HRESULT hr = m_Duplication->AcquireNextFrame(m_TimeoutMsValue, &frameInfo, &desktopRes);
    if (FAILED(hr)) {
      if (desktopRes) {
        desktopRes->Release();
      }
      if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
        continue;
      }
      if (!m_IsGrabActive || !initialize(m_DxDeviceID, m_OutputDeviceID)) {
        return;
      }
      continue;
    }

    // 本轮所有退出路径均释放帧；正常路径延至帧间等待结束，减少 DXGI 的重复更新。
    auto const frameGuard = qScopeGuard([this, desktopRes]() {
      if (desktopRes) {
        desktopRes->Release();
      }
      m_Duplication->ReleaseFrame();
    });
    if (!m_IsGrabActive || !desktopRes || !frameInfo.LastPresentTime.QuadPart) {
      continue;
    }
    QImage const grabImage = _captureFrame(desktopRes);
    if (grabImage.isNull()) {
      continue;
    }
    {
      QMutexLocker grabLocker(&m_GrabMutex);
      m_GrabImage = grabImage;
    }
    Q_EMIT grabScreenOver(grabImage);
    if (m_LastGrabTime == 0) {
      m_LastGrabTime = static_cast<qreal>(m_GrabTimer.elapsed()); // 毫秒
    } else {
      m_LastGrabTime = (static_cast<qreal>(m_GrabTimer.elapsed()) + m_LastGrabTime) / 2;
    }
    m_CpuSleepTime = (1000 - m_LastGrabTime * m_GrabFrameRate) / m_GrabFrameRate;
    waitElapsedTime(m_CpuSleepTime * 1000);
    // qDebug() << m_CpuSleepTime << m_LastGrabTime;
  }
}

QImage NXDxgi::_captureFrame(IDXGIResource *desktopResource)
{
  ID3D11Texture2D *textureRes = nullptr;
  HRESULT hr                  = desktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&textureRes));
  if (FAILED(hr)) {
    qWarning() << "获取桌面纹理失败，错误码：" << QString::number(static_cast<uint>(hr), 16);
    return {};
  }
  auto const textureGuard = qScopeGuard([textureRes]() {
    textureRes->Release();
  });
  D3D11_TEXTURE2D_DESC desc {};
  textureRes->GetDesc(&desc);
  if (desc.Format != DXGI_FORMAT_B8G8R8A8_UNORM || desc.Width == 0 || desc.Height == 0 ||
      desc.Width > static_cast<UINT>((std::numeric_limits<int>::max)() / 4) ||
      desc.Height > static_cast<UINT>((std::numeric_limits<int>::max)())) {
    qWarning() << "桌面纹理格式或尺寸无效";
    return {};
  }
  if (m_Texture) {
    D3D11_TEXTURE2D_DESC cachedDesc {};
    m_Texture->GetDesc(&cachedDesc);
    if (cachedDesc.Width != desc.Width || cachedDesc.Height != desc.Height || cachedDesc.Format != desc.Format) {
      m_Texture->Release();
      m_Texture = nullptr;
    }
  }
  if (!m_Texture) {
    D3D11_TEXTURE2D_DESC texDesc {};
    texDesc.Width            = desc.Width;
    texDesc.Height           = desc.Height;
    texDesc.MipLevels        = 1;
    texDesc.ArraySize        = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage            = D3D11_USAGE_STAGING;
    texDesc.Format           = desc.Format;
    texDesc.CPUAccessFlags   = D3D11_CPU_ACCESS_READ;
    hr                       = m_Device->CreateTexture2D(&texDesc, nullptr, &m_Texture);
    if (FAILED(hr)) {
      qWarning() << "创建暂存纹理失败，错误码：" << QString::number(static_cast<uint>(hr), 16);
      return {};
    }
  }
  m_Context->CopyResource(m_Texture, textureRes);
  IDXGISurface *surface = nullptr;
  hr                    = m_Texture->QueryInterface(__uuidof(IDXGISurface), reinterpret_cast<void **>(&surface));
  if (FAILED(hr)) {
    qWarning() << "获取暂存表面失败，错误码：" << QString::number(static_cast<uint>(hr), 16);
    return {};
  }
  auto const surfaceGuard = qScopeGuard([surface]() {
    surface->Release();
  });
  DXGI_MAPPED_RECT map {};
  hr = surface->Map(&map, DXGI_MAP_READ);
  if (FAILED(hr)) {
    qWarning() << "映射桌面纹理失败，错误码：" << QString::number(static_cast<uint>(hr), 16);
    return {};
  }
  auto const mapGuard = qScopeGuard([surface]() {
    surface->Unmap();
  });
  int const width     = static_cast<int>(desc.Width);
  int const height    = static_cast<int>(desc.Height);
  if (!map.pBits || map.Pitch < width * 4) {
    return {};
  }
  QImage const mappedImage(static_cast<uchar const *>(map.pBits), width, height, map.Pitch, QImage::Format_ARGB32);
  QRect grabArea = m_GrabArea;
  if (m_IsGrabCenter) {
    grabArea.moveTo((width - grabArea.width()) / 2, (height - grabArea.height()) / 2);
  }
  // 像素只在映射有效时复制一次；返回值和后续快照不再引用 DXGI 内存。
  return mappedImage.copy(grabArea);
}

void NXDxgi::releaseInterface()
{
  {
    QMutexLocker grabLocker(&m_GrabMutex);
    m_GrabImage = QImage();
  }
  if (m_Texture) {
    m_Texture->Release();
    m_Texture = nullptr;
  }
  if (m_Duplication) {
    m_Duplication->Release();
    m_Duplication = nullptr;
  }
  if (m_Context) {
    m_Context->Release();
    m_Context = nullptr;
  }
  if (m_Device) {
    m_Device->Release();
    m_Device = nullptr;
  }
}

void NXDxgi::waitElapsedTime(qreal usec)
{
  if (usec <= 0) {
    return;
  }
  static QElapsedTimer timer;
  timer.start();
  while (static_cast<qreal>(timer.nsecsElapsed()) / 1000.0 < usec) {
  }
}
#endif
