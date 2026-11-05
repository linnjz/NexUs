#include "T_NXPacketIO.h"

#if defined(Q_OS_WIN) && defined(BUILD_WITH_NXPACKETIO)
#  include <QApplication>
#  include <QDebug>
#  include <QElapsedTimer>
#  include "NXDxgiManager.h"
#  include "NXXIO_Connection.h"
#  include "NXXIO_Interface.h"
#  include "NXXIO_PacketRegistry.h"

T_NXPacketIO::T_NXPacketIO(QObject *parent)
    : QObject(parent)
{
  m_IsActive    = true;
  m_InterfaceIP = QStringLiteral("192.168.1");
}

T_NXPacketIO::~T_NXPacketIO()
{
}

void T_NXPacketIO::handleGrabImage()
{
  _interface = std::make_unique<NXXIO_Interface>();
  NXXIO_Interface::UDP_Target udp_target;
  udp_target._address      = "239.1.1.1";
  udp_target._sendPort     = 8001;
  udp_target._recvPort     = 8001;
  udp_target._interfaceIP  = m_InterfaceIP.toLocal8Bit().data();
  udp_target._type         = NXXIO_Interface::Multicast;
  udp_target._connectionId = 30;
  _interface->addUDP_Target(udp_target);
  _interface->setApplicationName("T_NXPacketIO_Send");
  CONNECT_HANDLE_FUNC_DEFINE(_interface, [this](NXXIO_Connection *connection) {
    if (connection->isReliable()) {
      qDebug() << "Recv Link Message! ApplicationName: "
               << QString::fromLocal8Bit(connection->getApplicationName().data());
      if (connection->getApplicationName() == "T_NXPacketIO_Recv") {
        _connection = connection;
      }
    }
  });
  DISCONNECT_HANDLE_FUNC_DEFINE(_interface, [this](NXXIO_Connection *connection) {
    qDebug() << "Recv DisConnect Message! ApplicationName: "
             << QString::fromLocal8Bit(connection->getApplicationName().data());
    if (connection->getApplicationName() == "T_NXPacketIO_Recv") {
      _connection = nullptr;
    }
  });
  _interface->init(53000);

  QElapsedTimer sendTimer;
  while (m_IsActive) {
    //sendTimer.start();
    if (!NXDxgiManager::getInstance()->getIsGrabScreen()) {
      std::this_thread::sleep_for(std::chrono::microseconds(2));
      continue;
    }
    QImage image = NXDxgiManager::getInstance()->grabScreenToImage();
    if (image.isNull()) {
      std::this_thread::sleep_for(std::chrono::microseconds(2));
      continue;
    }
    image           = image.scaled(image.width(), image.height(), Qt::KeepAspectRatio, Qt::FastTransformation)
                          .scaled(image.width() / 2, image.height() / 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    char *imageData = (char *) image.bits();

    int imageWidth   = image.width();
    int imageHeight  = image.height();
    int dataTotalLen = image.width() * image.height() * 4;
    // qDebug() << dataTotalLen << imageWidth << imageHeight;
    if (!_connection) {
      std::this_thread::sleep_for(std::chrono::microseconds(2));
      continue;
    }
    for (int i = 0; i < dataTotalLen / 1024; i++) {
      NXXIO_ScreenPkt screenPkt;
      screenPkt._imageWidth   = imageWidth;
      screenPkt._imageHeight  = imageHeight;
      screenPkt._dataID       = i;
      screenPkt._dataOffset   = i * 1024;
      screenPkt._dataTotalLen = dataTotalLen;
      if (dataTotalLen - i * 1024 >= 1024) {
        screenPkt._currentDataLen = 1024;
      } else {
        screenPkt._currentDataLen = dataTotalLen - i * 1024;
      }
      memcpy(screenPkt._data, imageData + screenPkt._dataOffset, 1024);
      _sendToXIO(screenPkt);
      //  和下面这句等效 但下方写法效率较低
      //  _interface->send(screenPkt, _interface->GetConnections()[0]);
    }
    //qDebug() << sendTimer.elapsed();
  }
}

void T_NXPacketIO::handleImagePacket()
{
  _interface = std::make_unique<NXXIO_Interface>();
  NXXIO_Interface::UDP_Target udp_target;
  udp_target._address      = "239.1.1.1";
  udp_target._sendPort     = 8001;
  udp_target._recvPort     = 8001;
  udp_target._interfaceIP  = m_InterfaceIP.toLocal8Bit().data();
  udp_target._type         = NXXIO_Interface::Multicast;
  udp_target._connectionId = 40;
  _interface->addUDP_Target(udp_target);
  _interface->setApplicationName("T_NXPacketIO_Recv");

  PACKET_HANDLE_FUNC_DEFINE(_interface, &T_NXPacketIO::_handleScreenPkt, this);
  CONNECT_HANDLE_FUNC_DEFINE(_interface, [this](NXXIO_Connection *connection) {
    if (connection->isReliable()) {
      qDebug() << "Recv Link Message! ApplicationName: "
               << QString::fromLocal8Bit(connection->getApplicationName().data());
      if (connection->getApplicationName() == "T_NXPacketIO_Send") {
        _connection = connection;
      }
    }
  });
  _interface->init(53001);
}

void T_NXPacketIO::_handleScreenPkt(NXXIO_ScreenPkt &screenPkt)
{
  if (screenPkt._dataOffset == 0) {
    // 新的一帧开始
    //recvTimer.start();
    _imageArray.clear();
    _imageArray.append(screenPkt._data, screenPkt._currentDataLen);
  } else {
    if (_imageArray.isEmpty()) {
      return;
    }
    _imageArray.append(screenPkt._data, screenPkt._currentDataLen);
    if (screenPkt._currentDataLen + screenPkt._dataOffset == screenPkt._dataTotalLen) {
      // 尾包 开始生成图片
      if (_imageArray.size() == screenPkt._dataTotalLen) {
        //qDebug() << recvTimer.elapsed();
        uchar *imageData = (uchar *) (_imageArray.data());
        QImage recvImage = QImage(imageData, screenPkt._imageWidth, screenPkt._imageHeight, QImage::Format_ARGB32);
        Q_EMIT sendHandleResult(std::move(QPixmap::fromImage(recvImage)));
      } else {
        // 包大小不正确 丢包
        qDebug() << u8"丢包" << screenPkt._dataID << _lastImageIndex;
        _imageArray.clear();
      }
    } else {
      // 中间包处理
      if (_lastImageIndex != screenPkt._dataID - 1) {
        // ID不连续 丢包
        qDebug() << u8"丢包" << screenPkt._dataID << _lastImageIndex;
        _imageArray.clear();
      }
    }
  }
  _lastImageIndex = screenPkt._dataID;
}

void T_NXPacketIO::_sendToXIO(NXXIO_Packet &packet, bool isMulticast)
{
  if (isMulticast) {
    if (!_multicastConnection) {
      std::vector<NXXIO_Interface::UDP_Target> const &udpTargetVector = _interface->getUDP_Targets();
      if (udpTargetVector.empty()) {
        return;
      }
      _multicastConnection = _interface->FindConnection(udpTargetVector[0]._connectionId);
    }
    if (_multicastConnection && _multicastConnection->isInitialized()) {
      _multicastConnection->send(packet);
    }
  } else {
    if (_connection) {
      _connection->send(packet);
    }
  }
}
#endif
