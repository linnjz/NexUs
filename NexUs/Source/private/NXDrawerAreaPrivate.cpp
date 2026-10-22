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
  _drawerContainer->doDrawerAnimation(isExpand);
  Q_EMIT q->expandStateChanged(isExpand);
}
