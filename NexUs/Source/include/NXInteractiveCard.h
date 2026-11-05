#ifndef NXINTERACTIVECARD_H
#define NXINTERACTIVECARD_H

#include <QPixmap>
#include <QPushButton>

#include "NXDef.h"
#include "NXProperty.h"

class NXInteractiveCardPrivate;

class NX_EXPORT NXInteractiveCard : public QPushButton
{
  Q_OBJECT
  Q_Q_CREATE(NXInteractiveCard)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Title)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), SubTitle)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QPixmap), CardPixmap)
  Q_PROPERTY_CREATE_H(QSize, CardPixmapSize)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, TitlePixelSize)
  Q_PROPERTY_CREATE_H(int, SubTitlePixelSize)
  Q_PROPERTY_CREATE_H(int, TitleSpacing)
  Q_PROPERTY_CREATE_H(int, CardPixmapBorderRadius)
  Q_PROPERTY_CREATE_H(NXCardPixType::PixMode, CardPixMode)

public:
  explicit NXInteractiveCard(QWidget *parent = nullptr);
  ~NXInteractiveCard();
  void setCardPixmapSize(int width, int height);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXINTERACTIVECARD_H
