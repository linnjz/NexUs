#ifndef NXSTEPS_H
#define NXSTEPS_H

#include <QWidget>

#include "NXProperty.h"

class NXStepsPrivate;

class NX_EXPORT NXSteps : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXSteps)
  Q_PROPERTY_CREATE_H(int, CurrentStep)
  Q_PROPERTY_CREATE_H(int, StepCount)

public:
  explicit NXSteps(QWidget *parent = nullptr);
  ~NXSteps();

  void setStepTitles(QStringList const &titles);
  QStringList getStepTitles() const;

  void next();
  void previous();
  Q_SIGNAL void currentStepChanged(int step);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXSTEPS_H
