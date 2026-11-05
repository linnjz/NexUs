#include "T_IconModel.h"

#include "NXDef.h"

T_IconModel::T_IconModel(QObject *parent)
    : QAbstractListModel { parent }
{
  _metaEnum      = QMetaEnum::fromType<NXIconType::IconName>();
  _rowCount      = _metaEnum.keyCount() - 1;
  m_IsSearchMode = false;
}

T_IconModel::~T_IconModel()
{
}

int T_IconModel::rowCount(QModelIndex const &parent) const
{
  return _rowCount;
}

void T_IconModel::setSearchKeyList(QStringList const &list)
{
  beginResetModel();
  this->_searchKeyList = list;
  if (m_IsSearchMode) {
    _rowCount = this->getSearchKeyList().count();
  } else {
    _rowCount = _metaEnum.keyCount() - 1;
  }
  endResetModel();
}

QStringList
T_IconModel::getSearchKeyList()
{
  return this->_searchKeyList;
}

QVariant
T_IconModel::data(QModelIndex const &index, int role) const
{
  if (role == Qt::UserRole) {
    if (!m_IsSearchMode) {
      if (index.row() >= _metaEnum.keyCount() - 1) {
        return QVariant();
      }
      return QStringList { _metaEnum.key(index.row() + 1), QChar(_metaEnum.value(index.row() + 1)) };
    } else {
      QStringList iconList;
      if (index.row() >= _searchKeyList.count()) {
        return QVariant();
      }
      iconList.append(_searchKeyList.at(index.row()));
      iconList.append(QChar(_metaEnum.keyToValue(_searchKeyList.at(index.row()).toUtf8().constData())));
      return iconList;
    }
  }
  return QVariant();
}

QString
T_IconModel::getIconNameFromModelIndex(QModelIndex const &index) const
{
  QString iconName;
  if (m_IsSearchMode) {
    if (index.row() < _searchKeyList.count()) {
      iconName = QString(QStringLiteral("NXIconType::")) + _searchKeyList.at(index.row());
    }
  } else {
    if (index.row() < _metaEnum.keyCount() - 1) {
      iconName = QString(QStringLiteral("NXIconType::")) + _metaEnum.key(index.row() + 1);
    }
  }
  return iconName;
}
