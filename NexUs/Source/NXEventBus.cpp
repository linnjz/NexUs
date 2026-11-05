#include "NXEventBus.h"

#include <QVariant>

#include "private/NXEventBusPrivate.h"
Q_PROPERTY_CREATE_CPP(NXEvent, QPS_SCREF_PAEMIT(QString), EventName)
Q_PROPERTY_CREATE_CPP(NXEvent, QPS_SCREF_PAEMIT(QString), FunctionName)
Q_PROPERTY_CREATE_CPP(NXEvent, QPS_PAEMIT(Qt::ConnectionType), ConnectionType)

NXEvent::NXEvent(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXEventPrivate())
{
  Q_D(NXEvent);
  d->q_ptr            = this;
  d->m_ConnectionType = Qt::AutoConnection;
  d->m_FunctionName   = QStringLiteral("");
  d->m_EventName      = QStringLiteral("");
}

NXEvent::NXEvent(QString eventName, QString functionName, QObject *parent)
    : QObject { parent }
    , d_ptr(new NXEventPrivate())
{
  Q_D(NXEvent);
  d->q_ptr            = this;
  d->m_ConnectionType = Qt::AutoConnection;
  d->m_EventName      = eventName;
  d->m_FunctionName   = functionName;
}

NXEventBusType::EventBusReturnType NXEvent::registerAndInit()
{
  return NXEventBus::getInstance()->d_ptr->registerEvent(this);
}

NXEvent::~NXEvent()
{
  NXEventBus::getInstance()->d_ptr->unRegisterEvent(this);
}

NXEventBus::NXEventBus(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXEventBusPrivate())
{
  Q_D(NXEventBus);
  d->q_ptr = this;
}

NXEventBus::~NXEventBus()
{
}

NXEventBusType::EventBusReturnType NXEventBus::post(QString const &eventName, QVariantMap const &data)
{
  Q_D(NXEventBus);
  if (eventName.isEmpty()) {
    return NXEventBusType::EventBusReturnType::EventNameInvalid;
  }
  if (d->m_EventMap.contains(eventName)) {
    QList<NXEvent *> eventList = d->m_EventMap.value(eventName);
    foreach(auto event, eventList)
    {
      if (event->parent()) {
        QMetaObject::invokeMethod(event->parent(), event->getFunctionName().toLocal8Bit().constData(), event->getConnectionType(), Q_ARG(QVariantMap, data));
      }
    }
  }
  return NXEventBusType::EventBusReturnType::Success;
}

QStringList NXEventBus::getRegisteredEventsName() const
{
  Q_CONST_D(NXEventBus);
  if (d->m_EventMap.count() == 0) {
    return QStringList();
  }
  QStringList eventsNameList = d->m_EventMap.keys();
  return eventsNameList;
}
