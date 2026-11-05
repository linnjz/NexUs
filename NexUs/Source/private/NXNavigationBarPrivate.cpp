#include <QApplication>
#include "NXNavigationBarPrivate.h"

#include <QDebug>
#include <QEvent>
#include <QLayout>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QTimer>

#include "DeveloperComponents/NXBaseListView.h"
#include "DeveloperComponents/NXCustomTabWidget.h"
#include "DeveloperComponents/NXCustomWidget.h"
#include "DeveloperComponents/NXFooterDelegate.h"
#include "DeveloperComponents/NXFooterModel.h"
#include "DeveloperComponents/NXNavigationModel.h"
#include "DeveloperComponents/NXNavigationNode.h"
#include "DeveloperComponents/NXNavigationView.h"
#include "NXApplication.h"
#include "NXIconButton.h"
#include "NXInteractiveCard.h"
#include "NXMenu.h"
#include "NXNavigationBar.h"
#include "NXNavigationRouter.h"
#include "NXToolButton.h"

NXNavigationBarPrivate::NXNavigationBarPrivate(QObject *parent)
    : QObject { parent }
{
}

NXNavigationBarPrivate::~NXNavigationBarPrivate()
{
}

void NXNavigationBarPrivate::onNavigationOpenNewWindow(QString const &nodeKey)
{
  Q_Q(NXNavigationBar);
  QMetaObject const *meta = m_PageMetaMap.value(nodeKey);
  if (!meta) {
    return;
  }
  QWidget *widget = dynamic_cast<QWidget *>(meta->newInstance());
  if (widget) {
    widget->setProperty("NXPageKey", nodeKey);
    widget->setProperty("IsMetaWidget", true);
    widget->setProperty("NXFloatParentWidget", QVariant::fromValue(q));
    widget->installEventFilter(this);
    NXCustomTabWidget *floatWidget = new NXCustomTabWidget(q);
    floatWidget->addTab(widget, widget->windowIcon(), widget->windowTitle());
    floatWidget->show();
    Q_EMIT q->pageOpenInNewWindow(nodeKey);
  }
}

void NXNavigationBarPrivate::onNavigationCloseCurrentWindow(QString const &nodeKey)
{
  Q_Q(NXNavigationBar);
  QMetaObject const *meta = m_PageMetaMap.value(nodeKey);
  if (!meta) {
    return;
  }
  q->removeNode(nodeKey);
}

void NXNavigationBarPrivate::onNavigationRoute(QVariantMap const &routeData)
{
  Q_Q(NXNavigationBar);
  bool isRouteBack = routeData.value(QStringLiteral("NXRouteBackMode")).toBool();
  QString pageKey  = isRouteBack ? routeData.value(QStringLiteral("NXBackPageKey")).toString()
                                 : routeData.value(QStringLiteral("NXForwardPageKey")).toString();
  q->navigation(pageKey, false, isRouteBack);
}

void NXNavigationBarPrivate::onTreeViewClicked(QModelIndex const &index, bool isLogRoute, bool isRouteBack)
{
  Q_Q(NXNavigationBar);
  if (index.isValid()) {
    NXNavigationNode *node = static_cast<NXNavigationNode *>(index.internalPointer());
    if (!node) {
      return;
    }
    if (node->getIsCategoryNode()) {
      Q_EMIT q->navigationNodeClicked(NXNavigationType::CategoryNode, node->getNodeKey(), isRouteBack);
      return;
    }
    if (node->getIsExpanderNode()) {
      _expandOrCollapseExpanderNode(node, !m_NavigationView->isExpanded(index));
    } else {
      if (node->getKeyPoints()) {
        node->setKeyPoints(0);
        m_NavigationView->update();
      }
      NXNavigationNode *selectedNode = m_NavigationModel->getSelectedNode();
      if (selectedNode != node) {
        // 记录跳转
        if (isLogRoute) {
          QVariantMap routeData = QVariantMap();
          QString backPageKey;
          if (selectedNode) {
            backPageKey = selectedNode->getNodeKey();
          } else {
            if (m_FooterModel->getSelectedNode()) {
              backPageKey = m_FooterModel->getSelectedNode()->getNodeKey();
            }
          }
          routeData.insert(QStringLiteral("NXBackPageKey"), backPageKey);
          routeData.insert(QStringLiteral("NXForwardPageKey"), node->getNodeKey());
          NXNavigationRouter::getInstance()->navigationRoute(q->window(), this, QStringLiteral("onNavigationRoute"), routeData);
        }
        Q_EMIT q->navigationNodeClicked(NXNavigationType::PageNode, node->getNodeKey(), isRouteBack);

        if (m_FooterModel->getSelectedNode()) {
          m_FooterView->clearSelection();
          QVariantMap footerPostData = QVariantMap();
          footerPostData.insert(QStringLiteral("SelectMarkChanged"), true);
          footerPostData.insert(QStringLiteral("LastSelectedNode"), QVariant::fromValue(m_FooterModel->getSelectedNode()));
          footerPostData.insert(QStringLiteral("SelectedNode"), QVariant::fromValue(nullptr));
          m_FooterModel->setSelectedNode(nullptr);
          m_FooterDelegate->navigationNodeStateChange(footerPostData);
        }
        QVariantMap postData = QVariantMap();
        postData.insert(QStringLiteral("SelectMarkChanged"), true);
        if (m_NavigationModel->getSelectedExpandedNode()) {
          postData.insert(QStringLiteral("LastSelectedNode"), QVariant::fromValue(m_NavigationModel->getSelectedExpandedNode()));
        } else {
          postData.insert(QStringLiteral("LastSelectedNode"), QVariant::fromValue(m_NavigationModel->getSelectedNode()));
        }
        if (m_CurrentDisplayMode == NXNavigationType::Compact) {
          NXNavigationNode *originNode = node->getOriginalNode();
          if (originNode == node) {
            postData.insert(QStringLiteral("SelectedNode"), QVariant::fromValue(node));
          } else {
            if (originNode == m_NavigationModel->getSelectedExpandedNode()) {
              m_NavigationModel->setSelectedNode(node);
              _resetNodeSelected();
              return;
            }
            m_NavigationModel->setSelectedExpandedNode(originNode);
            postData.insert(QStringLiteral("SelectedNode"), QVariant::fromValue(originNode));
          }
        } else {
          postData.insert(QStringLiteral("SelectedNode"), QVariant::fromValue(node));
        }
        m_NavigationModel->setSelectedNode(node);
        m_NavigationView->navigationNodeStateChange(postData);
        if (!node->getIsVisible() && m_CurrentDisplayMode != NXNavigationType::Compact) {
          _expandSelectedNodeParent();
        }
      }
    }
    _resetNodeSelected();
  }
}

void NXNavigationBarPrivate::onFooterViewClicked(QModelIndex const &index, bool isLogRoute, bool isRouteBack)
{
  Q_Q(NXNavigationBar);
  NXNavigationNode *node = index.data(Qt::UserRole).value<NXNavigationNode *>();
  if (node->getKeyPoints()) {
    node->setKeyPoints(0);
    m_NavigationView->update();
  }
  NXNavigationNode *selectedNode = m_FooterModel->getSelectedNode();

  if (selectedNode != node) {
    // 记录跳转
    if (isLogRoute && node->getIsHasFooterPage()) {
      QVariantMap routeData = QVariantMap();
      QString backPageKey;
      if (selectedNode) {
        backPageKey = selectedNode->getNodeKey();
      } else {
        if (m_NavigationModel->getSelectedNode()) {
          backPageKey = m_NavigationModel->getSelectedNode()->getNodeKey();
        }
      }
      routeData.insert(QStringLiteral("NXBackPageKey"), backPageKey);
      routeData.insert(QStringLiteral("NXForwardPageKey"), node->getNodeKey());
      NXNavigationRouter::getInstance()->navigationRoute(this, QStringLiteral("onNavigationRoute"), routeData);
    }
    Q_EMIT q->navigationNodeClicked(NXNavigationType::FooterNode, node->getNodeKey(), isRouteBack);

    if (node->getIsHasFooterPage()) {
      if (m_NavigationModel->getSelectedNode() || m_NavigationModel->getSelectedExpandedNode()) {
        QVariantMap mainPostData = QVariantMap();
        mainPostData.insert(QStringLiteral("SelectMarkChanged"), true);
        mainPostData.insert(QStringLiteral("LastSelectedNode"), QVariant::fromValue(m_NavigationModel->getSelectedExpandedNode() ? m_NavigationModel->getSelectedExpandedNode() : m_NavigationModel->getSelectedNode()));
        mainPostData.insert(QStringLiteral("SelectedNode"), QVariant::fromValue(nullptr));
        m_NavigationView->clearSelection();
        m_NavigationView->navigationNodeStateChange(mainPostData);
        m_NavigationModel->setSelectedExpandedNode(nullptr);
        m_NavigationModel->setSelectedNode(nullptr);
      }
      m_FooterView->clearSelection();
      m_FooterView->selectionModel()->select(index, QItemSelectionModel::Select);
      QVariantMap postData = QVariantMap();
      postData.insert(QStringLiteral("SelectMarkChanged"), true);
      postData.insert(QStringLiteral("LastSelectedNode"), QVariant::fromValue(m_FooterModel->getSelectedNode()));
      postData.insert(QStringLiteral("SelectedNode"), QVariant::fromValue(node));
      m_FooterDelegate->navigationNodeStateChange(postData);
      m_FooterModel->setSelectedNode(node);
    }
  }
}

bool NXNavigationBarPrivate::eventFilter(QObject *watched, QEvent *event)
{
  switch (event->type()) {
  case QEvent::Show : {
    QString nodeKey = watched->property("NXPageKey").toString();
    if (!nodeKey.isNull()) {
      m_PageNewWindowCountMap[nodeKey] += 1;
    }
    break;
  }
  case QEvent::HideToParent : {
    QString nodeKey = watched->property("NXPageKey").toString();
    if (!nodeKey.isNull()) {
      m_PageNewWindowCountMap[nodeKey] -= 1;
    }
    break;
  }
  default : {
    break;
  }
  }
  return QObject::eventFilter(watched, event);
}

void NXNavigationBarPrivate::_initNodeModelIndex(QModelIndex const &parentIndex)
{
  int rowCount = m_NavigationModel->rowCount(parentIndex);
  for (int row = 0; row < rowCount; ++row) {
    QModelIndex index           = m_NavigationModel->index(row, 0, parentIndex);
    NXNavigationNode *childNode = static_cast<NXNavigationNode *>(index.internalPointer());
    childNode->setModelIndex(index);
    if (m_NavigationModel->hasChildren(index)) {
      _initNodeModelIndex(index);
    }
  }
}

void NXNavigationBarPrivate::_resetNodeSelected()
{
  m_NavigationView->clearSelection();
  NXNavigationNode *selectedNode = m_NavigationModel->getSelectedNode();
  if (!selectedNode || !selectedNode->getModelIndex().isValid()) {
    return;
  }
  if (selectedNode->getParentNode()->getIsRootNode() || selectedNode->getIsVisible()) {
    m_NavigationView->selectionModel()->select(selectedNode->getModelIndex(), QItemSelectionModel::Select);
    if (m_NavigationModel->getSelectedExpandedNode()) {
      QVariantMap postData = QVariantMap();
      postData.insert(QStringLiteral("SelectMarkChanged"), true);
      postData.insert(QStringLiteral("LastSelectedNode"), QVariant::fromValue(m_NavigationModel->getSelectedExpandedNode()));
      postData.insert(QStringLiteral("SelectedNode"), QVariant::fromValue(selectedNode));
      m_NavigationView->navigationNodeStateChange(postData);
    }
    m_NavigationModel->setSelectedExpandedNode(nullptr);
  } else {
    NXNavigationNode *parentNode = selectedNode->getParentNode();
    while (parentNode && !parentNode->getParentNode()->getIsRootNode() && !parentNode->getIsVisible()) {
      parentNode = parentNode->getParentNode();
    }
    if (!parentNode) {
      return;
    }
    // 单级节点展开/收起时Mark变化
    if (!m_NavigationModel->getSelectedExpandedNode()) {
      QVariantMap postData = QVariantMap();
      postData.insert(QStringLiteral("SelectMarkChanged"), true);
      postData.insert(QStringLiteral("LastSelectedNode"), QVariant::fromValue(m_NavigationModel->getSelectedNode()));
      postData.insert(QStringLiteral("SelectedNode"), QVariant::fromValue(parentNode));
      m_NavigationView->navigationNodeStateChange(postData);
    } else {
      // 多级节点
      if (m_NavigationModel->getSelectedExpandedNode() != parentNode) {
        // 同一起源节点展开/收起时的Mark变化
        if (m_NavigationModel->getSelectedExpandedNode()->getOriginalNode() == parentNode->getOriginalNode()) {
          QVariantMap postData = QVariantMap();
          postData.insert(QStringLiteral("SelectMarkChanged"), true);
          postData.insert(QStringLiteral("LastSelectedNode"), QVariant::fromValue(m_NavigationModel->getSelectedExpandedNode()));
          postData.insert(QStringLiteral("SelectedNode"), QVariant::fromValue(parentNode));
          m_NavigationView->navigationNodeStateChange(postData);
        }
      }
    }
    m_NavigationModel->setSelectedExpandedNode(parentNode);
    m_NavigationView->selectionModel()->select(parentNode->getModelIndex(), QItemSelectionModel::Select);
  }
}

void NXNavigationBarPrivate::_expandSelectedNodeParent()
{
  NXNavigationNode *parentNode = m_NavigationModel->getSelectedNode()->getParentNode();
  while (parentNode && !parentNode->getIsRootNode()) {
    QVariantMap data;
    data.insert(QStringLiteral("Expand"), QVariant::fromValue(parentNode));
    m_NavigationView->navigationNodeStateChange(data);
    parentNode->setIsExpanded(true);
    m_NavigationView->expand(parentNode->getModelIndex());
    parentNode = parentNode->getParentNode();
  }
}

void NXNavigationBarPrivate::_expandOrCollapseExpanderNode(NXNavigationNode *node, bool isExpand)
{
  if (m_CurrentDisplayMode == NXNavigationType::Compact) {
    if (node->getIsHasPageChild()) {
      //展开菜单
      NXMenu *menu = m_CompactMenuMap.value(node);
      if (menu) {
        QPoint nodeTopRight =
            m_NavigationView->mapToGlobal(m_NavigationView->visualRect(node->getModelIndex()).topRight());
        menu->popup(QPoint(nodeTopRight.x() + 10, nodeTopRight.y()));
      }
    }
  } else {
    QModelIndex index = node->getModelIndex();
    bool isExpanded   = m_NavigationView->isExpanded(index);
    if (node->getIsHasChild() && isExpand != isExpanded) {
      QVariantMap data;
      if (isExpanded) {
        // 收起
        data.insert(QStringLiteral("Collapse"), QVariant::fromValue(node));
        node->setIsExpanded(isExpand);
        m_NavigationView->navigationNodeStateChange(data);
        m_NavigationView->collapse(index);
      } else {
        // 展开
        data.insert(QStringLiteral("Expand"), QVariant::fromValue(node));
        node->setIsExpanded(true);
        m_NavigationView->navigationNodeStateChange(data);
        m_NavigationView->expand(index);
      }
    }
  }
}

void NXNavigationBarPrivate::_addStackedPage(QWidget *page, QString const &pageKey)
{
  Q_Q(NXNavigationBar);
  page->setProperty("NXPageKey", pageKey);
  Q_EMIT q->navigationNodeAdded(NXNavigationType::PageNode, pageKey, page);
  NXNavigationNode *node = m_NavigationModel->getNavigationNode(pageKey);
  QVariantMap suggestData;
  suggestData.insert(QStringLiteral("NXPageKey"), pageKey);
  m_SuggestDataList.append(NXSuggestBox::SuggestData(node->getAwesome(), node->getNodeTitle(), suggestData));
}

void NXNavigationBarPrivate::_addFooterPage(QWidget *page, QString const &footKey)
{
  Q_Q(NXNavigationBar);
  Q_EMIT q->navigationNodeAdded(NXNavigationType::FooterNode, footKey, page);
  if (page) {
    page->setProperty("NXPageKey", footKey);
  }
  m_FooterView->setFixedHeight(40 * m_FooterModel->getFooterNodeCount());
  NXNavigationNode *node = m_FooterModel->getNavigationNode(footKey);
  QVariantMap suggestData;
  suggestData.insert(QStringLiteral("NXPageKey"), footKey);
  m_SuggestDataList.append(NXSuggestBox::SuggestData(node->getAwesome(), node->getNodeTitle(), suggestData));
}

void NXNavigationBarPrivate::_raiseNavigationBar()
{
  Q_Q(NXNavigationBar);
  q->raise();
}

void NXNavigationBarPrivate::_smoothScrollNavigationView(QModelIndex const &index)
{
  QTimer::singleShot(200, this, [=]() {
    if (m_CurrentDisplayMode == NXNavigationType::Compact) {
      return;
    }
    auto vScrollBar = m_NavigationView->verticalScrollBar();
    int startValue  = vScrollBar->value();
    m_NavigationView->scrollTo(index, QAbstractItemView::PositionAtCenter);
    int endValue                        = vScrollBar->value();
    QPropertyAnimation *scrollAnimation = new QPropertyAnimation(vScrollBar, "value");
    scrollAnimation->setEasingCurve(QEasingCurve::OutSine);
    scrollAnimation->setDuration(255);
    scrollAnimation->setStartValue(startValue);
    scrollAnimation->setEndValue(endValue);
    scrollAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  });
}

void NXNavigationBarPrivate::_doComponentAnimation(NXNavigationType::NavigationDisplayMode displayMode, bool isAnimation)
{
  switch (displayMode) {
  case NXNavigationType::Minimal : {
    _doNavigationBarWidthAnimation(displayMode, isAnimation);
    if (m_CurrentDisplayMode == NXNavigationType::Maximal) {
      m_UserCard->setVisible(false);
      if (m_IsShowUserCard) {
        m_UserButton->setVisible(true);
      }
      _handleNavigationExpandState(true);
    }
    m_CurrentDisplayMode = displayMode;
    break;
  }
  case NXNavigationType::Compact : {
    _doNavigationBarWidthAnimation(displayMode, isAnimation);
    _doNavigationViewWidthAnimation(isAnimation);
    if (m_CurrentDisplayMode != NXNavigationType::Minimal) {
      _handleUserButtonLayout(true);
      _doUserButtonAnimation(true, isAnimation);
      _handleNavigationExpandState(true);
    }
    m_CurrentDisplayMode = displayMode;
    break;
  }
  case NXNavigationType::Maximal : {
    _handleUserButtonLayout(false);
    _doNavigationBarWidthAnimation(displayMode, isAnimation);
    _doUserButtonAnimation(false, isAnimation);
    m_CurrentDisplayMode = displayMode;
    _handleNavigationExpandState(false);
    break;
  }
  default : {
    break;
  }
  }
}

void NXNavigationBarPrivate::_handleNavigationExpandState(bool isSave)
{
  if (isSave) {
    // 保存展开状态 收起根节点所有子树
    m_LastExpandedNodesList = m_NavigationModel->getRootExpandedNodes();
    for (auto node : m_LastExpandedNodesList) {
      onTreeViewClicked(node->getModelIndex(), false);
    }
  } else {
    // 修正动画覆盖
    m_NavigationView->resize(m_NavigationBarWidth - 5, m_NavigationView->height());
    for (auto node : m_LastExpandedNodesList) {
      onTreeViewClicked(node->getModelIndex(), false);
    }
  }
}

void NXNavigationBarPrivate::_handleUserButtonLayout(bool isCompact)
{
  while (m_UserButtonLayout->count()) {
    m_UserButtonLayout->takeAt(0);
  }
  if (m_IsShowUserCard) {
    m_UserButtonLayout->addSpacing(isCompact ? 36 : 80);
  }
}

void NXNavigationBarPrivate::_resetLayout()
{
  while (m_UserButtonLayout->count()) {
    m_UserButtonLayout->takeAt(0);
  }
  m_UserButtonLayout->addWidget(m_UserButton);
}

void NXNavigationBarPrivate::_doNavigationBarWidthAnimation(NXNavigationType::NavigationDisplayMode displayMode, bool isAnimation)
{
  Q_Q(NXNavigationBar);
  QPropertyAnimation *navigationBarWidthAnimation = new QPropertyAnimation(q, "maximumWidth");
  navigationBarWidthAnimation->setEasingCurve(QEasingCurve::OutCubic);
  navigationBarWidthAnimation->setStartValue(q->width());
  navigationBarWidthAnimation->setDuration(isAnimation ? 255 : 0);
  switch (displayMode) {
  case NXNavigationType::Minimal : {
    connect(navigationBarWidthAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      q->setFixedWidth(value.toUInt());
    });
    navigationBarWidthAnimation->setEndValue(0);
    break;
  }
  case NXNavigationType::Compact : {
    connect(navigationBarWidthAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      q->setFixedWidth(value.toUInt());
    });
    navigationBarWidthAnimation->setEndValue(42);
    break;
  }
  case NXNavigationType::Maximal : {
    connect(navigationBarWidthAnimation, &QPropertyAnimation::finished, this, [=]() {
      _resetLayout();
    });
    connect(navigationBarWidthAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      q->setFixedWidth(value.toUInt());
    });
    navigationBarWidthAnimation->setEndValue(m_NavigationBarWidth);
    break;
  }
  default : {
    break;
  }
  }
  navigationBarWidthAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXNavigationBarPrivate::_doNavigationViewWidthAnimation(bool isAnimation)
{
  QPropertyAnimation *navigationViewWidthAnimation = new QPropertyAnimation(this, "NavigationViewWidth");
  connect(navigationViewWidthAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    m_NavigationView->setColumnWidth(0, value.toUInt());
  });
  navigationViewWidthAnimation->setEasingCurve(QEasingCurve::OutCubic);
  navigationViewWidthAnimation->setStartValue(m_NavigationView->columnWidth(0));
  navigationViewWidthAnimation->setEndValue(40);
  navigationViewWidthAnimation->setDuration(isAnimation ? 255 : 0);
  navigationViewWidthAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXNavigationBarPrivate::_doUserButtonAnimation(bool isCompact, bool isAnimation)
{
  QPropertyAnimation *userButtonAnimation = new QPropertyAnimation(m_UserButton, "geometry");
  connect(userButtonAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    m_UserButton->setFixedSize(value.toRect().size());
  });
  userButtonAnimation->setEasingCurve(isCompact ? QEasingCurve::OutCubic : QEasingCurve::InOutSine);
  QRect maximumRect = QRect(13, 18, 64, 64);
  QRect compactRect = QRect(3, 10, 36, 36);
  userButtonAnimation->setStartValue(isCompact ? maximumRect : compactRect);
  userButtonAnimation->setEndValue(isCompact ? compactRect : maximumRect);

  QPropertyAnimation *spacingAnimation = new QPropertyAnimation(this, "UserButtonSpacing");
  connect(spacingAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    while (m_UserButtonLayout->count()) {
      m_UserButtonLayout->takeAt(0);
    }
    if (m_IsShowUserCard) {
      m_UserButtonLayout->addSpacing(value.toInt());
    }
  });
  spacingAnimation->setEasingCurve(isCompact ? QEasingCurve::OutCubic : QEasingCurve::InOutSine);
  spacingAnimation->setStartValue(isCompact ? 80 : 36);
  spacingAnimation->setEndValue(isCompact ? 36 : 80);
  if (isCompact) {
    m_UserCard->setVisible(false);
    if (m_IsShowUserCard) {
      m_UserButton->setVisible(true);
    }
    userButtonAnimation->setDuration(isAnimation ? 255 : 0);
    spacingAnimation->setDuration(isAnimation ? 255 : 0);
  } else {
    connect(spacingAnimation, &QPropertyAnimation::finished, this, [=]() {
      m_UserButton->setFixedSize(36, 36);
      m_UserButton->setGeometry(QRect(3, 10, 36, 36));
      m_UserButton->setVisible(false);
      _resetLayout();
      if (m_IsShowUserCard) {
        m_UserCard->setVisible(true);
      }
    });
    userButtonAnimation->setDuration(isAnimation ? 135 : 0);
    spacingAnimation->setDuration(isAnimation ? 135 : 0);
  }
  userButtonAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  spacingAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}
