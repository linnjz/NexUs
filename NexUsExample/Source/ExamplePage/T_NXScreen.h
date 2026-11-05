#ifndef T_NXSCREEN_H
#define T_NXSCREEN_H
#include <QWidget>

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
#  include "T_BasePage.h"

#  ifdef Q_OS_WIN
class NXImageCard;
#  endif
#  ifdef Q_OS_MAC
class NXScreenCaptureScreen;
#  endif
class NXComboBox;
#  if defined(Q_OS_WIN) && defined(BUILD_WITH_NXPACKETIO)
class T_NXPacketIO;
#  endif
class T_RecvScreen;

class T_NXScreen : public T_BasePage
{
  Q_OBJECT

public:
  Q_INVOKABLE explicit T_NXScreen(QWidget *parent = nullptr);
  ~T_NXScreen();

private:
#  ifdef Q_OS_WIN
  NXImageCard *_dxgiScreen { nullptr };
  NXComboBox *_dxComboBox { nullptr };
  NXComboBox *_outputComboBox { nullptr };
#  endif
#  ifdef Q_OS_MAC
  NXScreenCaptureScreen *_captureScreen { nullptr };
  NXComboBox *_displayComboBox { nullptr };
#  endif
#  if defined(Q_OS_WIN) && defined(BUILD_WITH_NXPACKETIO)
  T_NXPacketIO *_packetSendIO { nullptr };
  T_NXPacketIO *_packetRecvIO { nullptr };
  T_RecvScreen *_recvScreen { nullptr };
  QThread *_packetIOSendThread { nullptr };
  QThread *_packetIORecvThread { nullptr };
  void _initSendThread(QString const &interfaceIP);
  void _initRecvThread(QString const &interfaceIP);
  void _unInitThread(bool isSend);
#  endif
};
#endif
#endif // T_NXSCREEN_H
