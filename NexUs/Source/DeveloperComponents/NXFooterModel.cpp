#include "NXFooterModel.h"

#include <QUuid>

#include "NXNavigationNode.h"

NXFooterModel::NXFooterModel(QObject *parent)
    : QAbstractListModel { parent }
    , m_SelectedNode { nullptr }
{
}

NXFooterModel::~NXFooterModel()
{
  qDeleteAll(m_FooterNodeList);
}

int NXFooterModel::rowCount(QModelIndex const &parent) const
{
  return m_FooterNodeList.count();
}

QVariant NXFooterModel::data(QModelIndex const &index, int role) const
{
  if (role == Qt::UserRole) {
    if (index.row() < m_FooterNodeList.count()) {
      return QVariant::fromValue(m_FooterNodeList[index.row()]);
    }
  }
  return QVariant();
}

NXNodeResultExpected NXFooterModel::addFooterNode(QString const &footerTitle, bool isHasFooterPage, int keyPoints, NXIconType::IconName awesome)
{
  if (m_FooterNodeList.count() >= 3) {
    return NXNodeResultUnexpected { NXNavigationType::FooterUpperLimit };
  }
  NXNavigationNode *node = new NXNavigationNode(footerTitle);
  node->setKeyPoints(keyPoints);
  node->setIsFooterNode(true);
  node->setIsHasFooterPage(isHasFooterPage);
  node->setAwesome(awesome);
  beginResetModel();
  m_FooterNodeList.append(node);
  endResetModel();
  node->setModelIndex(this->index(m_FooterNodeList.count() - 1));
  return node->getNodeKey();
}

int NXFooterModel::getFooterNodeCount() const
{
  return m_FooterNodeList.count();
}

NXNavigationNode *NXFooterModel::getNavigationNode(QString const &footerKey) const
{
  for (auto node : m_FooterNodeList) {
    if (node->getNodeKey() == footerKey) {
      return node;
    }
  }
  return nullptr;
}

void NXFooterModel::removeNavigationNode(QString const &footerKey)
{
  for (auto node : m_FooterNodeList) {
    if (node->getNodeKey() == footerKey) {
      m_FooterNodeList.removeOne(node);
      break;
    }
  }
}
