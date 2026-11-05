#ifndef NXTREESELECTPRIVATE_H
#define NXTREESELECTPRIVATE_H

#include <QModelIndex>
#include <QObject>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

#include "NXDef.h"

class QStandardItemModel;
class NXTreeSelect;
class NXTreeView;
class NXLineEdit;
class QSortFilterProxyModel;

class NXTreeSelectPopupWidget : public QWidget
{
  Q_OBJECT

public:
  NXTreeSelectPopupWidget(QWidget *parent = nullptr);
  NXThemeType::ThemeMode m_ThemeMode;
  int m_BorderRadius { 6 };

protected:
  void paintEvent(QPaintEvent *event) override;
};

class NXTreeSelectPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXTreeSelect)
  Q_PROPERTY_CREATE_D(QString, PlaceholderText)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, ItemHeight)
  Q_PROPERTY_CREATE_D(int, MaxVisibleItems)
  Q_PROPERTY_CREATE_D(bool, IsSearchVisible)
  Q_PROPERTY_CREATE_D(bool, IsEditable)

public:
  explicit NXTreeSelectPrivate(QObject *parent = nullptr);
  ~NXTreeSelectPrivate();

  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);
  Q_SLOT void onItemClicked(QModelIndex const &index);
  Q_SLOT void onSearchTextChanged(QString const &text);
  Q_SLOT void onTreeExpanded(QModelIndex const &index);
  Q_SLOT void onTreeCollapsed(QModelIndex const &index);

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  friend class NXTreeSelect;
  bool m_IsHover { false };
  bool m_IsPopupVisible { false };
  bool m_IsPressed { false };
  NXThemeType::ThemeMode m_ThemeMode;
  qreal m_ExpandIconRotate { 0 };
  QString m_CurrentText;
  QModelIndex m_CurrentIndex;
  QStandardItemModel *m_Model { nullptr };
  QSortFilterProxyModel *m_ProxyModel { nullptr };
  NXTreeView *m_TreeView { nullptr };
  NXLineEdit *m_SearchEdit { nullptr };
  NXTreeSelectPopupWidget *m_PopupContainer { nullptr };
  void _showPopup();
  void _hidePopup();
  void _updateDisplayText();
  void _updatePopupHeight();
  int _calculatePopupHeight() const;
  int _countVisibleItems(QModelIndex const &parent = QModelIndex()) const;
};

#endif // NXTREESELECTPRIVATE_H
