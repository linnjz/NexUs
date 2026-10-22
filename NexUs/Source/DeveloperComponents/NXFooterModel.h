#ifndef NXFOOTERMODEL_H
#define NXFOOTERMODEL_H

#include <QAbstractListModel>

#include "NXDef.h"
class NXNavigationNode;

class NXFooterModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PRIVATE_CREATE(NXNavigationNode *, SelectedNode)

public:
  explicit NXFooterModel(QObject *parent = nullptr);
  ~NXFooterModel();
  NXNodeResultExpected addFooterNode(const QString &footerTitle, bool isHasFooterPage, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);
  int getFooterNodeCount() const;
  NXNavigationNode *getNavigationNode(const QString &footerKey) const;
  void removeNavigationNode(const QString &footerKey);

protected:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;

private:
  QList<NXNavigationNode *> _footerNodeList;
};

#endif // NXFOOTERMODEL_H
