#ifndef NXACTIONCOMMANDER_H
#define NXACTIONCOMMANDER_H

#include <QObject>
#include <QVariantMap>

#include "LinnSingleton.h"
#include "NXDef.h"

class NX_EXPORT NXActionCommand : public QObject
{
  Q_OBJECT
  // 命令描述
  Q_PRIVATE_CREATE(QPS_SCREF(QString), CommandDesc)
  // 弹出消息窗口
  Q_PRIVATE_CREATE(QWidget *, MessageWidget)
  // 弹出消息类型
  Q_PRIVATE_CREATE(NXMessageBarType::MessageMode, MessageMode)
  // 是否弹出消息
  Q_PRIVATE_CREATE(bool, IsShowMessage)

public:
  explicit NXActionCommand(QObject *parent = nullptr);
  ~NXActionCommand();

  virtual void undo() = 0;
  virtual void redo() = 0;
};

class NXActionCommanderPrivate;

class NX_EXPORT NXActionCommander : public QObject
{
  Q_OBJECT
  Q_Q_CREATE(NXActionCommander)
  Q_SINGLETON_CREATE(QSS_UNIQUE(NXActionCommander))
  // 当前命令域 只影响Design显示
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), CurrentDomain)
  // 是否启用命令消息提示
  Q_PROPERTY_CREATE_H(int, IsMessageDisplayEnable)
  // 命令描述显示时间 单位 ms
  Q_PROPERTY_CREATE_H(int, MessageDisplayMesc)
  // 最大命令容量
  Q_PROPERTY_CREATE_H(int, MaxCommandCount)
  // 运行至节点时是否在每一次运行间启用事件循环 设置为true时运行较慢 但会同步界面 设置为false时速度快 默认为true
  Q_PROPERTY_CREATE_H(bool, IsRunToCommandProcessEventEnable)

private:
  explicit NXActionCommander(QObject *parent = nullptr);
  ~NXActionCommander();
  Q_SIGNAL void commanderStateChanged(QString const &domainName, NXActionCommanderType::CommanderState state);

public:
  bool executeCommand(QString const &domainName, NXActionCommand *command, bool isInvoke = false);
  bool recordCommand(QString const &domainName, NXActionCommand *command, bool isRedo = true, bool isInvoke = false);
  void clearCommand(QString const &domainName);
  void undoCommand(QString const &domainName);
  void redoCommand(QString const &domainName);

  void runToCommand(QString const &domainName, int commandIndex);

  NXActionCommanderType::CommanderState getCommanderUndoState(QString const &domainName) const;
  NXActionCommanderType::CommanderState getCommanderRedoState(QString const &domainName) const;

  QWidget *getCommanderDesign() const;
};

#endif // NXACTIONCOMMANDER_H
