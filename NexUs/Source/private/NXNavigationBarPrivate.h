#ifndef NXNAVIGATIONBARPRIVATE_H
#define NXNAVIGATIONBARPRIVATE_H

#include <QMap>
#include <QObject>

#include "NXDef.h"
#include "NXSuggestBox.h"
class QLayout;
class NXMenu;
class QVBoxLayout;
class QHBoxLayout;
class QLinearGradient;

class NXNavigationBar;
class NXNavigationNode;
class NXNavigationModel;
class NXNavigationView;
class NXInteractiveCard;

class NXBaseListView;
class NXFooterModel;
class NXFooterDelegate;
class NXIconButton;
class NXToolButton;

class NXNavigationBarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXNavigationBar)
  Q_PROPERTY_CREATE_D(int, NavigationBarWidth)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), NavigationViewWidth)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), UserButtonSpacing)
  Q_PROPERTY_CREATE_D(bool, IsTransparent)
  Q_PROPERTY_CREATE_D(bool, IsAllowPageOpenInNewWindow)

public:
  explicit NXNavigationBarPrivate(QObject *parent = nullptr);
  ~NXNavigationBarPrivate();
  Q_SLOT void onNavigationOpenNewWindow(QString const &nodeKey);
  Q_SLOT void onNavigationCloseCurrentWindow(QString const &nodeKey);

  Q_INVOKABLE void onNavigationRoute(QVariantMap const &routeData);

  //核心跳转逻辑
  void onTreeViewClicked(QModelIndex const &index, bool isLogRoute = true, bool isRouteBack = false);
  void onFooterViewClicked(QModelIndex const &index, bool isLogRoute = true, bool isRouteBack = false);

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  friend class NXNavigationView;
  friend class NXNavigationStyle;
  bool m_IsShowUserCard { true };
  NXThemeType::ThemeMode m_ThemeMode;
  NXNavigationType::NavigationDisplayMode m_CurrentDisplayMode { NXNavigationType::NavigationDisplayMode::Maximal };
  QMap<QString, QMetaObject const *> m_PageMetaMap;
  QMap<QString, int> m_PageNewWindowCountMap;
  QMap<NXNavigationNode *, NXMenu *> m_CompactMenuMap;
  QList<NXSuggestBox::SuggestData> m_SuggestDataList;
  QList<NXNavigationNode *> m_LastExpandedNodesList;
  QVBoxLayout *m_UserCardLayout { nullptr };
  QVBoxLayout *m_UserButtonLayout { nullptr };
  NXIconButton *m_UserButton { nullptr };
  NXNavigationModel *m_NavigationModel { nullptr };
  NXNavigationView *m_NavigationView { nullptr };
  NXBaseListView *m_FooterView { nullptr };
  NXFooterModel *m_FooterModel { nullptr };
  NXFooterDelegate *m_FooterDelegate { nullptr };
  NXInteractiveCard *m_UserCard { nullptr };


  void _initNodeModelIndex(QModelIndex const &parentIndex);
  void _resetNodeSelected();
  void _expandSelectedNodeParent();
  void _expandOrCollapseExpanderNode(NXNavigationNode *node, bool isExpand);

  void _addStackedPage(QWidget *page, QString const &pageKey);
  void _addFooterPage(QWidget *page, QString const &footKey);

  void _raiseNavigationBar();
  void _smoothScrollNavigationView(QModelIndex const &index);

  void _doComponentAnimation(NXNavigationType::NavigationDisplayMode displayMode, bool isAnimation);
  void _handleNavigationExpandState(bool isSave);
  void _handleUserButtonLayout(bool isCompact);
  void _resetLayout();

  void _doNavigationBarWidthAnimation(NXNavigationType::NavigationDisplayMode displayMode, bool isAnimation);
  void _doNavigationViewWidthAnimation(bool isAnimation);
  void _doUserButtonAnimation(bool isCompact, bool isAnimation);
};

#endif // NXNAVIGATIONBARPRIVATE_H
