#include "NXSkeleton.h"

#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QVariantAnimation>

#include "NXTheme.h"
#include "private/NXSkeletonPrivate.h"

Q_PROPERTY_CREATE_CPP(NXSkeleton, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSkeleton, QPS_PAEMIT(bool), IsAnimated)

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
  if (m_ShimmerAnimation) {
    m_ShimmerAnimation->stop();
    delete m_ShimmerAnimation;
    m_ShimmerAnimation = nullptr;
  }
  m_ShimmerAnimation = new QVariantAnimation(this);
  m_ShimmerAnimation->setStartValue(0.0);
  m_ShimmerAnimation->setEndValue(1.0);
  m_ShimmerAnimation->setDuration(1500);
  m_ShimmerAnimation->setLoopCount(-1);
  connect(m_ShimmerAnimation, &QVariantAnimation::valueChanged, this, [=](QVariant const &value) {
    m_ShimmerPosition = value.toReal();
    q->update();
  });
  m_ShimmerAnimation->start();
}

void NXSkeletonPrivate::_stopShimmerAnimation()
{
  if (m_ShimmerAnimation) {
    m_ShimmerAnimation->stop();
    delete m_ShimmerAnimation;
    m_ShimmerAnimation = nullptr;
  }
  m_ShimmerPosition = 0.0;
}

NXSkeleton::NXSkeleton(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXSkeletonPrivate())
{
  Q_D(NXSkeleton);
  d->q_ptr             = this;
  d->m_BorderRadius    = 4;
  d->m_IsAnimated      = true;
  d->m_ShimmerPosition = 0.0;
  setObjectName("NXSkeleton");
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
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
  d->m_SkeletonType = type;
  if (type == Circle) {
    int diameter = width();
    setFixedSize(diameter, diameter);
  }
  update();
}

NXSkeleton::SkeletonType NXSkeleton::getSkeletonType() const
{
  return d_ptr->m_SkeletonType;
}

void NXSkeleton::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXSkeleton);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);

  QColor baseColor = NXThemeColor(d->m_ThemeMode, BasicDisable);

  if (d->m_IsAnimated) {
    QLinearGradient gradient(0, 0, width(), 0);
    QColor lighterColor = baseColor.lighter(120);
    qreal pos           = d->m_ShimmerPosition;
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

  switch (d->m_SkeletonType) {
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
    path.addRoundedRect(QRectF(0, 0, width(), height()), d->m_BorderRadius, d->m_BorderRadius);
    painter.drawPath(path);
    break;
  }
  }
}

QSize NXSkeleton::sizeHint() const
{
  Q_CONST_D(NXSkeleton);
  switch (d->m_SkeletonType) {
  case Text      : return QSize(200, 16);
  case Circle    : return QSize(40, 40);
  case Rectangle : return QSize(200, 100);
  }
  return QSize(200, 16);
}
