#include "T_TreeViewModel.h"

#include <QIcon>

#include "T_TreeItem.h"

T_TreeViewModel::T_TreeViewModel(QObject *parent)
    : QAbstractItemModel { parent }
{
  _rootItem = new T_TreeItem(QStringLiteral("root"));
  for (int i = 0; i < 20; i++) {
    T_TreeItem *level1Item = new T_TreeItem(QString(QStringLiteral("Lv1--TreeItem%1")).arg(i + 1), _rootItem);
    for (int j = 0; j < 6; j++) {
      T_TreeItem *level2Item = new T_TreeItem(QString(QStringLiteral("Lv2--TreeItem%1")).arg(j + 1), level1Item);
      for (int k = 0; k < 6; k++) {
        T_TreeItem *level3Item = new T_TreeItem(QString(QStringLiteral("Lv3--TreeItem%1")).arg(k + 1), level2Item);
        for (int l = 0; l < 6; l++) {
          T_TreeItem *level4Item = new T_TreeItem(QString(QStringLiteral("Lv4--TreeItem%1")).arg(l + 1), level3Item);
          level3Item->appendChildItem(level4Item);
          _itemsMap.insert(level4Item->getItemKey(), level4Item);
        }
        level2Item->appendChildItem(level3Item);
        _itemsMap.insert(level3Item->getItemKey(), level3Item);
      }
      level1Item->appendChildItem(level2Item);
      _itemsMap.insert(level2Item->getItemKey(), level2Item);
    }
    _rootItem->appendChildItem(level1Item);
    _itemsMap.insert(level1Item->getItemKey(), level1Item);
  }
}

T_TreeViewModel::~T_TreeViewModel()
{
  delete _rootItem;
}

QModelIndex
T_TreeViewModel::parent(QModelIndex const &child) const
{
  if (!child.isValid()) {
    return QModelIndex();
  }
  T_TreeItem *childItem  = static_cast<T_TreeItem *>(child.internalPointer());
  T_TreeItem *parentItem = childItem->getParentItem();
  if (parentItem == _rootItem) {
    return QModelIndex();
  } else if (parentItem == nullptr) {
    return QModelIndex();
  }
  return createIndex(parentItem->getRow(), 0, parentItem);
}

QModelIndex
T_TreeViewModel::index(int row, int column, QModelIndex const &parent) const
{
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }
  T_TreeItem *parentItem;
  if (!parent.isValid()) {
    parentItem = _rootItem;
  } else {
    parentItem = static_cast<T_TreeItem *>(parent.internalPointer());
  }
  T_TreeItem *childItem = nullptr;
  if (parentItem->getChildrenItems().count() > row) {
    childItem = parentItem->getChildrenItems().at(row);
  }
  if (childItem) {
    return createIndex(row, column, childItem);
  }
  return QModelIndex();
}

int T_TreeViewModel::rowCount(QModelIndex const &parent) const
{
  T_TreeItem *parentItem;
  if (parent.column() > 0) {
    return 0;
  }
  if (!parent.isValid()) {
    parentItem = _rootItem;
  } else {
    parentItem = static_cast<T_TreeItem *>(parent.internalPointer());
  }
  return parentItem->getChildrenItems().count();
}

int T_TreeViewModel::columnCount(QModelIndex const &parent) const
{
  Q_UNUSED(parent)
  return 1;
}

QVariant
T_TreeViewModel::data(QModelIndex const &index, int role) const
{
  if (role == Qt::DisplayRole) {
    return static_cast<T_TreeItem *>(index.internalPointer())->getItemTitle();
  } else if (role == Qt::DecorationRole) {
    return QIcon(QStringLiteral(":/Resource/Image/Cirno.jpg"));
  } else if (role == Qt::CheckStateRole) {
    T_TreeItem *item = static_cast<T_TreeItem *>(index.internalPointer());
    if (item->getIsHasChild()) {
      return item->getChildCheckState();
    } else {
      return item->getIsChecked() ? Qt::Checked : Qt::Unchecked;
    }
    return Qt::Unchecked;
  }
  return QVariant();
}

bool T_TreeViewModel::setData(QModelIndex const &index, QVariant const &value, int role)
{
  if (role == Qt::CheckStateRole) {
    T_TreeItem *item = static_cast<T_TreeItem *>(index.internalPointer());
    item->setIsChecked(!item->getIsChecked());
    item->setChildChecked(item->getIsChecked());
    Q_EMIT dataChanged(QModelIndex(), QModelIndex(), { role });
    return true;
  }
  return QAbstractItemModel::setData(index, value, role);
}

Qt::ItemFlags
T_TreeViewModel::flags(QModelIndex const &index) const
{
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);
  flags |= Qt::ItemIsUserCheckable;
  return flags;
}

QVariant
T_TreeViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return QString(QStringLiteral("NXTreeView-Example-4Level"));
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

int T_TreeViewModel::getItemCount() const
{
  return this->_itemsMap.count();
}
