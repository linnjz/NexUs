#ifndef NXAPPBARPRIVATE_H
#define NXAPPBARPRIVATE_H
#include <QPoint>

#include "NXDef.h"

class QLabel;
class QScreen;
class QHBoxLayout;
class QVBoxLayout;
class NXText;
class NXAppBar;
class NXIconButton;
class NXToolButton;
class QMenu;

class NXAppBarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXAppBar)
  Q_PRIVATE_CREATE_D(QMenu *, CustomMenu)
  Q_PROPERTY_CREATE_D(int, AppBarHeight)
  Q_PROPERTY_CREATE_D(int, RibbonHeight)
  Q_PROPERTY_CREATE_D(bool, IsStayTop)
  Q_PROPERTY_CREATE_D(bool, IsFixedSize)
  Q_PROPERTY_CREATE_D(bool, IsDefaultClosed)
  Q_PROPERTY_CREATE_D(bool, IsOnlyAllowMinAndClose)

public:
  explicit NXAppBarPrivate(QObject *parent = nullptr);
  ~NXAppBarPrivate();
  Q_SLOT void onMinButtonClicked();
  Q_SLOT void onMaxButtonClicked();
  Q_SLOT void onCloseButtonClicked();
  Q_SLOT void onStayTopButtonClicked();

private:
#ifndef Q_OS_WIN
  bool m_IsDragging { false };
#endif
  bool m_IsHoverMaxButton { false };
  NXThemeType::ThemeMode m_ThemeMode;
  NXAppBarType::ButtonFlags m_ButtonFlags;
  int m_LastMinTrackWidth { 0 };
  int m_Edges { 0 };
  int m_Margins { 8 };
  int m_Win7Margins { 0 };
  qint64 m_CurrentWinID { 0 };
  quint64 m_ClickTimer { 0 };
#ifndef Q_OS_WIN
  QPoint m_DragStartPos;
#endif
  QList<QWidget *> m_CustomAreaWidgetList { nullptr, nullptr, nullptr };
  QList<QObject *> m_CustomAreaHitTestObjectList { nullptr, nullptr, nullptr };
  QStringList m_CustomAreaHitTestFunctionNameList { QStringLiteral(""), QStringLiteral(""), QStringLiteral("") };
  QList<QWidget *> m_ClientWidgetList;
  QHBoxLayout *m_MainLayout { nullptr };
  QVBoxLayout *m_IconLabelLayout { nullptr };
  QVBoxLayout *m_TitleLabelLayout { nullptr };
  NXToolButton *m_RouteBackButton { nullptr };
  NXToolButton *m_RouteForwardButton { nullptr };
  NXToolButton *m_NavigationButton { nullptr };
  NXToolButton *m_ThemeChangeButton { nullptr };
  NXToolButton *m_StayTopButton { nullptr };
  NXToolButton *m_MinButton { nullptr };
  NXToolButton *m_MaxButton { nullptr };
  NXIconButton *m_CloseButton { nullptr };
  QScreen *m_LastScreen { nullptr };
  NXText *m_TitleLabel { nullptr };
  QLabel *m_IconLabel { nullptr };
  void _changeMaxButtonAwesome(bool isMaximized);
  void _showAppBarMenu(QPoint point);
  void _updateCursor(int edges);
  bool _containsCursorToItem(QWidget *item);
  void _onThemeModeChange(NXThemeType::ThemeMode themeMode);
  int _calculateMinimumWidth();
  QVBoxLayout *_createVLayout(QWidget *widget);
};

#endif // NXAPPBARPRIVATE_H
