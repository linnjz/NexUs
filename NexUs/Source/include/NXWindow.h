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

  void setCustomWidget(NXAppBarType::CustomArea customArea, QWidget *customWidget, QObject *hitTestObject = nullptr, const QString &hitTestFunctionName = QStringLiteral(""));
  QWidget *getCustomWidget(NXAppBarType::CustomArea customArea) const;

  void setCentralCustomWidget(QWidget *customWidget);
  QWidget *getCentralCustomWidget() const;

  void setCustomMenu(QMenu *customMenu);
  QMenu *getCustomMenu() const;

  void setUserInfoCardVisible(bool isVisible);
  void setUserInfoCardPixmap(const QPixmap &pix);
  void setUserInfoCardTitle(const QString &title);
  void setUserInfoCardSubTitle(const QString &subTitle);

  QString addExpanderNode(const QString &expanderTitle, NXIconType::IconName awesome = NXIconType::None) const;
  NXNodeResultExpected addExpanderNode(const QString &expanderTitle, const QString &targetExpanderKey, NXIconType::IconName awesome = NXIconType::None) const;

  NXNodeResultExpected
  addPageNode(const QString &pageTitle, QWidget *page, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addPageNode(const QString &pageTitle, QWidget *page, const QString &targetExpanderKey, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addPageNode(const QString &pageTitle, QWidget *page, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);
  NXNodeResultExpected addPageNode(const QString &pageTitle, QWidget *page, const QString &targetExpanderKey, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);

  NXNodeResultExpected
  addFooterNode(const QString &footerTitle, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None) const;
  NXNodeResultExpected addFooterNode(const QString &footerTitle, QWidget *page, int keyPoints = 0, NXIconType::IconName awesome = NXIconType::None);

  QString addCategoryNode(const QString &categoryTitle);
  NXNodeResultExpected addCategoryNode(const QString &categoryTitle, const QString &targetExpanderKey);

  void addCentralWidget(QWidget *centralWidget);
  QWidget *getCentralWidget(int index) const;

  bool getNavigationNodeIsExpanded(const QString &expanderKey) const;
  void expandNavigationNode(const QString &expanderKey);
  void collapseNavigationNode(const QString &expanderKey);
  void removeNavigationNode(const QString &nodeKey) const;
  int getPageOpenInNewWindowCount(const QString &nodeKey) const;
  void backtrackNavigationNode(const QString &nodeKey);

  void setNodeKeyPoints(const QString &nodeKey, int keyPoints);
  int getNodeKeyPoints(const QString &nodeKey) const;

  void setNavigationNodeTitle(const QString &nodeKey, const QString &nodeTitle);
  QString getNavigationNodeTitle(const QString &nodeKey) const;

  void navigation(const QString &pageKey);
  int getCurrentNavigationIndex() const;
  QString getCurrentNavigationPageKey() const;

  QList<NXSuggestBox::SuggestData> getNavigationSuggestDataList() const;

  void setWindowButtonFlag(NXAppBarType::ButtonType buttonFlag, bool isEnable = true);
  void setWindowButtonFlags(NXAppBarType::ButtonFlags buttonFlags);
  NXAppBarType::ButtonFlags getWindowButtonFlags() const;

  void setWindowMoviePath(NXThemeType::ThemeMode themeMode, const QString &moviePath);
  QString getWindowMoviePath(NXThemeType::ThemeMode themeMode) const;

  void setWindowPixmap(NXThemeType::ThemeMode themeMode, const QPixmap &pixmap);
  QPixmap getWindowPixmap(NXThemeType::ThemeMode themeMode) const;

  void setWindowMovieRate(qreal rate);
  qreal getWindowMovieRate() const;

  void tabifyDockWidget(QDockWidget *targetDockWidget, QDockWidget *dockWidget);
  void tabifyDockWidget(Qt::DockWidgetArea area, const QString &targetDockTitle, QDockWidget *dockWidget);
  Q_SIGNAL void userInfoCardClicked();
  Q_SIGNAL void closeButtonClicked();
  Q_SIGNAL void navigationNodeClicked(NXNavigationType::NavigationNodeType nodeType, const QString &nodeKey);
  Q_SIGNAL void navigationNodeRemoved(NXNavigationType::NavigationNodeType nodeType, const QString &nodeKey);
  Q_SIGNAL void customWidgetChanged();
  Q_SIGNAL void centralCustomWidgetChanged();
  Q_SIGNAL void customMenuChanged();
  Q_SIGNAL void pageOpenInNewWindow(const QString &nodeKey);

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
