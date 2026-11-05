#include "NXDrawerAreaPrivate.h"

#include "NXDrawerArea.h"

NXDrawerAreaPrivate::NXDrawerAreaPrivate(QObject *parent)
    : QObject(parent)
{
}

NXDrawerAreaPrivate::~NXDrawerAreaPrivate()
{
}

void NXDrawerAreaPrivate::onDrawerHeaderClicked(bool isExpand)
{
  Q_Q(NXDrawerArea);
  m_DrawerContainer->doDrawerAnimation(isExpand);
  Q_EMIT q->expandStateChanged(isExpand);
}
