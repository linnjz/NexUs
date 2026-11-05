#ifndef NXSPOTLIGHT_H
#define NXSPOTLIGHT_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXSpotlightPrivate;

class NX_EXPORT NXSpotlight : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXSpotlight)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Title)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Content)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, Padding)
  Q_PROPERTY_CREATE_H(int, OverlayAlpha)
  Q_PROPERTY_CREATE_H(bool, IsCircle)

public:
  explicit NXSpotlight(QWidget *parent = nullptr);
  ~NXSpotlight();

  void showSpotlight(QWidget *target, QString const &buttonText = QStringLiteral("知道了"));

  struct SpotlightStep
  {
    QWidget *target { nullptr };
    QString title;
    QString content;
    bool isCircle { false };
  };

  void setSteps(QList<SpotlightStep> const &steps);
  void start();
  void next();
  void previous();
  void finish();

  int currentStep() const;
  int stepCount() const;
  Q_SIGNAL void stepChanged(int step);
  Q_SIGNAL void finished();

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // NXSPOTLIGHT_H
