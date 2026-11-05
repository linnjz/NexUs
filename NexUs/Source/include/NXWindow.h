#ifndef NXWINDOW_H
#define NXWINDOW_H

#include <QMainWindow>

#include "NXAppBar.h"
#include "NXDef.h"
#include "NXSuggestBox.h"
class NXWindowPrivate;

class NX_EXPORT NXWindow : public QMainWindow
{
  Q_OBJECT
  Q_Q_CREATE(NXWindow)
  Q_PROPERTY_CREATE_H(int, AppBarHeight)
  Q_PROPERTY_CREATE_H(int, RibbonHeight)
  Q_PROPERTY_CREATE_H(int, ThemeChangeTime)
  Q_PROPERTY_CREATE_H(int, NavigationBarWidth)
  Q_PROPERTY_CREATE_H(int, CurrentStackIndex)
  Q_PROPERTY_CREATE_H(NXNavigationType::NavigationDisplayMode, NavigationBarDisplayMode)
  Q_PROPERTY_CREATE_H(NXWindowType::StackSwitchMode, StackSwitchMode)
  Q_PROPERTY_CREATE_H(NXWindowType::PaintMode, WindowPaintMode)
  Q_PROPERTY_CREATE_H(bool, IsStayTop)
  Q_PROPERTY_CREATE_H(bool, IsFixedSize)
  Q_PROPERTY_CREATE_H(bool, IsDefaultClosed)
  Q_PROPERTY_CREATE_H(bool, IsCentralStackedWidgetTransparent)
  Q_PROPERTY_CREATE_H(bool, IsAllowPageOpenInNewWindow)
  Q_PROPERTY_CREATE_H(bool, IsNavigationBarEnable)
  Q_TAKEOVER_NATIVEEVENT_H

public:
  Q_INVOKABLE explicit NXWindow(QWidget *parent = nullptr);
  ~NXWindow();

  void moveToCenter();

  void setCustomWidget(NXAppBarType::CustomArea customArea, QWidget *customWidget, QObject *hitTestObject = nullptr, QString const &hitTestFunctionName = QStringLiteral(""));
  QWidget *getCustomWidget(NXAppBarType::CustomArea customArea) const;

  void setCentralCustomWidget(QWidget *customWidget);
  QWidget *getCentralCustomWidget() const;

  void setCustomMenu(QMenu *customMenu);
  QMenu *getCustomMenu() const;

  void setUserInfoCardVisible(bool isVisible);
  void setUserInfoCardPixmap(QPixmap const &pix);
  void setUserInfoCardTitle(QString const &title);
  void setUserInfoCardSubTitle(QString const &subTitle);

  QString addExpanderNode(QString const &expanderTitle, NXIconType::IconName awesome = NXIconType::None) const;
  NXNodeResultExpected addExpanderNode(QString const &expanderTitle, QString const &targetExpanderKey, NXIconType::IconName awesome = NXIconType::None) const;

  NXNodeResultExpected
  addPageNode(QString const &pageTitle, QWidget *page, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addPageNode(QString const &pageTitle, QWidget *page, QString const &targetExpanderKey, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addPageNode(QString const &pageTitle, QWidget *page, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addPageNode(QString const &pageTitle, QWidget *page, QString const &targetExpanderKey, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);

  NXNodeResultExpected
  addFooterNode(QString const &footerTitle, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None) const;
  NXNodeResultExpected addFooterNode(QString const &footerTitle, QWidget *page, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);

  QString addCategoryNode(QString const &categoryTitle);
  NXNodeResultExpected addCategoryNode(QString const &categoryTitle, QString const &targetExpanderKey);

  void addCentralWidget(QWidget *centralWidget);
  QWidget *getCentralWidget(int index) const;

  bool getNavigationNodeIsExpanded(QString const &expanderKey) const;
  void expandNavigationNode(QString const &expanderKey);
  void collapseNavigationNode(QString const &expanderKey);
  void removeNavigationNode(QString const &nodeKey) const;
  int getPageOpenInNewWindowCount(QString const &nodeKey) const;
  void backtrackNavigationNode(QString const &nodeKey);

  void setNodeKeyPoints(QString const &nodeKey, int keyPoints);
  int getNodeKeyPoints(QString const &nodeKey) const;

  void setNavigationNodeTitle(QString const &nodeKey, QString const &nodeTitle);
  QString getNavigationNodeTitle(QString const &nodeKey) const;

  void navigation(QString const &pageKey);
  int getCurrentNavigationIndex() const;
  QString getCurrentNavigationPageKey() const;

  QList<NXSuggestBox::SuggestData> getNavigationSuggestDataList() const;

  void setWindowButtonFlag(NXAppBarType::ButtonType buttonFlag, bool isEnable = true);
  void setWindowButtonFlags(NXAppBarType::ButtonFlags buttonFlags);
  NXAppBarType::ButtonFlags getWindowButtonFlags() const;

  void setWindowMoviePath(NXThemeType::ThemeMode themeMode, QString const &moviePath);
  QString getWindowMoviePath(NXThemeType::ThemeMode themeMode) const;

  void setWindowPixmap(NXThemeType::ThemeMode themeMode, QPixmap const &pixmap);
  QPixmap getWindowPixmap(NXThemeType::ThemeMode themeMode) const;

  void setWindowMovieRate(qreal rate);
  qreal getWindowMovieRate() const;

  void tabifyDockWidget(QDockWidget *targetDockWidget, QDockWidget *dockWidget);
  void tabifyDockWidget(Qt::DockWidgetArea area, QString const &targetDockTitle, QDockWidget *dockWidget);
  Q_SIGNAL void userInfoCardClicked();
  Q_SIGNAL void closeButtonClicked();
  Q_SIGNAL void navigationNodeClicked(NXNavigationType::NavigationNodeType nodeType, QString const &nodeKey);
  Q_SIGNAL void navigationNodeRemoved(NXNavigationType::NavigationNodeType nodeType, QString const &nodeKey);
  Q_SIGNAL void customWidgetChanged();
  Q_SIGNAL void centralCustomWidgetChanged();
  Q_SIGNAL void customMenuChanged();
  Q_SIGNAL void pageOpenInNewWindow(QString const &nodeKey);

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;
  virtual QMenu *createPopupMenu() override;
  void paintEvent(QPaintEvent *event) override;
#ifdef Q_OS_MACOS
  void mousePressEvent(QMouseEvent *event) override;
#endif

private:
  QWidget *centralWidget() const;
  void setCentralWidget(QWidget *widget);
};

#endif // NXWINDOW_H
