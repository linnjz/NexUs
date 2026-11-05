#include "NXNavigationBar.h"

#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QScroller>
#include <QVBoxLayout>

#include "DeveloperComponents/NXBaseListView.h"
#include "DeveloperComponents/NXFooterDelegate.h"
#include "DeveloperComponents/NXFooterModel.h"
#include "DeveloperComponents/NXNavigationModel.h"
#include "DeveloperComponents/NXNavigationNode.h"
#include "DeveloperComponents/NXNavigationView.h"
#include "NXIconButton.h"
#include "NXInteractiveCard.h"
#include "NXMenu.h"
#include "NXSuggestBox.h"
#include "NXTheme.h"
#include "NXToolButton.h"
#include "NXToolTip.h"
#include "private/NXNavigationBarPrivate.h"
#include "private/NXSuggestBoxPrivate.h"
Q_PROPERTY_CREATE_CPP(NXNavigationBar, QPS_PAEMIT(bool), IsTransparent)
Q_PROPERTY_CREATE_CPP(NXNavigationBar, QPS_PAEMIT(bool), IsAllowPageOpenInNewWindow)

NXNavigationBar::NXNavigationBar(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXNavigationBarPrivate())
{
  Q_D(NXNavigationBar);
  d->q_ptr                        = this;
  d->m_IsAllowPageOpenInNewWindow = true;
  d->m_NavigationBarWidth         = 300;
  setFixedWidth(300);
  d->m_IsTransparent = true;

  //用户卡片
  d->m_UserCard = new NXInteractiveCard(this);
  d->m_UserCard->setMinimumWidth(0);
  d->m_UserCard->setCardPixmap(QPixmap(QStringLiteral(":/Resource/Image/Cirno.jpg")));
  d->m_UserCard->setTitle(QStringLiteral("NX Tool"));
  d->m_UserCard->setSubTitle(QStringLiteral("Liniyous@gmail.com"));
  connect(d->m_UserCard, &NXInteractiveCard::clicked, this, &NXNavigationBar::userInfoCardClicked);
  d->m_UserButton = new NXIconButton(QPixmap(QStringLiteral(":/Resource/Image/Cirno.jpg")), this);
  d->m_UserButton->setFixedSize(36, 36);
  d->m_UserButton->setVisible(false);
  d->m_UserButton->setBorderRadius(8);
  d->m_UserButtonLayout = new QVBoxLayout();
  d->m_UserButtonLayout->setAlignment(Qt::AlignLeft);
  d->m_UserButtonLayout->setContentsMargins(0, 0, 0, 0);
  d->m_UserButtonLayout->setSpacing(0);
  d->m_UserButtonLayout->addWidget(d->m_UserButton);

  connect(d->m_UserButton, &NXIconButton::clicked, this, &NXNavigationBar::userInfoCardClicked);
  d->m_UserCardLayout = new QVBoxLayout();
  d->m_UserCardLayout->setContentsMargins(3, 10, 0, 5);
  d->m_UserCardLayout->addLayout(d->m_UserButtonLayout);
  d->m_UserCardLayout->addWidget(d->m_UserCard);

  // 搜索跳转
  //    connect(d->m_NavigationSuggestBox, &NXSuggestBox::suggestionClicked, this, [=](QString suggestText, QVariantMap suggestData) {
  //        navigation(suggestData.value("NXPageKey").toString());
  //    });

  // 导航模型
  d->m_NavigationModel = new NXNavigationModel(this);
  d->m_NavigationView  = new NXNavigationView(this);
  d->m_NavigationView->setNavigationBarPrivate(d);
  d->m_NavigationView->setModel(d->m_NavigationModel);
  connect(d->m_NavigationView, &NXNavigationView::navigationClicked, this, [=](QModelIndex const &index) {
    d->onTreeViewClicked(index);
  });
  connect(d->m_NavigationView, &NXNavigationView::navigationOpenNewWindow, d, &NXNavigationBarPrivate::onNavigationOpenNewWindow);
  connect(d->m_NavigationView, &NXNavigationView::navigationCloseCurrentWindow, d, &NXNavigationBarPrivate::onNavigationCloseCurrentWindow);
  connect(d->m_NavigationModel, &NXNavigationModel::rowsMoved, this, [=]() {
    d->_initNodeModelIndex(QModelIndex());
    d->_resetNodeSelected();
  });
  // 页脚
  d->m_FooterView = new NXBaseListView(this);
  d->m_FooterView->setFixedHeight(0);
  d->m_FooterModel = new NXFooterModel(this);
  d->m_FooterView->setModel(d->m_FooterModel);
  d->m_FooterDelegate = new NXFooterDelegate(this);
  d->m_FooterDelegate->setNXListView(d->m_FooterView);
  d->m_FooterView->setItemDelegate(d->m_FooterDelegate);
  connect(d->m_FooterView, &NXBaseListView::mousePress, this, [=](QModelIndex const &index) {
    d->m_FooterDelegate->setPressIndex(index);
    d->m_FooterView->viewport()->update();
  });
  connect(d->m_FooterView, &NXBaseListView::mouseDoubleClick, this, [=](QModelIndex const &index) {
    d->m_FooterDelegate->setPressIndex(index);
    d->m_FooterView->viewport()->update();
  });
  connect(d->m_FooterView, &NXBaseListView::mouseRelease, this, [=](QModelIndex const &index) {
    d->m_FooterDelegate->setPressIndex(QModelIndex());
    d->m_FooterView->viewport()->update();
  });
  connect(d->m_FooterView, &NXBaseListView::clicked, this, [=](QModelIndex const &index) {
    d->onFooterViewClicked(index);
  });

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setAlignment(Qt::AlignLeft);
  mainLayout->setSpacing(0);
  mainLayout->addLayout(d->m_UserCardLayout);
  mainLayout->addWidget(d->m_NavigationView);
  mainLayout->addWidget(d->m_FooterView);

  // 主题设置
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXNavigationBar::~NXNavigationBar()
{
}

void NXNavigationBar::setNavigationBarWidth(int navigationBarWidth)
{
  Q_D(NXNavigationBar);
  if (navigationBarWidth < 180) {
    navigationBarWidth = 180;
  }
  if (d->m_CurrentDisplayMode == NXNavigationType::NavigationDisplayMode::Maximal) {
    setFixedWidth(navigationBarWidth);
  }
  d->m_NavigationBarWidth = navigationBarWidth;
  Q_EMIT signalNavigationBarWidthChanged();
}

int NXNavigationBar::getNavigationBarWidth() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXNavigationBar);
  return d->m_NavigationBarWidth;
}

void NXNavigationBar::setUserInfoCardVisible(bool isVisible)
{
  Q_D(NXNavigationBar);
  d->m_IsShowUserCard = isVisible;
  if (d->m_CurrentDisplayMode == NXNavigationType::NavigationDisplayMode::Compact) {
    d->m_UserButton->setVisible(isVisible);
  } else {
    d->m_UserCard->setVisible(isVisible);
  }
  if (isVisible) {
    d->m_UserCardLayout->setContentsMargins(3, 10, 5, 5);
  } else {
    d->m_UserCardLayout->setContentsMargins(0, 0, 0, 0);
  }
}

void NXNavigationBar::setUserInfoCardPixmap(QPixmap const &pix)
{
  Q_D(NXNavigationBar);
  d->m_UserCard->setCardPixmap(pix);
  d->m_UserButton->setPixmap(pix);
}

void NXNavigationBar::setUserInfoCardTitle(QString const &title)
{
  Q_D(NXNavigationBar);
  d->m_UserCard->setTitle(title);
}

void NXNavigationBar::setUserInfoCardSubTitle(QString const &subTitle)
{
  Q_D(NXNavigationBar);
  d->m_UserCard->setSubTitle(subTitle);
}

void NXNavigationBar::setIsNodeDragDropEnable(bool isEnable)
{
  Q_D(NXNavigationBar);
  d->m_NavigationView->setIsDragDropEnable(isEnable);
}

void NXNavigationBar::setToolTipOffset(int offsetX, int offsetY)
{
  Q_D(NXNavigationBar);
  d->m_NavigationView->getCompactToolTip()->setOffSetX(offsetX);
  d->m_NavigationView->getCompactToolTip()->setOffSetY(offsetY);
}

QString NXNavigationBar::addExpanderNode(QString const &expanderTitle, NXIconType::IconName awesome)
{
  Q_D(NXNavigationBar);
  QString key = d_ptr->m_NavigationModel->addExpanderNode(expanderTitle, awesome);
  d->_initNodeModelIndex(QModelIndex());
  d->_resetNodeSelected();
  Q_EMIT navigationNodeAdded(NXNavigationType::ExpanderNode, key, nullptr);
  return key;
}

NXNodeResultExpected NXNavigationBar::addExpanderNode(QString const &expanderTitle, QString const &targetExpanderKey, NXIconType::IconName awesome)
{
  Q_D(NXNavigationBar);
  NXNodeResultExpected returnType = d->m_NavigationModel->addExpanderNode(expanderTitle, targetExpanderKey, awesome);
  if (returnType.has_value()) {
    d->_initNodeModelIndex(QModelIndex());
    d->_resetNodeSelected();
    Q_EMIT navigationNodeAdded(NXNavigationType::ExpanderNode, *returnType, nullptr);
  }
  return returnType;
}

NXNodeResultExpected NXNavigationBar::addPageNode(QString const &pageTitle, QWidget *page, NXIconType::IconName awesome)
{
  Q_D(NXNavigationBar);
  if (!page) {
    return NXNodeResultUnexpected { NXNavigationType::PageInvalid };
  }
  QString key = d_ptr->m_NavigationModel->addPageNode(pageTitle, awesome);
  d->m_PageMetaMap.insert(key, page->metaObject());
  d->m_PageNewWindowCountMap.insert(key, 0);
  d->_addStackedPage(page, key);
  d->_initNodeModelIndex(QModelIndex());
  d->_resetNodeSelected();
  return key;
}

NXNodeResultExpected NXNavigationBar::addPageNode(QString const &pageTitle, QWidget *page, int keyPoints, NXIconType::IconName awesome)
{
  Q_D(NXNavigationBar);
  if (!page) {
    return NXNodeResultUnexpected { NXNavigationType::PageInvalid };
  }
  NXNodeResultExpected returnType = d_ptr->m_NavigationModel->addPageNode(pageTitle, keyPoints, awesome);
  if (returnType.has_value()) {
    d->m_PageMetaMap.insert(*returnType, page->metaObject());
    d->m_PageNewWindowCountMap.insert(*returnType, 0);
    d->_addStackedPage(page, *returnType);
    d->_initNodeModelIndex(QModelIndex());
    d->_resetNodeSelected();
  }
  return returnType;
}

NXNodeResultExpected NXNavigationBar::addPageNode(QString const &pageTitle, QWidget *page, QString const &targetExpanderKey, NXIconType::IconName awesome)
{
  Q_D(NXNavigationBar);
  if (!page) {
    return NXNodeResultUnexpected { NXNavigationType::PageInvalid };
  }
  if (targetExpanderKey.isEmpty()) {
    return NXNodeResultUnexpected { NXNavigationType::TargetNodeInvalid };
  }
  NXNodeResultExpected returnType = d->m_NavigationModel->addPageNode(pageTitle, targetExpanderKey, awesome);
  if (returnType.has_value()) {
    d->m_PageMetaMap.insert(*returnType, page->metaObject());
    d->m_PageNewWindowCountMap.insert(*returnType, 0);
    NXNavigationNode *node         = d->m_NavigationModel->getNavigationNode(*returnType);
    NXNavigationNode *originalNode = node->getOriginalNode();
    if (d->m_CompactMenuMap.contains(originalNode)) {
      NXMenu *menu    = d->m_CompactMenuMap.value(originalNode);
      QAction *action = menu->addNXIconAction(node->getAwesome(), node->getNodeTitle());
      connect(action, &QAction::triggered, this, [=]() {
        d->onTreeViewClicked(node->getModelIndex());
      });
    } else {
      NXMenu *menu    = new NXMenu(const_cast<NXNavigationBar *>(this));
      QAction *action = menu->addNXIconAction(node->getAwesome(), node->getNodeTitle());
      connect(action, &QAction::triggered, this, [=]() {
        d_ptr->onTreeViewClicked(node->getModelIndex());
      });
      d_ptr->m_CompactMenuMap.insert(originalNode, menu);
    }
    d_ptr->_addStackedPage(page, *returnType);
    d->_initNodeModelIndex(QModelIndex());
    d->_resetNodeSelected();
  }
  return returnType;
}

NXNodeResultExpected NXNavigationBar::addPageNode(QString const &pageTitle, QWidget *page, QString const &targetExpanderKey, int keyPoints, NXIconType::IconName awesome)
{
  Q_D(NXNavigationBar);
  if (!page) {
    return NXNodeResultUnexpected { NXNavigationType::PageInvalid };
  }
  if (targetExpanderKey.isEmpty()) {
    return NXNodeResultUnexpected { NXNavigationType::TargetNodeInvalid };
  }
  NXNodeResultExpected returnType =
      d_ptr->m_NavigationModel->addPageNode(pageTitle, targetExpanderKey, keyPoints, awesome);
  if (returnType.has_value()) {
    d->m_PageMetaMap.insert(*returnType, page->metaObject());
    d->m_PageNewWindowCountMap.insert(*returnType, 0);
    NXNavigationNode *node         = d_ptr->m_NavigationModel->getNavigationNode(*returnType);
    NXNavigationNode *originalNode = node->getOriginalNode();
    if (d_ptr->m_CompactMenuMap.contains(originalNode)) {
      NXMenu *menu    = d_ptr->m_CompactMenuMap.value(originalNode);
      QAction *action = menu->addNXIconAction(node->getAwesome(), node->getNodeTitle());
      connect(action, &QAction::triggered, this, [=]() {
        d_ptr->onTreeViewClicked(node->getModelIndex());
      });
    } else {
      NXMenu *menu    = new NXMenu(const_cast<NXNavigationBar *>(this));
      QAction *action = menu->addNXIconAction(node->getAwesome(), node->getNodeTitle());
      connect(action, &QAction::triggered, this, [=]() {
        d_ptr->onTreeViewClicked(node->getModelIndex());
      });
      d_ptr->m_CompactMenuMap.insert(originalNode, menu);
    }
    d_ptr->_addStackedPage(page, *returnType);
    d->_initNodeModelIndex(QModelIndex());
    d->_resetNodeSelected();
  }
  return returnType;
}

NXNodeResultExpected NXNavigationBar::addFooterNode(QString const &footerTitle, int keyPoints, NXIconType::IconName awesome)
{
  return addFooterNode(footerTitle, nullptr, keyPoints, awesome);
}

NXNodeResultExpected NXNavigationBar::addFooterNode(QString const &footerTitle, QWidget *page, int keyPoints, NXIconType::IconName awesome)
{
  NXNodeResultExpected returnType =
      d_ptr->m_FooterModel->addFooterNode(footerTitle, page != nullptr, keyPoints, awesome);
  if (returnType.has_value()) {
    d_ptr->_addFooterPage(page, *returnType);
  }
  return returnType;
}

QString NXNavigationBar::addCategoryNode(QString const &categoryTitle)
{
  Q_D(NXNavigationBar);
  QString key = d_ptr->m_NavigationModel->addCategoryNode(categoryTitle);
  d->_initNodeModelIndex(QModelIndex());
  d->_resetNodeSelected();
  Q_EMIT navigationNodeAdded(NXNavigationType::CategoryNode, key, nullptr);
  return key;
}

NXNodeResultExpected NXNavigationBar::addCategoryNode(QString const &categoryTitle, QString const &targetExpanderKey)
{
  Q_D(NXNavigationBar);
  NXNodeResultExpected result = d_ptr->m_NavigationModel->addCategoryNode(categoryTitle, targetExpanderKey);
  if (result.has_value()) {
    d->_initNodeModelIndex(QModelIndex());
    d->_resetNodeSelected();
    Q_EMIT navigationNodeAdded(NXNavigationType::CategoryNode, *result, nullptr);
  }
  return result;
}

bool NXNavigationBar::getNodeIsExpanded(QString const &expanderKey) const
{
  Q_CONST_D(NXNavigationBar);
  NXNavigationNode *node = d->m_NavigationModel->getNavigationNode(expanderKey);
  if (!node || !node->getIsExpanderNode()) {
    return false;
  }
  return d->m_NavigationView->isExpanded(node->getModelIndex());
}

void NXNavigationBar::expandNode(QString const &expanderKey)
{
  Q_D(NXNavigationBar);
  NXNavigationNode *node = d->m_NavigationModel->getNavigationNode(expanderKey);
  if (!node || !node->getIsExpanderNode()) {
    return;
  }
  d->_expandOrCollapseExpanderNode(node, true);
  d->_resetNodeSelected();
}

void NXNavigationBar::collapseNode(QString const &expanderKey)
{
  Q_D(NXNavigationBar);
  NXNavigationNode *node = d->m_NavigationModel->getNavigationNode(expanderKey);
  if (!node || !node->getIsExpanderNode()) {
    return;
  }
  d->_expandOrCollapseExpanderNode(node, false);
  d->_resetNodeSelected();
}

void NXNavigationBar::removeNode(QString const &nodeKey)
{
  Q_D(NXNavigationBar);
  NXNavigationNode *node = d->m_NavigationModel->getNavigationNode(nodeKey);
  if (!node) {
    node = d->m_FooterModel->getNavigationNode(nodeKey);
  }
  if (!node) {
    return;
  }
  if (node->getIsFooterNode()) {
    Q_EMIT navigationNodeRemoved(NXNavigationType::FooterNode, nodeKey);
    d->m_FooterModel->removeNavigationNode(nodeKey);
    d->m_FooterView->setFixedHeight(40 * d->m_FooterModel->getFooterNodeCount());
  } else {
    if (node->getIsCategoryNode()) {
      QStringList removeKeyList = d->m_NavigationModel->removeNavigationNode(nodeKey);
      d->_initNodeModelIndex(QModelIndex());
      for (auto const &removeKey : removeKeyList) {
        Q_EMIT navigationNodeRemoved(NXNavigationType::CategoryNode, removeKey);
      }
    } else {
      QStringList removeKeyList = d->m_NavigationModel->removeNavigationNode(nodeKey);
      d->_initNodeModelIndex(QModelIndex());
      for (auto const &removeKey : removeKeyList) {
        d->m_PageMetaMap.remove(removeKey);
        d->m_PageNewWindowCountMap.remove(removeKey);
        Q_EMIT navigationNodeRemoved(NXNavigationType::PageNode, removeKey);
      }
    }
  }
  for (int i = 0; i < d->m_SuggestDataList.count(); i++) {
    auto &suggestData = d->m_SuggestDataList[i];
    if (suggestData.getSuggestData().value(QStringLiteral("NXPageKey")).toString() == nodeKey) {
      d->m_SuggestDataList.removeAt(i);
      break;
    }
  }
}

void NXNavigationBar::setNodeKeyPoints(QString const &nodeKey, int keyPoints)
{
  Q_D(NXNavigationBar);
  NXNavigationNode *node = d->m_NavigationModel->getNavigationNode(nodeKey);
  if (!node) {
    node = d->m_FooterModel->getNavigationNode(nodeKey);
  } else if (node->getIsExpanderNode() || keyPoints < 0) {
    return;
  }
  if (!node) {
    return;
  }
  node->setKeyPoints(keyPoints);
  if (node->getIsFooterNode()) {
    d->m_FooterView->viewport()->update();
  } else {
    d->m_NavigationView->viewport()->update();
  }
}

int NXNavigationBar::getNodeKeyPoints(QString const &nodeKey) const
{
  Q_CONST_D(NXNavigationBar);
  NXNavigationNode *node = d->m_NavigationModel->getNavigationNode(nodeKey);
  if (!node) {
    node = d->m_FooterModel->getNavigationNode(nodeKey);
  } else if (node->getIsExpanderNode()) {
    return -1;
  }
  if (!node) {
    return -1;
  }
  return node->getKeyPoints();
}

void NXNavigationBar::setNodeTitle(QString const &nodeKey, QString const &nodeTitle)
{
  Q_D(NXNavigationBar);
  NXNavigationNode *node = d->m_NavigationModel->getNavigationNode(nodeKey);
  if (!node) {
    node = d->m_FooterModel->getNavigationNode(nodeKey);
  }
  if (!node) {
    return;
  }
  node->setNodeTitle(nodeTitle);
  update();
}

QString NXNavigationBar::getNodeTitle(QString const &nodeKey) const
{
  Q_CONST_D(NXNavigationBar);
  NXNavigationNode *node = d->m_NavigationModel->getNavigationNode(nodeKey);
  if (!node) {
    node = d->m_FooterModel->getNavigationNode(nodeKey);
  }
  if (!node) {
    return {};
  }
  return node->getNodeTitle();
}

void NXNavigationBar::navigation(QString const &pageKey, bool isLogClicked, bool isRouteBack)
{
  Q_D(NXNavigationBar);
  NXNavigationNode *node = d->m_NavigationModel->getNavigationNode(pageKey);
  if (!node) {
    node = d->m_FooterModel->getNavigationNode(pageKey);
  }
  if (node) {
    if (node->getIsFooterNode()) {
      d->onFooterViewClicked(node->getModelIndex(), isLogClicked, isRouteBack);
    } else {
      if (!node->getIsExpanderNode()) {
        d->onTreeViewClicked(node->getModelIndex(), isLogClicked, isRouteBack);
        d->_smoothScrollNavigationView(node->getModelIndex());
      }
    }
  }
}

void NXNavigationBar::setDisplayMode(NXNavigationType::NavigationDisplayMode displayMode, bool isAnimation)
{
  Q_D(NXNavigationBar);
  if (d->m_CurrentDisplayMode == displayMode || displayMode == NXNavigationType::Auto) {
    return;
  }
  d->m_NavigationModel->setIsMaximalMode(displayMode == NXNavigationType::Maximal);
  d->_doComponentAnimation(displayMode, isAnimation);
  d->_raiseNavigationBar();
  Q_EMIT displayModeChanged(displayMode);
}

NXNavigationType::NavigationDisplayMode NXNavigationBar::getDisplayMode() const
{
  Q_CONST_D(NXNavigationBar);
  return d->m_CurrentDisplayMode;
}

int NXNavigationBar::getPageOpenInNewWindowCount(QString const &nodeKey) const
{
  Q_CONST_D(NXNavigationBar);
  if (!d->m_PageNewWindowCountMap.contains(nodeKey)) {
    return 0;
  }
  return d->m_PageNewWindowCountMap[nodeKey];
}

QList<NXSuggestBox::SuggestData> NXNavigationBar::getSuggestDataList() const
{
  Q_CONST_D(NXNavigationBar);
  return d->m_SuggestDataList;
}

void NXNavigationBar::paintEvent(QPaintEvent *event)
{
  Q_D(NXNavigationBar);
  if (!d->m_IsTransparent) {
    QPainter painter(this);
    painter.save();
    painter.setPen(NXThemeColor(d->m_ThemeMode, PopupBorder));
    painter.setBrush(NXThemeColor(d->m_ThemeMode, PopupBase));
    QRect baseRect = this->rect();
    baseRect.adjust(-1, 0, -1, 0);
    QPainterPath path;
    path.moveTo(baseRect.topLeft());
    path.lineTo(QPoint(baseRect.right() - 8, baseRect.y()));
    path.arcTo(QRectF(baseRect.right() - 16, baseRect.y(), 16, 16), 90, -90);
    path.lineTo(QPoint(baseRect.right(), baseRect.bottom() - 8));
    path.arcTo(QRectF(baseRect.right() - 16, baseRect.bottom() - 16, 16, 16), 0, -90);
    path.lineTo(baseRect.bottomLeft());
    path.closeSubpath();
    painter.drawPath(path);
    painter.restore();
  }
  QWidget::paintEvent(event);
}
