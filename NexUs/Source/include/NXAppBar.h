#ifndef NXAPPBAR_H
#define NXAPPBAR_H

#include <QWidget>

#include "NXDef.h"

#ifdef Q_OS_WIN
#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#    define Q_TAKEOVER_NATIVEEVENT_H \
      bool nativeEvent(QByteArray const &eventType, void *message, qintptr *result) override;
#  else
#    define Q_TAKEOVER_NATIVEEVENT_H \
      bool nativeEvent(QByteArray const &eventType, void *message, long *result) override;
#  endif
#else
#  define Q_TAKEOVER_NATIVEEVENT_H
#endif

#ifdef Q_OS_WIN
#  define NXAPPBAR_HANDLE(NXAppBar)                                        \
    if (NXAppBar) {                                                        \
      int ret = NXAppBar->takeOverNativeEvent(eventType, message, result); \
      if (ret == -1) {                                                     \
        return QWidget::nativeEvent(eventType, message, result);           \
      }                                                                    \
      return (bool) ret;                                                   \
    }                                                                      \
    return QWidget::nativeEvent(eventType, message, result);
#endif

#ifdef Q_OS_WIN
#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#    define Q_TAKEOVER_NATIVEEVENT_CPP(CLASS, NXAppBar)                                    \
      bool CLASS::nativeEvent(QByteArray const &eventType, void *message, qintptr *result) \
      {                                                                                    \
        NXAPPBAR_HANDLE(NXAppBar)                                                          \
      }
#  else
#    define Q_TAKEOVER_NATIVEEVENT_CPP(CLASS, NXAppBar)                                 \
      bool CLASS::nativeEvent(QByteArray const &eventType, void *message, long *result) \
      {                                                                                 \
        NXAPPBAR_HANDLE(NXAppBar)                                                       \
      }
#  endif
#else
#  define Q_TAKEOVER_NATIVEEVENT_CPP(CLASS, NXAppBar)
#endif

class QMenu;
class NXAppBarPrivate;

class NX_EXPORT NXAppBar : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXAppBar)
  Q_PROPERTY_CREATE_H(int, AppBarHeight)
  Q_PROPERTY_CREATE_H(int, RibbonHeight)
  Q_PROPERTY_CREATE_H(bool, IsStayTop)
  Q_PROPERTY_CREATE_H(bool, IsFixedSize)
  Q_PROPERTY_CREATE_H(bool, IsDefaultClosed)
  Q_PROPERTY_CREATE_H(bool, IsOnlyAllowMinAndClose)

public:
  explicit NXAppBar(QWidget *parent = nullptr);
  ~NXAppBar();

  void setCustomWidget(NXAppBarType::CustomArea customArea, QWidget *customWidget, QObject *hitTestObject = nullptr, QString const &hitTestFunctionName = QStringLiteral(""));
  QWidget *getCustomWidget(NXAppBarType::CustomArea customArea) const;

  void setCustomMenu(QMenu *customMenu);
  QMenu *getCustomMenu() const;

  void setWindowButtonFlag(NXAppBarType::ButtonType buttonFlag, bool isEnable = true);
  void setWindowButtonFlags(NXAppBarType::ButtonFlags buttonFlags);
  NXAppBarType::ButtonFlags getWindowButtonFlags() const;

  void setRouteBackButtonEnable(bool isEnable);
  void setRouteForwardButtonEnable(bool isEnable);

#ifdef Q_OS_WIN
#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  int takeOverNativeEvent(QByteArray const &eventType, void *message, qintptr *result);
#  else
  int takeOverNativeEvent(QByteArray const &eventType, void *message, long *result);
#  endif
#endif
  Q_SIGNAL void routeBackButtonClicked();
  Q_SIGNAL void routeForwardButtonClicked();
  Q_SIGNAL void navigationButtonClicked();
  Q_SIGNAL void themeChangeButtonClicked();
  Q_SIGNAL void closeButtonClicked();
  Q_SIGNAL void customWidgetChanged();
  Q_SIGNAL void customMenuChanged();

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;
#ifdef Q_OS_WIN
  void paintEvent(QPaintEvent *event) override;
#endif
#ifdef Q_OS_MACOS
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
#endif
};

#endif // NXAPPBAR_H
