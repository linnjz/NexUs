#ifndef NXDOCKWIDGET_H
#define NXDOCKWIDGET_H

#include <QDockWidget>

#include "NXProperty.h"
class NXDockWidgetPrivate;

class NX_EXPORT NXDockWidget : public QDockWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXDockWidget)

public:
  explicit NXDockWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
  explicit NXDockWidget(QString const &title, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
  ~NXDockWidget();
  Q_SIGNAL void dockClosed();
  Q_SIGNAL void dockResized(QSize size);

protected:
  void resizeEvent(QResizeEvent *event) override;
  void closeEvent(QCloseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
#ifdef Q_OS_WIN
  bool event(QEvent *event) override;
#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  bool nativeEvent(QByteArray const &eventType, void *message, qintptr *result) override;
#  else
  bool nativeEvent(QByteArray const &eventType, void *message, long *result) override;
#  endif
#endif
};

#endif // NXDOCKWIDGET_H
