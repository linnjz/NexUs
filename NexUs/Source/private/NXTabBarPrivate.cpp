#include "NXTabBarPrivate.h"

#include <QPropertyAnimation>
#include <private/qtabbar_p.h>

#include "DeveloperComponents/NXTabBarStyle.h"
#include "NXTabBar.h"

NXTabBarPrivate::NXTabBarPrivate(QObject *parent)
    : QObject { parent }
{
  _scrollAnimation = new QPropertyAnimation(this, "pScrollOffset");
  _scrollAnimation->setDuration(200);
  _scrollAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

NXTabBarPrivate::~NXTabBarPrivate()
{
}

void NXTabBarPrivate::startScrollAnimation()
{
  if (qFuzzyCompare(_pScrollOffset, _pTargetScrollOffset)) {
    return;
  }
  _scrollAnimation->stop();
  _scrollAnimation->setStartValue(_pScrollOffset);
  _scrollAnimation->setEndValue(_pTargetScrollOffset);
  _scrollAnimation->start();
}

void NXTabBarPrivate::restoreScrollOffset(qreal offset)
{
  Q_Q(NXTabBar);
  // 不超过当前最大偏移
  int maxOffset = qMax(0, _tabBarPrivate->tabList.size() * _style->getTabSize().width() - q->width());
  offset        = qBound(0.0, offset, static_cast<qreal>(maxOffset));
  setTargetScrollOffset(offset);
  setScrollOffset(offset);
  _tabBarPrivate->scrollOffset = qRound(offset);
  q->update();
}
