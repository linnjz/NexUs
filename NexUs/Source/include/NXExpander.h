#ifndef NXEXPANDER_H
#define NXEXPANDER_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"
class NXExpanderPrivate;

class NX_EXPORT NXExpander : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXExpander)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Title)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), SubTitle)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, AnimationDuration)
  Q_PROPERTY_CREATE_H(NXIconType::IconName, HeaderIcon)

public:
  enum ExpandDirection
  {
    Down = 0,
    Up
  };
  Q_ENUM(ExpandDirection)

  explicit NXExpander(QWidget *parent = nullptr);
  explicit NXExpander(QString const &title, QWidget *parent = nullptr);
  ~NXExpander();

  void setExpandDirection(ExpandDirection direction);
  ExpandDirection getExpandDirection() const;

  void setContentWidget(QWidget *widget);
  QWidget *getContentWidget() const;

  void setHeaderWidget(QWidget *widget);

  void setIsExpanded(bool expanded);
  bool getIsExpanded() const;
  Q_SIGNAL void expandStateChanged(bool expanded);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  bool event(QEvent *event) override;
};

#endif // NXEXPANDER_H
