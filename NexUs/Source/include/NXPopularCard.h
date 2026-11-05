#ifndef NXPOPULARCARD_H
#define NXPOPULARCARD_H

#include <QPixmap>
#include <QWidget>

#include "NXProperty.h"
class NXPopularCardPrivate;

class NX_EXPORT NXPopularCard : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXPopularCard)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QPixmap), CardPixmap)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QPixmap), CardFloatPixmap)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Title)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), SubTitle)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), InteractiveTips)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), DetailedText)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), CardButtonText)
  Q_PROPERTY_CREATE_H(QWidget *, CardFloatArea)
  Q_PROPERTY_CREATE_H(int, BorderRadius)

public:
  explicit NXPopularCard(QWidget *parent = nullptr);
  ~NXPopularCard();
  Q_SIGNAL void popularCardClicked();
  Q_SIGNAL void popularCardButtonClicked();

protected:
  bool event(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXPOPULARCARD_H
