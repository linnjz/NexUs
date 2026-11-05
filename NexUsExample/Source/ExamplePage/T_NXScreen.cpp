#include "T_NXScreen.h"

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
#  include <QThread>
#  include <QTimer>
#  include <QVBoxLayout>
#  include "NXComboBox.h"
#  include "NXScrollPageArea.h"
#  include "NXText.h"
#  include "NXToggleButton.h"

#  ifdef Q_OS_WIN
#    include "NXDxgiManager.h"
#    include "NXImageCard.h"
#    include "NXLineEdit.h"
#    include "T_NXPacketIO.h"
#    include "T_RecvScreen.h"
#  endif

#  ifdef Q_OS_MAC
#    include "NXScreenCaptureManager.h"
#  endif

T_NXScreen::T_NXScreen(QWidget *parent)
    : T_BasePage(parent)
{
  setWindowTitle(QStringLiteral("NXScreen"));

#  ifdef Q_OS_WIN
  createCustomWidget(QStringLiteral("DXGI录制组件被放置于此，可在此界面预览录制效果"));

  NXDxgiManager *dxgiManager = NXDxgiManager::getInstance();
  dxgiManager->setGrabArea(1920, 1080);

  NXScrollPageArea *dxgiScreenArea = new NXScrollPageArea(this);
  dxgiScreenArea->setFixedHeight(700);
  QHBoxLayout *dxgiScreenLayout = new QHBoxLayout(dxgiScreenArea);
  _dxgiScreen                   = new NXImageCard(this);
  _dxgiScreen->setFixedHeight(678);
  dxgiScreenLayout->addWidget(_dxgiScreen);
  connect(dxgiManager, &NXDxgiManager::grabImageUpdate, this, [=](QImage const &img) {
    _dxgiScreen->setCardImage(img);
  });

  NXText *dxText = new NXText(QStringLiteral("显卡选择"), this);
  dxText->setTextPixelSize(15);
  _dxComboBox = new NXComboBox(this);
  _dxComboBox->addItems(dxgiManager->getDxDeviceList());
  _dxComboBox->setCurrentIndex(dxgiManager->getDxDeviceID());

  NXText *outputText = new NXText(QStringLiteral("屏幕选择"), this);
  outputText->setTextPixelSize(15);
  _outputComboBox = new NXComboBox(this);
  _outputComboBox->addItems(dxgiManager->getOutputDeviceList());
  _outputComboBox->setCurrentIndex(dxgiManager->getOutputDeviceID());

  connect(_dxComboBox, QOverload<int>::of(&NXComboBox::currentIndexChanged), this, [=](int index) {
    dxgiManager->setDxDeviceID(index);
    _outputComboBox->blockSignals(true);
    _outputComboBox->clear();
    _outputComboBox->addItems(dxgiManager->getOutputDeviceList());
    _outputComboBox->setCurrentIndex(dxgiManager->getOutputDeviceID());
    _outputComboBox->blockSignals(false);
    _dxgiScreen->update();
  });
  connect(_outputComboBox, QOverload<int>::of(&NXComboBox::currentIndexChanged), this, [=](int index) {
    dxgiManager->setOutputDeviceID(index);
    _dxgiScreen->update();
  });

  NXToggleButton *startButton = new NXToggleButton(QStringLiteral("捕获"), this);
  connect(startButton, &NXToggleButton::toggled, this, [=](bool isToggled) {
    if (isToggled) {
      dxgiManager->startGrabScreen();
    } else {
      dxgiManager->stopGrabScreen();
      _dxgiScreen->setCardImage({});
      _dxgiScreen->update();
    }
  });

  QHBoxLayout *comboBoxLayout = new QHBoxLayout();
  comboBoxLayout->addWidget(dxText);
  comboBoxLayout->addWidget(_dxComboBox);
  comboBoxLayout->addSpacing(10);
  comboBoxLayout->addWidget(outputText);
  comboBoxLayout->addWidget(_outputComboBox);
  comboBoxLayout->addWidget(startButton);
  comboBoxLayout->addStretch();

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("NXScreen"));
  QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
  centerLayout->setContentsMargins(0, 0, 0, 0);
  centerLayout->addLayout(comboBoxLayout);
  centerLayout->addWidget(dxgiScreenArea);

#    if defined(BUILD_WITH_NXPACKETIO)
  QHBoxLayout *packetLayout = new QHBoxLayout();
  NXText *packetIOText =
      new NXText(QStringLiteral("网络视图 (需要先进行屏幕捕获 若接口IP不正确或不可用 程序可能会崩溃)"), this);
  packetIOText->setTextPixelSize(17);

  NXText *interfaceIPText = new NXText(QStringLiteral("接口IP"), this);
  interfaceIPText->setTextPixelSize(15);
  NXLineEdit *interfaceIPEdit = new NXLineEdit(this);
  interfaceIPEdit->setMaximumWidth(140);
  interfaceIPEdit->setFixedHeight(33);
  interfaceIPEdit->setPlaceholderText(QStringLiteral("接口IP"));
  interfaceIPEdit->setText(QStringLiteral("192.168.1"));

  NXToggleButton *sendButton2 = new NXToggleButton(QStringLiteral("初始发送"), this);
  connect(sendButton2, &NXToggleButton::toggled, this, [=](bool isToggled) {
    if (isToggled) {
      _initSendThread(interfaceIPEdit->text().trimmed());
    } else {
      _unInitThread(true);
    }
  });
  NXToggleButton *recvButton = new NXToggleButton(QStringLiteral("初始接收"), this);
  connect(recvButton, &NXToggleButton::toggled, this, [=](bool isToggled) {
    if (isToggled) {
      _initRecvThread(interfaceIPEdit->text().trimmed());
    } else {
      _unInitThread(false);
    }
  });
  packetLayout->addWidget(interfaceIPText);
  packetLayout->addWidget(interfaceIPEdit);
  packetLayout->addWidget(sendButton2);
  packetLayout->addWidget(recvButton);
  packetLayout->addStretch();
  _recvScreen                      = new T_RecvScreen(this);
  NXScrollPageArea *recvScreenArea = new NXScrollPageArea(this);
  recvScreenArea->setFixedHeight(700);
  QHBoxLayout *recvScreenLayout = new QHBoxLayout(recvScreenArea);
  recvScreenLayout->addWidget(_recvScreen);
  centerLayout->addSpacing(30);
  centerLayout->addWidget(packetIOText);
  centerLayout->addLayout(packetLayout);
  centerLayout->addWidget(recvScreenArea);
#    endif

  addCentralWidget(centralWidget, false, true);
#  endif

#  ifdef Q_OS_MAC
  createCustomWidget(QStringLiteral("屏幕录制组件被放置于此，可在此界面预览录制效果"));

  NXScreenCaptureManager *captureManager = NXScreenCaptureManager::getInstance();
  captureManager->setGrabArea(1920, 1080);

  NXScrollPageArea *captureScreenArea = new NXScrollPageArea(this);
  captureScreenArea->setFixedHeight(700);
  QHBoxLayout *captureScreenLayout = new QHBoxLayout(captureScreenArea);
  _captureScreen                   = new NXScreenCaptureScreen(this);
  _captureScreen->setFixedSize(1200, 678);
  captureScreenLayout->addWidget(_captureScreen);

  NXText *displayText = new NXText(QStringLiteral("显示器选择"), this);
  displayText->setTextPixelSize(15);
  _displayComboBox = new NXComboBox(this);
  _displayComboBox->addItems(captureManager->getDisplayList());
  _displayComboBox->setCurrentIndex(captureManager->getDisplayID());

  connect(_displayComboBox, QOverload<int>::of(&NXComboBox::currentIndexChanged), this, [=](int index) {
    captureManager->setDisplayID(index);
    _captureScreen->update();
  });

  NXToggleButton *startButton = new NXToggleButton(QStringLiteral("捕获"), this);
  connect(startButton, &NXToggleButton::toggled, this, [=](bool isToggled) {
    if (isToggled) {
      captureManager->startGrabScreen();
    } else {
      captureManager->stopGrabScreen();
      _captureScreen->update();
    }
  });

  QHBoxLayout *comboBoxLayout = new QHBoxLayout();
  comboBoxLayout->addWidget(displayText);
  comboBoxLayout->addWidget(_displayComboBox);
  comboBoxLayout->addWidget(startButton);
  comboBoxLayout->addStretch();

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("NXScreen"));
  QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
  centerLayout->setContentsMargins(0, 0, 0, 0);
  centerLayout->addLayout(comboBoxLayout);
  centerLayout->addWidget(captureScreenArea);

  addCentralWidget(centralWidget, false, true);
#  endif
}

T_NXScreen::~T_NXScreen()
{
#  if defined(Q_OS_WIN) && defined(BUILD_WITH_NXPACKETIO)
  _unInitThread(true);
  _unInitThread(false);
#  endif
}

#  if defined(Q_OS_WIN) && defined(BUILD_WITH_NXPACKETIO)
void T_NXScreen::_initSendThread(QString const &interfaceIP)
{
  _packetIOSendThread = new QThread(this);
  _packetSendIO       = new T_NXPacketIO();
  _packetSendIO->setInterfaceIP(interfaceIP);
  _packetSendIO->moveToThread(_packetIOSendThread);
  connect(_packetIOSendThread, &QThread::started, _packetSendIO, &T_NXPacketIO::handleGrabImage);
  _packetIOSendThread->start();
}

void T_NXScreen::_initRecvThread(QString const &interfaceIP)
{
  // 原则上一个程序最好只初始化一个XIO 但如果需要多个XIO 则控制初始化间隔 不能同时进行初始化
  QTimer::singleShot(1000, this, [=]() {
    _packetIORecvThread = new QThread(this);
    _packetRecvIO       = new T_NXPacketIO();
    _packetRecvIO->setInterfaceIP(interfaceIP);
    _packetRecvIO->moveToThread(_packetIORecvThread);
    connect(_packetIORecvThread, &QThread::started, _packetRecvIO, &T_NXPacketIO::handleImagePacket);
    _packetIORecvThread->start();
    connect(_packetRecvIO, &T_NXPacketIO::sendHandleResult, _recvScreen, &T_RecvScreen::onSendHandleResult);
  });
}

void T_NXScreen::_unInitThread(bool isSend)
{
  if (isSend) {
    if (!_packetSendIO || !_packetIOSendThread) {
      return;
    }
    _packetSendIO->setIsActive(false);
    _packetIOSendThread->quit();
    _packetIOSendThread->wait();
    delete _packetSendIO;
    delete _packetIOSendThread;
    _packetSendIO       = nullptr;
    _packetIOSendThread = nullptr;
  } else {
    if (!_packetRecvIO || !_packetIORecvThread) {
      return;
    }
    _packetRecvIO->setIsActive(false);
    _packetIORecvThread->quit();
    _packetIORecvThread->wait();
    delete _packetRecvIO;
    delete _packetIORecvThread;
    _packetRecvIO       = nullptr;
    _packetIORecvThread = nullptr;
  }
}
#  endif
#endif
