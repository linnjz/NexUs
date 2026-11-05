#ifndef NXFRAMEWORK_NEXUSEXAMPLE_EXAMPLEPAGE_T_NXPACKETIO_H_
#define NXFRAMEWORK_NEXUSEXAMPLE_EXAMPLEPAGE_T_NXPACKETIO_H_

#include <QObject>
#if defined(Q_OS_WIN) && defined(BUILD_WITH_NXPACKETIO)
#  include <QImage>
#  include <QMutex>
#  include <QQueue>
#  include "NXProperty.h"
#  include "NXXIO_PacketRegistry.h"
#  include "UtCallbackHolder.h"
class NXXIO_Interface;
class PakPacket;
class NXXIO_Connection;

class T_NXPacketIO : public QObject
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QPS_SCREF(QString), InterfaceIP)
  Q_PRIVATE_CREATE(bool, IsActive)

public:
  Q_INVOKABLE explicit T_NXPacketIO(QObject *parent = nullptr);
  ~T_NXPacketIO();

  Q_SLOT void handleGrabImage();
  Q_SLOT void handleImagePacket();

  Q_SIGNAL void sendHandleResult(QPixmap const &map);

private:
  int _lastImageIndex { 0 };
  std::unique_ptr<NXXIO_Interface> _interface;
  NXXIO_Connection *_connection { nullptr };
  NXXIO_Connection *_multicastConnection { nullptr };
  QByteArray _imageArray;
  QMutex _mutex;
  UtCallbackHolder _callback;
  void _handleScreenPkt(NXXIO_ScreenPkt &screenPkt);
  void _sendToXIO(NXXIO_Packet &packet, bool isMulticast = false);
};
#endif
#endif //NXFRAMEWORK_NEXUSEXAMPLE_EXAMPLEPAGE_T_NXPACKETIO_H_
