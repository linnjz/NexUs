#ifndef NXACTIONCOMMANDERMODEL_H
#define NXACTIONCOMMANDERMODEL_H

#include <QAbstractItemModel>
#include <QMap>
#include <QObject>

#include "NXActionCommander.h"

class NXActionCommanderNode;

class NXActionCommanderModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  explicit NXActionCommanderModel(QObject *parent = nullptr);
  ~NXActionCommanderModel() override;

  QModelIndex parent(QModelIndex const &child) const override;
  QModelIndex index(int row, int column, QModelIndex const &parent) const override;
  int rowCount(QModelIndex const &parent) const override;
  int columnCount(QModelIndex const &parent) const override;
  QVariant data(QModelIndex const &index, int role) const override;

  void addCommand(NXActionCommand *command);
  void removeCommand(NXActionCommand *command);
  void updateCommand(NXActionCommand *command, bool isRedo);
  void clearCommand();

  QModelIndex const &getCommandModelIndex(NXActionCommand *command);

private:
  NXActionCommanderNode *m_RootNode { nullptr };
  QMap<NXActionCommand *, NXActionCommanderNode *> m_CommanderNodeMap;
  void _updateNodeModelIndex(QModelIndex const &parentIndex = {});
};

#endif //NXACTIONCOMMANDERMODEL_H
