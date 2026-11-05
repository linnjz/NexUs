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
  m_IsFloating    = true;
  m_HoverYOffset = 0;
  m_FloatTimer->stop();
  m_Floater->showFloater();
}
