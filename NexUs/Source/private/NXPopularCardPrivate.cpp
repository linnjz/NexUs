#include "NXPopularCardPrivate.h"

#include <QTimer>

#include "DeveloperComponents/NXPopularCardFloater.h"
#include "NXPopularCard.h"

NXPopularCardPrivate::NXPopularCardPrivate(QObject *parent)
    : QObject { parent }
{
}

NXPopularCardPrivate::~NXPopularCardPrivate()
{
}

void NXPopularCardPrivate::_showFloater()
{
  Q_Q(NXPopularCard);
  q->update();
  _isFloating    = true;
  _pHoverYOffset = 0;
  _floatTimer->stop();
  _floater->showFloater();
}
