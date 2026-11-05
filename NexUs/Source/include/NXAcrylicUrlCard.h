#ifndef NXACRYLICURLCARD_H
#define NXACRYLICURLCARD_H

#include <QPixmap>
#include <QPushButton>

#include "NXDef.h"
#include "NXProperty.h"
class NXAcrylicUrlCardPrivate;

class NX_EXPORT NXAcrylicUrlCard : public QPushButton
{
  Q_OBJECT
  Q_Q_CREATE(NXAcrylicUrlCard);
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Title)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), SubTitle)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Url)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QPixmap), CardPixmap)
  Q_PROPERTY_CREATE_H(qreal, MainOpacity)
  Q_PROPERTY_CREATE_H(qreal, NoiseOpacity)
  Q_PROPERTY_CREATE_H(QSize, CardPixmapSize)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, BrushAlpha)
  Q_PROPERTY_CREATE_H(int, TitlePixelSize)
  Q_PROPERTY_CREATE_H(int, SubTitlePixelSize)
  Q_PROPERTY_CREATE_H(int, TitleSpacing)
  Q_PROPERTY_CREATE_H(int, SubTitleSpacing)
  Q_PROPERTY_CREATE_H(int, CardPixmapBorderRadius)
  Q_PROPERTY_CREATE_H(NXCardPixType::PixMode, CardPixMode)

public:
  explicit NXAcrylicUrlCard(QWidget *parent = nullptr);
  ~NXAcrylicUrlCard();
  void setCardPixmapSize(int width, int height);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXACRYLICURLCARD_H
