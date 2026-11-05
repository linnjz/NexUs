#ifndef NXFRAMEWORK_NEXUS_INCLUDE_NXROLLER_H_
#define NXFRAMEWORK_NEXUS_INCLUDE_NXROLLER_H_

#include <QWidget>

#include "NXProperty.h"

class NXRollerPrivate;

class NX_EXPORT NXRoller : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXRoller)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QStringList), ItemList)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, ItemHeight)
  Q_PROPERTY_CREATE_H(int, MaxVisibleItems)
  Q_PROPERTY_CREATE_H(int, CurrentIndex)
  Q_PROPERTY_CREATE_H(bool, IsContainer)
  Q_PROPERTY_CREATE_H(bool, IsEnableLoop)

public:
  explicit NXRoller(QWidget *parent = nullptr);
  ~NXRoller();

  void setCurrentData(QString const &data);
  QString getCurrentData() const;
  Q_SIGNAL void currentDataChanged(QString const &data);

protected:
  void wheelEvent(QWheelEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
};

#endif //NXFRAMEWORK_NEXUS_INCLUDE_NXROLLER_H_
