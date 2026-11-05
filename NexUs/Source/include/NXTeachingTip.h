#ifndef NXTEACHINGTIP_H
#define NXTEACHINGTIP_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"
class NXTeachingTipPrivate;

class NX_EXPORT NXTeachingTip : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXTeachingTip)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Title)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), SubTitle)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Content)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QPixmap), HeroImage)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(NXIconType::IconName, TipIcon)
  Q_PROPERTY_CREATE_H(bool, IsLightDismiss)

public:
  enum TailPosition
  {
    Auto = 0,
    Top,
    Bottom,
    Left,
    Right
  };
  Q_ENUM(TailPosition)

  explicit NXTeachingTip(QWidget *parent = nullptr);
  ~NXTeachingTip();

  void setTailPosition(TailPosition position);
  TailPosition getTailPosition() const;

  void setTarget(QWidget *target);
  QWidget *getTarget() const;

  void setCloseButtonVisible(bool visible);

  void addAction(QString const &text, std::function<void()> const &callback);
  void clearActions();

  void showTip();
  void closeTip();
  Q_SIGNAL void closed();
  Q_SIGNAL void closeButtonClicked();

protected:
  void paintEvent(QPaintEvent *event) override;
  bool eventFilter(QObject *watched, QEvent *event) override;
  void showEvent(QShowEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
};

#endif // NXTEACHINGTIP_H
