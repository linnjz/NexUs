#ifndef NXNAVIGATIONNODE_H
#define NXNAVIGATIONNODE_H

#include <QModelIndex>
#include <QObject>

#include "NXDef.h"

class NXNavigationNode : public QObject
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QPS_SCREF(QList<NXNavigationNode *>), ChildrenNodes)
  Q_PRIVATE_CREATE(QPS_SCREF(QModelIndex), ModelIndex)
  Q_PRIVATE_CREATE(QPS_SCREF(QString), NodeTitle)
  Q_PRIVATE_CREATE(NXNavigationNode *, ParentNode)
  Q_PRIVATE_CREATE(NXIconType::IconName, Awesome)
  Q_PRIVATE_CREATE(int, KeyPoints)
  Q_PRIVATE_CREATE(int, Depth)
  Q_PRIVATE_CREATE(bool, IsRootNode)
  Q_PRIVATE_CREATE(bool, IsFooterNode)
  Q_PRIVATE_CREATE(bool, IsHasFooterPage)
  Q_PRIVATE_CREATE(bool, IsExpanderNode)
  Q_PRIVATE_CREATE(bool, IsCategoryNode)
  Q_PRIVATE_CREATE(bool, IsVisible)

public:
  explicit NXNavigationNode(QString const &nodeTitle, NXNavigationNode *parent = nullptr);
  ~NXNavigationNode();

  QString getNodeKey() const;

  void setIsExpanded(bool isExpanded);
  bool getIsExpanded() const;

  void setChildVisible(bool isVisible);
  bool getIsHasChild() const;
  bool getIsHasPageChild() const;

  void appendChildNode(NXNavigationNode *childNode);
  void removeChildNode(NXNavigationNode *childNode);
  void insertChildNode(int row, NXNavigationNode *childNode);

  bool getIsChildHasKeyPoints() const;

  NXNavigationNode *getOriginalNode();
  bool getIsChildNode(NXNavigationNode *node);

  int getRow() const;
  int getRowExceptCategoryNodes() const;

  QList<NXNavigationNode *> getExceptCategoryNodes();

private:
  bool m_IsExpanded { false };
  QString m_NodeKey = QStringLiteral("");
};

#endif // NXNAVIGATIONNODE_H
