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
  NXNodeResultExpected addFooterNode(QString const &footerTitle, bool isHasFooterPage, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);
  int getFooterNodeCount() const;
  NXNavigationNode *getNavigationNode(QString const &footerKey) const;
  void removeNavigationNode(QString const &footerKey);

protected:
  int rowCount(QModelIndex const &parent = QModelIndex()) const override;
  QVariant data(QModelIndex const &index, int role) const override;

private:
  QList<NXNavigationNode *> m_FooterNodeList;
};

#endif // NXFOOTERMODEL_H
