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
  m_IsShowMessage = true;
  m_MessageMode   = NXMessageBarType::Success;
  m_MessageWidget = nullptr;
}

NXActionCommand::~NXActionCommand()
{
}

Q_PROPERTY_CREATE_CPP(NXActionCommander, QPS_PAEMIT(int), IsMessageDisplayEnable)
Q_PROPERTY_CREATE_CPP(NXActionCommander, QPS_PAEMIT(int), MessageDisplayMesc)
Q_PROPERTY_CREATE_CPP(NXActionCommander, QPS_PAEMIT(int), MaxCommandCount)
Q_PROPERTY_CREATE_CPP(NXActionCommander, QPS_PAEMIT(bool), IsRunToCommandProcessEventEnable)

NXActionCommander::NXActionCommander(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXActionCommanderPrivate())
{
  Q_D(NXActionCommander);
  d->q_ptr                              = this;
  d->m_IsMessageDisplayEnable           = true;
  d->m_MessageDisplayMesc               = 5000;
  d->m_MaxCommandCount                  = 500;
  d->m_IsRunToCommandProcessEventEnable = true;

  d->m_CommanderDesign = new QWidget();
  d->m_CommanderDesign->setMinimumWidth(400);
  d->m_CommanderDesign->setObjectName("NXCommanderDesign");
  d->m_CommanderDesign->setStyleSheet(QStringLiteral("#NXCommanderDesign{background-color:transparent;}"));
  d->m_CommanderDesign->resize(400, 1000);

  d->m_CommanderView = new NXActionCommanderView(d->m_CommanderDesign);
  connect(d->m_CommanderView, &NXActionCommanderView::runToCommand, this, [=](QString const &domainName, int commandIndex) {
    runToCommand(domainName, commandIndex);
  });

  // 配置界面
  auto settingLayout = new QVBoxLayout();
  settingLayout->setSpacing(15);
  settingLayout->setContentsMargins(15, 0, 15, 15);
  auto settingTextLayout = new QHBoxLayout();
  auto settingIconText   = new NXText(d->m_CommanderDesign);
  settingIconText->setFixedSize(25, 25);
  settingIconText->setNXIcon(NXIconType::GearComplex);
  settingIconText->setTextPixelSize(17);
  auto settingText = new NXText(QStringLiteral("命令管理器配置"), 17, d->m_CommanderDesign);
  settingText->setFixedWidth(130);
  d->m_CommanderBusyRing = new NXProgressRing(d->m_CommanderDesign);
  d->m_CommanderBusyRing->setIsDisplayValue(false);
  d->m_CommanderBusyRing->setBusyingWidth(4);
  d->m_CommanderBusyRing->setFixedSize(25, 25);
  d->m_CommanderBusyRing->setIsBusying(true);

  d->m_CommanderBusyRingValue = new NXText(QStringLiteral("100.0%"), 17, d->m_CommanderDesign);
  d->m_CommanderBusyRingValue->setFixedWidth(65);
  settingTextLayout->addWidget(settingIconText);
  settingTextLayout->addWidget(settingText);
  settingTextLayout->addSpacing(80);
  settingTextLayout->addWidget(d->m_CommanderBusyRing);
  settingTextLayout->addWidget(d->m_CommanderBusyRingValue);
  settingTextLayout->addStretch();
  settingLayout->addLayout(settingTextLayout);

  int textWidth           = 105;
  auto firstSettingLayout = new QHBoxLayout();
  settingLayout->addLayout(firstSettingLayout);
  auto messageDisplayText = new NXText(QStringLiteral("启用命令提示"), 17, d->m_CommanderDesign);
  messageDisplayText->setFixedWidth(textWidth);
  auto messageDisplayCheckBox = new NXCheckBox(d->m_CommanderDesign);
  messageDisplayCheckBox->setChecked(true);
  connect(messageDisplayCheckBox, &NXCheckBox::stateChanged, this, [=](int state) {
    setIsMessageDisplayEnable(state);
  });
  firstSettingLayout->addWidget(messageDisplayText);
  firstSettingLayout->addWidget(messageDisplayCheckBox);
  firstSettingLayout->addSpacing(70);

  auto processEventEnableText = new NXText(QStringLiteral("启用界面同步"), 17, d->m_CommanderDesign);
  processEventEnableText->setFixedWidth(textWidth);
  auto processEventEnableCheckBox = new NXCheckBox(d->m_CommanderDesign);
  processEventEnableCheckBox->setChecked(true);
  connect(processEventEnableCheckBox, &NXCheckBox::stateChanged, this, [=](int state) {
    setIsRunToCommandProcessEventEnable(state);
  });
  firstSettingLayout->addWidget(processEventEnableText);
  firstSettingLayout->addWidget(processEventEnableCheckBox);
  firstSettingLayout->addStretch();

  auto secondSettingLayout = new QHBoxLayout();
  settingLayout->addLayout(secondSettingLayout);
  auto messageDisplayTimeText = new NXText(QStringLiteral("提示持续时间"), 17, d->m_CommanderDesign);
  messageDisplayTimeText->setFixedWidth(textWidth);
  auto messageDisplayTimeSpinBox = new NXSpinBox(d->m_CommanderDesign);
  messageDisplayTimeSpinBox->setMinimumWidth(200);
  messageDisplayTimeSpinBox->setRange(0, 100000);
  messageDisplayTimeSpinBox->setValue(d->m_MessageDisplayMesc);
  connect(messageDisplayTimeSpinBox, QOverload<int>::of(&NXSpinBox::valueChanged), this, [=](int value) {
    setMessageDisplayMesc(value);
  });
  auto messageDisplayTimeEndText = new NXText(QStringLiteral("毫秒"), 17, d->m_CommanderDesign);
  secondSettingLayout->addWidget(messageDisplayTimeText);
  secondSettingLayout->addWidget(messageDisplayTimeSpinBox);
  secondSettingLayout->addWidget(messageDisplayTimeEndText);
  secondSettingLayout->addStretch();

  auto thirdSettingLayout = new QHBoxLayout();
  settingLayout->addLayout(thirdSettingLayout);
  auto maxCommandCountText = new NXText(QStringLiteral("最大命令容量"), 17, d->m_CommanderDesign);
  maxCommandCountText->setFixedWidth(textWidth);
  auto maxCommandCountSpinBox = new NXSpinBox(d->m_CommanderDesign);
  maxCommandCountSpinBox->setMinimumWidth(200);
  maxCommandCountSpinBox->setRange(0, 100000);
  maxCommandCountSpinBox->setValue(d->m_MaxCommandCount);
  connect(maxCommandCountSpinBox, QOverload<int>::of(&NXSpinBox::valueChanged), this, [=](int value) {
    setMaxCommandCount(value);
  });
  auto maxCommandCountEndText = new NXText(QStringLiteral("条"), 17, d->m_CommanderDesign);
  thirdSettingLayout->addWidget(maxCommandCountText);
  thirdSettingLayout->addWidget(maxCommandCountSpinBox);
  thirdSettingLayout->addWidget(maxCommandCountEndText);
  thirdSettingLayout->addStretch();

  auto designLayout = new QVBoxLayout(d->m_CommanderDesign);
  designLayout->setContentsMargins(0, 0, 0, 0);
  designLayout->addWidget(d->m_CommanderView);
  designLayout->addLayout(settingLayout);
  d->m_CommanderDesign->setVisible(false);
}

NXActionCommander::~NXActionCommander()
{
}

void NXActionCommander::setCurrentDomain(QString const &domainName)
{
  Q_D(NXActionCommander);
  auto &commandData = d->m_CommandDomainMap[domainName];
  d->_initDomain(commandData);
  if (d->m_CommanderView->getDomainName() != domainName) {
    d->m_CommanderView->setDomainName(domainName);
    d->m_CommanderView->setModel(commandData.commanderModel);
  }
}

QString NXActionCommander::getCurrentDomain() const noexcept(std::is_nothrow_copy_constructible_v<QString>)
{
  Q_CONST_D(NXActionCommander);
  return d->m_CommanderView->getDomainName();
}

bool NXActionCommander::executeCommand(QString const &domainName, NXActionCommand *command, bool isInvoke)
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

bool NXActionCommander::recordCommand(QString const &domainName, NXActionCommand *command, bool isRedo, bool isInvoke)
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

void NXActionCommander::clearCommand(QString const &domainName)
{
  Q_D(NXActionCommander);
  if (!d->m_CommandDomainMap.contains(domainName)) {
    return;
  }
  auto &commandData        = d->m_CommandDomainMap[domainName];
  auto &commandList        = commandData.commandList;
  commandData.currentIndex = -1;
  for (auto const command : commandList) {
    delete command;
  }
  commandList.clear();
  commandData.undoState = NXActionCommanderType::UndoInvalid;
  commandData.redoState = NXActionCommanderType::RedoInvalid;
  Q_EMIT commanderStateChanged(domainName, NXActionCommanderType::UndoInvalid);
  Q_EMIT commanderStateChanged(domainName, NXActionCommanderType::RedoInvalid);
}

void NXActionCommander::undoCommand(QString const &domainName)
{
  Q_D(NXActionCommander);
  auto &commandData = d->m_CommandDomainMap[domainName];
  if (commandData.isBusyMode) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("任务管理器繁忙中!"), d->m_MessageDisplayMesc, nullptr);
    return;
  }
  if (!d->m_IsCommanderValid) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("当前不处于可编辑模式，仅支持运行命令!"), d->m_MessageDisplayMesc, nullptr);
    return;
  }
  setCurrentDomain(domainName);
  d->_undoCommand(domainName, true);
}

void NXActionCommander::redoCommand(QString const &domainName)
{
  Q_D(NXActionCommander);
  auto &commandData = d->m_CommandDomainMap[domainName];
  if (commandData.isBusyMode) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("任务管理器繁忙中!"), d->m_MessageDisplayMesc, nullptr);
    return;
  }
  if (!d->m_IsCommanderValid) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("当前不处于可编辑模式，仅支持运行命令!"), d->m_MessageDisplayMesc, nullptr);
    return;
  }
  setCurrentDomain(domainName);
  d->_redoCommand(domainName, true);
}

void NXActionCommander::runToCommand(QString const &domainName, int commandIndex)
{
  Q_D(NXActionCommander);
  auto &commandData = d->m_CommandDomainMap[domainName];
  auto &commandList = commandData.commandList;
  if (commandData.isBusyMode || commandIndex >= commandList.count()) {
    NXMessageBar::warning(NXMessageBarType::TopRight, QStringLiteral("警告"), QStringLiteral("任务管理器繁忙中!"), d->m_MessageDisplayMesc, nullptr);
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
      d->m_CommanderBusyRingValue->setText(QStringLiteral("%1%").arg(percent));
      if (d->m_IsRunToCommandProcessEventEnable) {
        QApplication::processEvents();
      }
    }
  } else {
    isRedo = false;
    // 撤销到目标索引 m_CurrentIndex最小为-1
    int runCount = commandData.currentIndex - commandIndex + 1;
    for (int i = 0; i < runCount; i++) {
      d->_undoCommand(domainName, false);
      QString percent = QString::number(100 * (i + 1) / static_cast<qreal>(runCount), 'f', 1);
      d->m_CommanderBusyRingValue->setText(QStringLiteral("%1%").arg(percent));
      if (d->m_IsRunToCommandProcessEventEnable) {
        QApplication::processEvents();
      }
    }
  }
  if (isRedo) {
    NXMessageBar::information(NXMessageBarType::TopRight, QStringLiteral("重做"), QStringLiteral("重做到指定节点完成!"), d->m_MessageDisplayMesc, nullptr);
  } else {
    NXMessageBar::information(NXMessageBarType::TopRight, QStringLiteral("撤销"), QStringLiteral("撤销到指定节点完成!"), d->m_MessageDisplayMesc, nullptr);
  }
  commandData.isBusyMode = false;
}

NXActionCommanderType::CommanderState NXActionCommander::getCommanderUndoState(QString const &domainName) const
{
  Q_CONST_D(NXActionCommander);
  if (!d->m_CommandDomainMap.contains(domainName)) {
    return NXActionCommanderType::UndoInvalid;
  }
  return d->m_CommandDomainMap[domainName].undoState;
}

NXActionCommanderType::CommanderState NXActionCommander::getCommanderRedoState(QString const &domainName) const
{
  Q_CONST_D(NXActionCommander);
  if (!d->m_CommandDomainMap.contains(domainName)) {
    return NXActionCommanderType::RedoInvalid;
  }
  return d->m_CommandDomainMap[domainName].redoState;
}

QWidget *NXActionCommander::getCommanderDesign() const
{
  Q_CONST_D(NXActionCommander);
  return d->m_CommanderDesign;
}
