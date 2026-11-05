#ifndef NXACTIONCOMMANDERPRIVATE_H
#define NXACTIONCOMMANDERPRIVATE_H
#include <QMap>
#include <QObject>
#include <QVector>

#include "NXDef.h"
class NXActionCommand;
class NXActionCommander;
class NXText;
class NXProgressRing;
class NXActionCommanderModel;
class NXActionCommanderView;

class NXActionCommanderPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXActionCommander)
  Q_PROPERTY_CREATE_D(int, IsMessageDisplayEnable)
  Q_PROPERTY_CREATE_D(int, MessageDisplayMesc)
  Q_PROPERTY_CREATE_D(int, MaxCommandCount)
  Q_PROPERTY_CREATE_D(bool, IsRunToCommandProcessEventEnable)

public:
  struct NXActionCommandDomainData
  {
    bool isBusyMode { false };
    int currentIndex { -1 };
    NXActionCommanderType::CommanderState undoState { NXActionCommanderType::UndoInvalid };
    NXActionCommanderType::CommanderState redoState { NXActionCommanderType::RedoInvalid };
    NXActionCommanderModel *commanderModel { nullptr };
    QVector<NXActionCommand *> commandList;
  };

  explicit NXActionCommanderPrivate(QObject *parent = nullptr);
  ~NXActionCommanderPrivate();

private:
  void _showCommandDesc(NXActionCommand const *command, bool isRedo, bool isExecute = false) const;
  bool _executeCommand(QString const &domainName, NXActionCommand *command);
  bool _recordCommand(QString const &domainName, NXActionCommand *command, bool isRedo);
  void _undoCommand(QString const &domainName, bool isShowMessage);
  void _redoCommand(QString const &domainName, bool isShowMessage);
  void _initDomain(NXActionCommandDomainData &domainData) const;
  bool m_IsCommanderValid { true }; // 是否处于可编辑模式
  NXProgressRing *m_CommanderBusyRing { nullptr };
  NXText *m_CommanderBusyRingValue { nullptr };
  QWidget *m_CommanderDesign { nullptr };
  NXActionCommanderView *m_CommanderView { nullptr };
  QMap<QString, NXActionCommandDomainData> m_CommandDomainMap;
};

#endif // NXACTIONCOMMANDERPRIVATE_H
