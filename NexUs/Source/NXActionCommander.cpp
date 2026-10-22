#include "NXActionCommander.h"

#include "DeveloperComponents/NXActionCommanderModel.h"
#include "DeveloperComponents/NXActionCommanderView.h"
#include "NXCheckBox.h"
#include "NXMessageBar.h"
#include "NXProgressRing.h"
#include "NXSpinBox.h"
#include "NXText.h"
#include "private/NXActionCommanderPrivate.h"

#include <QApplication>
#include <QVBoxLayout>

NXActionCommand::NXActionCommand(QObject *parent)
    : QObject(parent)
{
  _pIsShowMessage = true;
  _pMessageMode   = NXMessageBarType::Success;
  _pMessageWidget = nullptr;
}

NXActionCommand::~NXActionCommand()
{
}

Q_PROPERTY_CREATE_CPP(NXActionCommander, int, IsMessageDisplayEnable)
Q_PROPERTY_CREATE_CPP(NXActionCommander, int, MessageDisplayMesc)
Q_PROPERTY_CREATE_CPP(NXActionCommander, int, MaxCommandCount)
Q_PROPERTY_CREATE_CPP(NXActionCommander, bool, IsRunToCommandProcessEventEnable)

NXActionCommander::NXActionCommander(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXActionCommanderPrivate())
{
  Q_D(NXActionCommander);
  d->q_ptr                              = this;
  d->_pIsMessageDisplayEnable           = true;
  d->_pMessageDisplayMesc               = 5000;
  d->_pMaxCommandCount                  = 500;
  d->_pIsRunToCommandProcessEventEnable = true;

  d->_commanderDesign = new QWidget();
  d->_commanderDesign->setMinimumWidth(400);
  d->_commanderDesign->setObjectName("NXCommanderDesign");
  d->_commanderDesign->setStyleSheet(QStringLiteral("#NXCommanderDesign{background-color:transparent;}"));
  d->_commanderDesign->resize(400, 1000);

  d->_commanderView = new NXActionCommanderView(d->_commanderDesign);
  connect(d->_commanderView, &NXActionCommanderView::runToCommand, this, [=](const QString &domainName, int commandIndex) {
    runToCommand(domainName, commandIndex);
  });

  // 配置界面
  auto settingLayout = new QVBoxLayout();
  settingLayout->setSpacing(15);
  settingLayout->setContentsMargins(15, 0, 15, 15);
  auto settingTextLayout = new QHBoxLayout();
  auto settingIconText   = new NXText(d->_commanderDesign);
  settingIconText->setFixedSize(25, 25);
  settingIconText->setNXIcon(NXIconType::GearComplex);
  settingIconText->setTextPixelSize(17);
  auto settingText = new NXText(QStringLiteral("命令管理器配置"), 17, d->_commanderDesign);
  settingText->setFixedWidth(130);
  d->_commanderBusyRing = new NXProgressRing(d->_commanderDesign);
  d->_commanderBusyRing->setIsDisplayValue(false);
  d->_commanderBusyRing->setBusyingWidth(4);
  d->_commanderBusyRing->setFixedSize(25, 25);
  d->_commanderBusyRing->setIsBusying(true);

  d->_commanderBusyRingValue = new NXText(QStringLiteral("100.0%"), 17, d->_commanderDesign);
  d->_commanderBusyRingValue->setFixedWidth(65);
  settingTextLayout->addWidget(settingIconText);
  settingTextLayout->addWidget(settingText);
  settingTextLayout->addSpacing(80);
  settingTextLayout->addWidget(d->_commanderBusyRing);
  settingTextLayout->addWidget(d->_commanderBusyRingValue);
  settingTextLayout->addStretch();
  settingLayout->addLayout(settingTextLayout);

  int textWidth           = 105;
  auto firstSettingLayout = new QHBoxLayout();
  settingLayout->addLayout(firstSettingLayout);
  auto messageDisplayText = new NXText(QStringLiteral("启用命令提示"), 17, d->_commanderDesign);
  messageDisplayText->setFixedWidth(textWidth);
  auto messageDisplayCheckBox = new NXCheckBox(d->_commanderDesign);
  messageDisplayCheckBox->setChecked(true);
  connect(messageDisplayCheckBox, &NXCheckBox::stateChanged, this, [=](int state) {
    setIsMessageDisplayEnable(state);
  });
  firstSettingLayout->addWidget(messageDisplayText);
  firstSettingLayout->addWidget(messageDisplayCheckBox);
  firstSettingLayout->addSpacing(70);

  auto processEventEnableText = new NXText(QStringLiteral("启用界面同步"), 17, d->_commanderDesign);
  processEventEnableText->setFixedWidth(textWidth);
  auto processEventEnableCheckBox = new NXCheckBox(d->_commanderDesign);
  processEventEnableCheckBox->setChecked(true);
  connect(processEventEnableCheckBox, &NXCheckBox::stateChanged, this, [=](int state) {
    setIsRunToCommandProcessEventEnable(state);
  });
  firstSettingLayout->addWidget(processEventEnableText);
  firstSettingLayout->addWidget(processEventEnableCheckBox);
  firstSettingLayout->addStretch();

  auto secondSettingLayout = new QHBoxLayout();
  settingLayout->addLayout(secondSettingLayout);
  auto messageDisplayTimeText = new NXText(QStringLiteral("提示持续时间"), 17, d->_commanderDesign);
  messageDisplayTimeText->setFixedWidth(textWidth);
  auto messageDisplayTimeSpinBox = new NXSpinBox(d->_commanderDesign);
  messageDisplayTimeSpinBox->setMinimumWidth(200);
  messageDisplayTimeSpinBox->setRange(0, 100000);
  messageDisplayTimeSpinBox->setValue(d->_pMessageDisplayMesc);
  connect(messageDisplayTimeSpinBox, QOverload<int>::of(&NXSpinBox::valueChanged), this, [=](int value) {
    setMessageDisplayMesc(value);
  });
  auto messageDisplayTimeEndText = new NXText(QStringLiteral("毫秒"), 17, d->_commanderDesign);
  secondSettingLayout->addWidget(messageDisplayTimeText);
  secondSettingLayout->addWidget(messageDisplayTimeSpinBox);
  secondSettingLayout->addWidget(messageDisplayTimeEndText);
  secondSettingLayout->addStretch();

  auto thirdSettingLayout = new QHBoxLayout();
  settingLayout->addLayout(thirdSettingLayout);
  auto maxCommandCountText = new NXText(QStringLiteral("最大命令容量"), 17, d->_commanderDesign);
  maxCommandCountText->setFixedWidth(textWidth);
  auto maxCommandCountSpinBox = new NXSpinBox(d->_commanderDesign);
  maxCommandCountSpinBox->setMinimumWidth(200);
  maxCommandCountSpinBox->setRange(0, 100000);
  maxCommandCountSpinBox->setValue(d->_pMaxCommandCount);
  connect(maxCommandCountSpinBox, QOverload<int>::of(&NXSpinBox::valueChanged), this, [=](int value) {
    setMaxCommandCount(value);
  });
  auto maxCommandCountEndText = new NXText(QStringLiteral("条"), 17, d->_commanderDesign);
  thirdSettingLayout->addWidget(maxCommandCountText);
  thirdSettingLayout->addWidget(maxCommandCountSpinBox);
  thirdSettingLayout->addWidget(maxCommandCountEndText);
  thirdSettingLayout->addStretch();

  auto designLayout = new QVBoxLayout(d->_commanderDesign);
  designLayout->setContentsMargins(0, 0, 0, 0);
  designLayout->addWidget(d->_commanderView);
  designLayout->addLayout(settingLayout);
  d->_commanderDesign->setVisible(false);
}

NXActionCommander::~NXActionCommander()
{
}

void NXActionCommander::setCurrentDomain(const QString &domainName)
{
  Q_D(NXActionCommander);
  auto &commandData = d->_commandDomainMap[domainName];
  d->_initDomain(commandData);
  if (d->_commanderView->getDomainName() != domainName) {
    d->_commanderView->setDomainName(domainName);
    d->_commanderView->setModel(commandData.commanderModel);
  }
}

QString NXActionCommander::getCurrentDomain() const
{
  Q_D(const NXActionCommander);
  return d->_commanderView->getDomainName();
}

bool NXActionCommander::executeCommand(const QString &domainName, NXActionCommand *command, bool isInvoke)
{
  Q_D(NXActionCommander);
  if (isInvoke) {
    bool result = false;
    QMetaObject::invokeMethod(this, [&]() {
      result = d->_executeCommand(domainName, command);
    }, Qt::BlockingQueuedConnection);
    return result;
  }
  return d->_executeCommand(domainName, command);
}

bool NXActionCommander::recordCommand(const QString &domainName, NXActionCommand *command, bool isRedo, bool isInvoke)
{
  Q_D(NXActionCommander);
  if (isInvoke) {
    bool result = false;
    QMetaObject::invokeMethod(this, [&]() {
      result = d->_recordCommand(domainName, command, isRedo);
    }, Qt::BlockingQueuedConnection);
    return result;
  }
  return d->_recordCommand(domainName, command, isRedo);
}

void NXActionCommander::clearCommand(const QString &domainName)
{
  Q_D(NXActionCommander);
  if (!d->_commandDomainMap.contains(domainName)) {
    return;
  }
  auto &commandData        = d->_commandDomainMap[domainName];
  auto &commandList        = commandData.commandList;
  commandData.currentIndex = -1;
  for (const auto command : commandList) {
    delete command;
  }
  commandList.clear();
  commandData.undoState = NXActionCommanderType::UndoInvalid;
  commandData.redoState = NXActionCommanderType::RedoInvalid;
  Q_EMIT commanderStateChanged(domainName, NXActionCommanderType::UndoInvalid);
  Q_EMIT commanderStateChanged(domainName, NXActionCommanderType::RedoInvalid);
}

void NXActionCommander::undoCommand(const QString &domainName)
{
  Q_D(NXActionCommander);
  auto &commandData = d->_commandDomainMap[domainName];
  if (commandData.isBusyMode) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("任务管理器繁忙中!"), d->_pMessageDisplayMesc, nullptr);
    return;
  }
  if (!d->_isCommanderValid) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("当前不处于可编辑模式，仅支持运行命令!"), d->_pMessageDisplayMesc, nullptr);
    return;
  }
  setCurrentDomain(domainName);
  d->_undoCommand(domainName, true);
}

void NXActionCommander::redoCommand(const QString &domainName)
{
  Q_D(NXActionCommander);
  auto &commandData = d->_commandDomainMap[domainName];
  if (commandData.isBusyMode) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("任务管理器繁忙中!"), d->_pMessageDisplayMesc, nullptr);
    return;
  }
  if (!d->_isCommanderValid) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("当前不处于可编辑模式，仅支持运行命令!"), d->_pMessageDisplayMesc, nullptr);
    return;
  }
  setCurrentDomain(domainName);
  d->_redoCommand(domainName, true);
}

void NXActionCommander::runToCommand(const QString &domainName, int commandIndex)
{
  Q_D(NXActionCommander);
  auto &commandData = d->_commandDomainMap[domainName];
  auto &commandList = commandData.commandList;
  if (commandData.isBusyMode || commandIndex >= commandList.count()) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("任务管理器繁忙中!"), d->_pMessageDisplayMesc, nullptr);
    return;
  }
  commandData.isBusyMode = true;
  bool isRedo            = true;
  if (commandIndex > commandData.currentIndex) {
    // 重做到目标索引
    int runCount = commandIndex - commandData.currentIndex;
    for (int i = 0; i < runCount; i++) {
      d->_redoCommand(domainName, false);
      QString percent = QString::number(100 * (i + 1) / static_cast<qreal>(runCount), 'f', 1);
      d->_commanderBusyRingValue->setText(QStringLiteral("%1%").arg(percent));
      if (d->_pIsRunToCommandProcessEventEnable) {
        QApplication::processEvents();
      }
    }
  } else {
    isRedo = false;
    // 撤销到目标索引 _currentIndex最小为-1
    int runCount = commandData.currentIndex - commandIndex + 1;
    for (int i = 0; i < runCount; i++) {
      d->_undoCommand(domainName, false);
      QString percent = QString::number(100 * (i + 1) / static_cast<qreal>(runCount), 'f', 1);
      d->_commanderBusyRingValue->setText(QStringLiteral("%1%").arg(percent));
      if (d->_pIsRunToCommandProcessEventEnable) {
        QApplication::processEvents();
      }
    }
  }
  if (isRedo) {
    NXMessageBar::information(NXMessageBarType::TopRight, QStringLiteral("重做"), QStringLiteral("重做到指定节点完成!"), d->_pMessageDisplayMesc, nullptr);
  } else {
    NXMessageBar::information(NXMessageBarType::TopRight, QStringLiteral("撤销"), QStringLiteral("撤销到指定节点完成!"), d->_pMessageDisplayMesc, nullptr);
  }
  commandData.isBusyMode = false;
}

NXActionCommanderType::CommanderState NXActionCommander::getCommanderUndoState(const QString &domainName) const
{
  Q_D(const NXActionCommander);
  if (!d->_commandDomainMap.contains(domainName)) {
    return NXActionCommanderType::UndoInvalid;
  }
  return d->_commandDomainMap[domainName].undoState;
}

NXActionCommanderType::CommanderState NXActionCommander::getCommanderRedoState(const QString &domainName) const
{
  Q_D(const NXActionCommander);
  if (!d->_commandDomainMap.contains(domainName)) {
    return NXActionCommanderType::RedoInvalid;
  }
  return d->_commandDomainMap[domainName].redoState;
}

QWidget * NXActionCommander::getCommanderDesign() const
{
  Q_D(const NXActionCommander);
  return d->_commanderDesign;
}
