#ifndef NXNAVIGATIONMODEL_H
#define NXNAVIGATIONMODEL_H

#include <QAbstractItemModel>
#include <QObject>

#include "NXDef.h"
class NXNavigationNode;

class NXNavigationModel : public QAbstractItemModel
{
  Q_OBJECT
  Q_PRIVATE_CREATE(NXNavigationNode *, SelectedNode)
  Q_PRIVATE_CREATE(NXNavigationNode *, SelectedExpandedNode)

public:
  explicit NXNavigationModel(QObject *parent = nullptr);
  ~NXNavigationModel();

  void setIsMaximalMode(bool isMaximal);
  bool getIsMaximalMode();

  QModelIndex parent(QModelIndex const &child) const override;
  QModelIndex index(int row, int column, QModelIndex const &parent = QModelIndex()) const override;
  int rowCount(QModelIndex const &parent = QModelIndex()) const override;
  int columnCount(QModelIndex const &parent = QModelIndex()) const override;
  QVariant data(QModelIndex const &index, int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(QModelIndex const &index) const override;

  QString addExpanderNode(QString const &expanderTitle, NXIconType::IconName awesome);
  NXNodeResultExpected
  addExpanderNode(QString const &expanderTitle, QString const &targetExpanderKey, NXIconType::IconName awesome);

  QString addPageNode(QString const &pageTitle, NXIconType::IconName awesome);
  NXNodeResultExpected
  addPageNode(QString const &pageTitle, QString const &targetExpanderKey, NXIconType::IconName awesome);
  QString addPageNode(QString const &pageTitle, int keyPoints, NXIconType::IconName awesome);
  NXNodeResultExpected
  addPageNode(QString const &pageTitle, QString const &targetExpanderKey, int keyPoints, NXIconType::IconName awesome);

  QString addCategoryNode(QString const &categoryTitle);
  NXNodeResultExpected addCategoryNode(QString const &categoryTitle, QString const &targetExpanderKey);

  QStringList removeNavigationNode(QString const &nodeKey);

  NXNavigationNode const *getRootNode() const;
  NXNavigationNode *getNavigationNode(QString const &nodeKey) const;
  QList<NXNavigationNode *> getRootExpanderNodes() const;
  QList<NXNavigationNode *> getRootExpandedNodes() const;

  QList<NXNavigationNode *> getRootCategoryNodes() const;

private:
  bool m_IsMaximalMode { true };
  QMap<QString, NXNavigationNode *> m_NodesMap;
  NXNavigationNode *m_RootNode { nullptr };
};

#endif // NXNAVIGATIONMODEL_H
