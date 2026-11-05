#ifndef NXSPLASHSCREEN_H
#define NXSPLASHSCREEN_H

#include <QWidget>

#include "NXProperty.h"

class NXSplashScreenPrivate;

class NX_EXPORT NXSplashScreen : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXSplashScreen)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, Minimum)
  Q_PROPERTY_CREATE_H(int, Maximum)
  Q_PROPERTY_CREATE_H(int, Value)
  Q_PROPERTY_CREATE_H(bool, IsShowProgressBar)
  Q_PROPERTY_CREATE_H(bool, IsShowProgressRing)
  Q_PROPERTY_CREATE_H(bool, IsClosable)

public:
  explicit NXSplashScreen(QWidget *parent = nullptr);
  explicit NXSplashScreen(QPixmap const &logo, QWidget *parent = nullptr);
  ~NXSplashScreen();

  void setLogo(QPixmap const &logo);
  void setTitle(QString const &title);
  void setSubTitle(QString const &subTitle);
  void setStatusText(QString const &text);

  void show();
  void close();
  void finish(QWidget *mainWindow);
  Q_SIGNAL void closed();

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // NXSPLASHSCREEN_H
