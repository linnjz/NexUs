#include "NXPromotionCardPrivate.h"

#include <QPoint>
#include <QPropertyAnimation>
#include <QtMath>

#include "NXPromotionCard.h"

NXPromotionCardPrivate::NXPromotionCardPrivate(QObject *parent)
    : QObject { parent }
{
}

NXPromotionCardPrivate::~NXPromotionCardPrivate()
{
}

qreal NXPromotionCardPrivate::_getLongestDistance(QPoint point)
{
  Q_Q(NXPromotionCard);
  qreal topLeftDis     = _distance(point, QPoint(0, 0));
  qreal topRightDis    = _distance(point, QPoint(q->width(), 0));
  qreal bottomLeftDis  = _distance(point, QPoint(0, q->height()));
  qreal bottomRightDis = _distance(point, QPoint(q->width(), q->height()));
  QList<qreal> disList { topLeftDis, topRightDis, bottomLeftDis, bottomRightDis };
  std::sort(disList.begin(), disList.end());
  return disList[3];
}

qreal NXPromotionCardPrivate::_distance(QPoint point1, QPoint point2)
{
  return std::sqrt((point1.x() - point2.x()) * (point1.x() - point2.x()) + (point1.y() - point2.y()) * (point1.y() - point2.y()));
}

void NXPromotionCardPrivate::_startHoverOpacityAnimation(bool isVisible)
{
  Q_Q(NXPromotionCard);
  QPropertyAnimation *opacityAnimation = new QPropertyAnimation(this, "HoverOpacity");
  connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    q->update();
  });
  opacityAnimation->setDuration(250);
  opacityAnimation->setStartValue(m_HoverOpacity);
  opacityAnimation->setEndValue(isVisible ? 1 : 0);
  opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}
