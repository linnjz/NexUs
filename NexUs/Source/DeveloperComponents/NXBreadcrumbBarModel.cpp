#include "NXBreadcrumbBarModel.h"

NXBreadcrumbBarModel::NXBreadcrumbBarModel(QObject *parent)
    : QAbstractListModel { parent }
    , m_BreadcrumbList{}
{
}

NXBreadcrumbBarModel::~NXBreadcrumbBarModel()
{
}

int NXBreadcrumbBarModel::rowCount(QModelIndex const &parent) const
{
  if (!m_BreadcrumbList.isEmpty()) {
    return m_BreadcrumbList.count() * 2 - 1;
  }
  return 0;
}

QVariant NXBreadcrumbBarModel::data(QModelIndex const &index, int role) const
{
  if (role == Qt::DisplayRole) {
    if (index.row() % 2 == 0) {
      return m_BreadcrumbList[index.row() / 2];
    } else {
      return QStringLiteral(">");
    }
  } else if (role == Qt::UserRole) {
    if (index.row() == m_BreadcrumbList.count() * 2 - 2 || index.row() == m_BreadcrumbList.count() * 2 - 3) {
      return QString(QStringLiteral("LastBreadcrumb"));
    }
  }
  return QVariant();
}

void NXBreadcrumbBarModel::appendBreadcrumb(QString const &breadcrumb)
{
  if (!breadcrumb.isEmpty()) {
    if (m_BreadcrumbList.count() && m_BreadcrumbList.last() == breadcrumb) {
      return;
    }
    if (m_BreadcrumbList.count() >= 2 && m_BreadcrumbList[m_BreadcrumbList.count() - 2] == breadcrumb) {
      beginResetModel();
      m_BreadcrumbList.removeLast();
      endResetModel();
      return;
    }
    beginResetModel();
    m_BreadcrumbList.append(breadcrumb);
    endResetModel();
  }
}

void NXBreadcrumbBarModel::removeBreadcrumb(QString const &breadcrumb)
{
  if (m_BreadcrumbList.contains(breadcrumb)) {
    beginResetModel();
    m_BreadcrumbList.removeAt(m_BreadcrumbList.lastIndexOf(breadcrumb));
    endResetModel();
  }
}

void NXBreadcrumbBarModel::removeBreadcrumb(int index)
{
  if (index >= m_BreadcrumbList.count()) {
    return;
  }
  beginResetModel();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  m_BreadcrumbList.remove(index, m_BreadcrumbList.count() - index);
#else
  for (int i = m_BreadcrumbList.count() - 1; i >= 0; i--) {
    if (i < index) {
      break;
    }
    m_BreadcrumbList.removeAt(i);
  }
#endif
  endResetModel();
}

void NXBreadcrumbBarModel::setBreadcrumbList(QStringList const &breadcrumbList)
{
  beginResetModel();
  this->m_BreadcrumbList = breadcrumbList;
  endResetModel();
}

int NXBreadcrumbBarModel::getBreadcrumbListCount() const
{
  return m_BreadcrumbList.count();
}

QStringList NXBreadcrumbBarModel::getBreadcrumbList() const
{
  return m_BreadcrumbList;
}
