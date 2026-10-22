#include "NXSkeleton.h"

#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QVariantAnimation>

#include "NXTheme.h"
#include "private/NXSkeletonPrivate.h"

Q_PROPERTY_CREATE_CPP(NXSkeleton, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSkeleton, bool, IsAnimated)

NXSkeletonPrivate::NXSkeletonPrivate(QObject *parent)
    : QObject(parent)
{
}

NXSkeletonPrivate::~NXSkeletonPrivate()
{
}

void NXSkeletonPrivate::_startShimmerAnimation()
{
  Q_Q(NXSkeleton);
  if (_shimmerAnimation) {
    _shimmerAnimation->stop();
    delete _shimmerAnimation;
    _shimmerAnimation = nullptr;
  }
  _shimmerAnimation = new QVariantAnimation(this);
  _shimmerAnimation->setStartValue(0.0);
  _shimmerAnimation->setEndValue(1.0);
  _shimmerAnimation->setDuration(1500);
  _shimmerAnimation->setLoopCount(-1);
  connect(_shimmerAnimation, &QVariantAnimation::valueChanged, this, [=](const QVariant &value) {
    _pShimmerPosition = value.toReal();
    q->update();
  });
  _shimmerAnimation->start();
}

void NXSkeletonPrivate::_stopShimmerAnimation()
{
  if (_shimmerAnimation) {
    _shimmerAnimation->stop();
    delete _shimmerAnimation;
    _shimmerAnimation = nullptr;
  }
  _pShimmerPosition = 0.0;
}

NXSkeleton::NXSkeleton(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXSkeletonPrivate())
{
  Q_D(NXSkeleton);
  d->q_ptr             = this;
  d->_pBorderRadius    = 4;
  d->_pIsAnimated      = true;
  d->_pShimmerPosition = 0.0;
  setObjectName("NXSkeleton");
  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
  d->_startShimmerAnimation();
}

NXSkeleton::~NXSkeleton()
{
  Q_D(NXSkeleton);
  d->_stopShimmerAnimation();
}

void NXSkeleton::setSkeletonType(SkeletonType type)
{
  Q_D(NXSkeleton);
  d->_skeletonType = type;
  if (type == Circle) {
    int diameter = width();
    setFixedSize(diameter, diameter);
  }
  update();
}

NXSkeleton::SkeletonType NXSkeleton::getSkeletonType() const
{
  return d_ptr->_skeletonType;
}

void NXSkeleton::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXSkeleton);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);

  QColor baseColor = NXThemeColor(d->_themeMode, BasicDisable);

  if (d->_pIsAnimated) {
    QLinearGradient gradient(0, 0, width(), 0);
    QColor lighterColor = baseColor.lighter(120);
    qreal pos           = d->_pShimmerPosition;
    qreal shimmerWidth  = 0.3;
    qreal start         = pos - shimmerWidth;
    qreal end           = pos + shimmerWidth;

    if (start < 0.0) {
      gradient.setColorAt(0.0, lighterColor);
      gradient.setColorAt(qMin(end, 1.0), baseColor);
      if (end < 1.0) {
        gradient.setColorAt(1.0, baseColor);
      }
    } else if (end > 1.0) {
      gradient.setColorAt(0.0, baseColor);
      gradient.setColorAt(start, baseColor);
      gradient.setColorAt(1.0, lighterColor);
    } else {
      gradient.setColorAt(0.0, baseColor);
      gradient.setColorAt(start, baseColor);
      gradient.setColorAt(pos, lighterColor);
      gradient.setColorAt(end, baseColor);
      gradient.setColorAt(1.0, baseColor);
    }
    painter.setBrush(gradient);
  } else {
    painter.setBrush(baseColor);
  }

  switch (d->_skeletonType) {
  case Text : {
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, width(), height()), 4, 4);
    painter.drawPath(path);
    break;
  }
  case Circle : {
    painter.drawEllipse(QRectF(0, 0, width(), height()));
    break;
  }
  case Rectangle : {
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, width(), height()), d->_pBorderRadius, d->_pBorderRadius);
    painter.drawPath(path);
    break;
  }
  }
}

QSize NXSkeleton::sizeHint() const
{
  Q_D(const NXSkeleton);
  switch (d->_skeletonType) {
  case Text      : return QSize(200, 16);
  case Circle    : return QSize(40, 40);
  case Rectangle : return QSize(200, 100);
  }
  return QSize(200, 16);
}
