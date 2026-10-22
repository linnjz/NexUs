#include "NXScrollArea.h"

#include <QScrollBar>
#include <QScroller>

#include "NXScrollBar.h"
#include "private/NXScrollAreaPrivate.h"

NXScrollArea::NXScrollArea(QWidget *parent)
    : QScrollArea(parent)
    , d_ptr(new NXScrollAreaPrivate())
{
  Q_D(NXScrollArea);
  d->q_ptr = this;
  setObjectName("NXScrollArea");
  setStyleSheet(QStringLiteral("#NXScrollArea{background-color:transparent;border:0px;}"));
  setHorizontalScrollBar(new NXScrollBar(this));
  setVerticalScrollBar(new NXScrollBar(this));
  QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

NXScrollArea::~NXScrollArea()
{
}

void NXScrollArea::setIsGrabGesture(bool isEnable, qreal mousePressEventDelay)
{
  if (isEnable) {
    QScroller::grabGesture(this->viewport(), QScroller::LeftMouseButtonGesture);
    QScrollerProperties properties = QScroller::scroller(this->viewport())->scrollerProperties();
    properties.setScrollMetric(QScrollerProperties::MousePressEventDelay, mousePressEventDelay);
    properties.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor, 0.35);
    properties.setScrollMetric(QScrollerProperties::OvershootScrollTime, 0.5);
    properties.setScrollMetric(QScrollerProperties::FrameRate, QScrollerProperties::Fps60);
    QScroller::scroller(this->viewport())->setScrollerProperties(properties);
  } else {
    QScroller::ungrabGesture(this->viewport());
  }
}

void NXScrollArea::setIsOverShoot(Qt::Orientation orientation, bool isEnable)
{
  QScrollerProperties properties = QScroller::scroller(this->viewport())->scrollerProperties();
  properties.setScrollMetric(orientation == Qt::Horizontal ? QScrollerProperties::HorizontalOvershootPolicy : QScrollerProperties::VerticalOvershootPolicy, isEnable ? QScrollerProperties::OvershootAlwaysOn : QScrollerProperties::OvershootAlwaysOff);
  QScroller::scroller(this->viewport())->setScrollerProperties(properties);
}

bool NXScrollArea::getIsOverShoot(Qt::Orientation orientation) const
{
  QScrollerProperties properties = QScroller::scroller(this->viewport())->scrollerProperties();
  return properties
      .scrollMetric(orientation == Qt::Horizontal ? QScrollerProperties::HorizontalOvershootPolicy : QScrollerProperties::VerticalOvershootPolicy)
      .toBool();
}

void NXScrollArea::setIsAnimation(Qt::Orientation orientation, bool isAnimation)
{
  if (orientation == Qt::Horizontal) {
    dynamic_cast<NXScrollBar *>(this->horizontalScrollBar())->setIsAnimation(isAnimation);
  } else {
    dynamic_cast<NXScrollBar *>(this->verticalScrollBar())->setIsAnimation(isAnimation);
  }
}

bool NXScrollArea::getIsAnimation(Qt::Orientation orientation) const
{
  if (orientation == Qt::Horizontal) {
    return dynamic_cast<NXScrollBar *>(this->horizontalScrollBar())->getIsAnimation();
  } else {
    return dynamic_cast<NXScrollBar *>(this->verticalScrollBar())->getIsAnimation();
  }
}
