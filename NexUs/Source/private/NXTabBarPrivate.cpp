#include "NXTabBarPrivate.h"

#include <QPropertyAnimation>
#include <private/qtabbar_p.h>

#include "DeveloperComponents/NXTabBarStyle.h"
#include "NXTabBar.h"

NXTabBarPrivate::NXTabBarPrivate(QObject *parent)
    : QObject { parent }
    , m_ScrollAnimation { new QPropertyAnimation(this, "ScrollOffset") }
{
  m_ScrollAnimation->setDuration(200);
  m_ScrollAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

NXTabBarPrivate::~NXTabBarPrivate()
{
}

void NXTabBarPrivate::startScrollAnimation()
{
  if (qFuzzyCompare(m_ScrollOffset, m_TargetScrollOffset)) {
    return;
  }
  m_ScrollAnimation->stop();
  m_ScrollAnimation->setStartValue(m_ScrollOffset);
  m_ScrollAnimation->setEndValue(m_TargetScrollOffset);
  m_ScrollAnimation->start();
}

void NXTabBarPrivate::restoreScrollOffset(qreal offset)
{
  Q_Q(NXTabBar);
  // 不超过当前最大偏移
  int maxOffset = qMax(0, m_TabBarPrivate->tabList.size() * m_Style->getTabSize().width() - q->width());
  offset        = qBound(0.0, offset, static_cast<qreal>(maxOffset));
  setTargetScrollOffset(offset);
  setScrollOffset(offset);
  m_TabBarPrivate->scrollOffset = qRound(offset);
  q->update();
}
