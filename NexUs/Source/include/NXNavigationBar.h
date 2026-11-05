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
  void setUserInfoCardPixmap(QPixmap const &pix);
  void setUserInfoCardTitle(QString const &title);
  void setUserInfoCardSubTitle(QString const &subTitle);
  void setIsNodeDragDropEnable(bool isEnable);
  void setToolTipOffset(int offsetX, int offsetY);

  QString addExpanderNode(QString const &expanderTitle, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addExpanderNode(QString const &expanderTitle, QString const &targetExpanderKey, NXIconType::IconName awesome = NXIconType::None);

  NXNodeResultExpected
  addPageNode(QString const &pageTitle, QWidget *page, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addPageNode(QString const &pageTitle, QWidget *page, QString const &targetExpanderKey, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addPageNode(QString const &pageTitle, QWidget *page, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addPageNode(QString const &pageTitle, QWidget *page, QString const &targetExpanderKey, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);

  NXNodeResultExpected
  addFooterNode(QString const &footerTitle, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addFooterNode(QString const &footerTitle, QWidget *page, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);

  QString addCategoryNode(QString const &categoryTitle);
  NXNodeResultExpected addCategoryNode(QString const &categoryTitle, QString const &targetExpanderKey);

  bool getNodeIsExpanded(QString const &expanderKey) const;
  void expandNode(QString const &expanderKey);
  void collapseNode(QString const &expanderKey);
  void removeNode(QString const &nodeKey);

  void setNodeKeyPoints(QString const &nodeKey, int keyPoints);
  int getNodeKeyPoints(QString const &nodeKey) const;

  void setNodeTitle(QString const &nodeKey, QString const &nodeTitle);
  QString getNodeTitle(QString const &nodeKey) const;

  void navigation(QString const &pageKey, bool isLogClicked = true, bool isRouteBack = false);
  void setDisplayMode(NXNavigationType::NavigationDisplayMode displayMode, bool isAnimation = true);
  NXNavigationType::NavigationDisplayMode getDisplayMode() const;

  int getPageOpenInNewWindowCount(QString const &nodeKey) const;

  QList<NXSuggestBox::SuggestData> getSuggestDataList() const;
  Q_SIGNAL void pageOpenInNewWindow(QString const &nodeKey);
  Q_SIGNAL void userInfoCardClicked();
  Q_SIGNAL void
  navigationNodeClicked(NXNavigationType::NavigationNodeType nodeType, QString const &nodeKey, bool isRouteBack);
  Q_SIGNAL void
  navigationNodeAdded(NXNavigationType::NavigationNodeType nodeType, QString const &nodeKey, QWidget *page);
  Q_SIGNAL void navigationNodeRemoved(NXNavigationType::NavigationNodeType nodeType, QString const &nodeKey);
  Q_SIGNAL void displayModeChanged(NXNavigationType::NavigationDisplayMode displayMode);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXNAVIGATIONBAR_H
