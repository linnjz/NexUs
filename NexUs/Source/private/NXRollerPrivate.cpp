#include "NXRollerPrivate.h"

#include <QPropertyAnimation>

NXRollerPrivate::NXRollerPrivate(QObject *parent)
    : QObject(parent)
{
}

NXRollerPrivate::~NXRollerPrivate()
{
}

void NXRollerPrivate::_scroll(int delta)
{
  int steps = delta / 120;
  m_TargetScrollOffset -= steps * m_ItemHeight;
  m_TargetScrollOffset = qRound(m_TargetScrollOffset / m_ItemHeight) * m_ItemHeight;
  if (!m_IsEnableLoop) {
    if (m_TargetScrollOffset < 0) {
      m_TargetScrollOffset = 0;
    }
    if (m_TargetScrollOffset > (m_ItemList.size() - 1) * m_ItemHeight) {
      m_TargetScrollOffset = (m_ItemList.size() - 1) * m_ItemHeight;
    }
  }
  m_ScrollAnimation->stop();
  m_ScrollAnimation->setStartValue(m_ScrollOffset);
  m_ScrollAnimation->setEndValue(m_TargetScrollOffset);
  m_ScrollAnimation->start();
}
