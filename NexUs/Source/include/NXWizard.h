#ifndef NXWIZARD_H
#define NXWIZARD_H

#include <QWidget>

#include "NXProperty.h"

class NXWizardPrivate;

class NX_EXPORT NXWizard : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXWizard)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, CurrentStep)

public:
  explicit NXWizard(QWidget *parent = nullptr);
  ~NXWizard();

  void addStep(QString const &title, QWidget *page);
  void next();
  void previous();
  void finish();
  int getStepCount() const;
  Q_SIGNAL void currentStepChanged(int step);
  Q_SIGNAL void finished();
  Q_SIGNAL void cancelled();

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
};

#endif // NXWIZARD_H
