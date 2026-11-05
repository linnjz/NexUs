#include "T_TreeItem.h"

#include <QUuid>

T_TreeItem::T_TreeItem(QString const &itemTitle, T_TreeItem *parent)
    : QObject { parent }
{
  _itemKey     = QUuid::createUuid()
                     .toString()
                     .remove(QStringLiteral("{"))
                     .remove(QStringLiteral("}"))
                     .remove(QStringLiteral("-"));
  _itemTitle   = itemTitle;
  m_ParentItem = parent;
  m_IsChecked  = false;
}

T_TreeItem::~T_TreeItem()
{
  qDeleteAll(m_ChildrenItems);
}

QString
T_TreeItem::getItemKey() const
{
  return _itemKey;
}

QString
T_TreeItem::getItemTitle() const
{
  return _itemTitle;
}

void T_TreeItem::setChildChecked(bool isChecked)
{
  if (isChecked) {
    for (auto node : m_ChildrenItems) {
      node->setIsChecked(isChecked);
      node->setChildChecked(isChecked);
    }
  } else {
    for (auto node : m_ChildrenItems) {
      node->setChildChecked(isChecked);
      node->setIsChecked(isChecked);
    }
  }
}

Qt::CheckState
T_TreeItem::getChildCheckState()
{
  bool isAllChecked = true;
  bool isAnyChecked = false;
  for (auto node : m_ChildrenItems) {
    if (node->getIsChecked()) {
      isAnyChecked = true;
    } else {
      isAllChecked = false;
    }
    Qt::CheckState childState = node->getChildCheckState();
    if (childState == Qt::PartiallyChecked) {
      isAllChecked = false;
      isAnyChecked = true;
      break;
    } else if (childState == Qt::Unchecked) {
      isAllChecked = false;
    }
  }
  if (m_ChildrenItems.count() > 0) {
    if (isAllChecked) {
      return Qt::Checked;
    }
    if (isAnyChecked) {
      return Qt::PartiallyChecked;
    }
    return Qt::Unchecked;
  }
  return Qt::Checked;
}

void T_TreeItem::appendChildItem(T_TreeItem *childItem)
{
  m_ChildrenItems.append(childItem);
}

bool T_TreeItem::getIsHasChild() const
{
  if (m_ChildrenItems.count() > 0) {
    return true;
  }
  return false;
}

int T_TreeItem::getRow() const
{
  if (m_ParentItem) {
    return m_ParentItem->getChildrenItems().indexOf(const_cast<T_TreeItem *>(this));
  }
  return 0;
}
