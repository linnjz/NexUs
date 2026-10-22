#ifndef NXNAVIGATIONBAR_H
#define NXNAVIGATIONBAR_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"
#include "NXSuggestBox.h"
class NXNavigationBarPrivate;

class NX_EXPORT NXNavigationBar : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXNavigationBar)
  Q_PROPERTY_CREATE_H(int, NavigationBarWidth)
  Q_PROPERTY_CREATE_H(bool, IsTransparent)
  Q_PROPERTY_CREATE_H(bool, IsAllowPageOpenInNewWindow)

public:
  explicit NXNavigationBar(QWidget *parent = nullptr);
  ~NXNavigationBar();
  void setUserInfoCardVisible(bool isVisible);
  void setUserInfoCardPixmap(const QPixmap &pix);
  void setUserInfoCardTitle(const QString &title);
  void setUserInfoCardSubTitle(const QString &subTitle);
  void setIsNodeDragDropEnable(bool isEnable);
  void setToolTipOffset(int offsetX, int offsetY);

  QString addExpanderNode(const QString &expanderTitle, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addExpanderNode(const QString &expanderTitle, const QString &targetExpanderKey, NXIconType::IconName awesome = NXIconType::None);

  NXNodeResultExpected
  addPageNode(const QString &pageTitle, QWidget *page, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addPageNode(const QString &pageTitle, QWidget *page, const QString &targetExpanderKey, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addPageNode(const QString &pageTitle, QWidget *page, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addPageNode(const QString &pageTitle, QWidget *page, const QString &targetExpanderKey, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);

  NXNodeResultExpected
  addFooterNode(const QString &footerTitle, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addFooterNode(const QString &footerTitle, QWidget *page, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);

  QString addCategoryNode(const QString &categoryTitle);
  NXNodeResultExpected addCategoryNode(const QString &categoryTitle, const QString &targetExpanderKey);

  bool getNodeIsExpanded(const QString &expanderKey) const;
  void expandNode(const QString &expanderKey);
  void collapseNode(const QString &expanderKey);
  void removeNode(const QString &nodeKey);

  void setNodeKeyPoints(const QString &nodeKey, int keyPoints);
  int getNodeKeyPoints(const QString &nodeKey) const;

  void setNodeTitle(const QString &nodeKey, const QString &nodeTitle);
  QString getNodeTitle(const QString &nodeKey) const;

  void navigation(const QString &pageKey, bool isLogClicked = true, bool isRouteBack = false);
  void setDisplayMode(NXNavigationType::NavigationDisplayMode displayMode, bool isAnimation = true);
  NXNavigationType::NavigationDisplayMode getDisplayMode() const;

  int getPageOpenInNewWindowCount(const QString &nodeKey) const;

  QList<NXSuggestBox::SuggestData> getSuggestDataList() const;
  Q_SIGNAL void pageOpenInNewWindow(const QString &nodeKey);
  Q_SIGNAL void userInfoCardClicked();
  Q_SIGNAL void
  navigationNodeClicked(NXNavigationType::NavigationNodeType nodeType, const QString &nodeKey, bool isRouteBack);
  Q_SIGNAL void
  navigationNodeAdded(NXNavigationType::NavigationNodeType nodeType, const QString &nodeKey, QWidget *page);
  Q_SIGNAL void navigationNodeRemoved(NXNavigationType::NavigationNodeType nodeType, const QString &nodeKey);
  Q_SIGNAL void displayModeChanged(NXNavigationType::NavigationDisplayMode displayMode);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXNAVIGATIONBAR_H
