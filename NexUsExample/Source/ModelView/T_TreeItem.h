#ifndef T_TREEITEM_H
#define T_TREEITEM_H

#include <QModelIndex>
#include <QObject>

#include "NXProperty.h"

class T_TreeItem : public QObject
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_SCREF(QList<T_TreeItem *>), ChildrenItems)
  Q_PRIVATE_CREATE(T_TreeItem *, ParentItem)
  Q_PROPERTY_CREATE(bool, IsChecked)

public:
  explicit T_TreeItem(QString const &itemTitle, T_TreeItem *parent = nullptr);
  ~T_TreeItem();
  QString getItemKey() const;
  QString getItemTitle() const;

  void setChildChecked(bool isChecked);
  Qt::CheckState getChildCheckState();

  void appendChildItem(T_TreeItem *childItem);
  bool getIsHasChild() const;
  int getRow() const;

private:
  bool _isExpanded { false };
  QString _itemKey   = QStringLiteral("");
  QString _itemTitle = QStringLiteral("");
};

#endif // T_TREEITEM_H
