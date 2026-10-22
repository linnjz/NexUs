#include "NXEventBus.h"

#include <QVariant>

#include "private/NXEventBusPrivate.h"
Q_PROPERTY_CREATE_CPP(NXEvent, QS_SET_CREF(QString), EventName)
Q_PROPERTY_CREATE_CPP(NXEvent, QS_SET_CREF(QString), FunctionName)
Q_PROPERTY_CREATE_CPP(NXEvent, Qt::ConnectionType, ConnectionType)

NXEvent::NXEvent(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXEventPrivate())
{
  Q_D(NXEvent);
  d->q_ptr            = this;
  d->_pConnectionType = Qt::AutoConnection;
  d->_pFunctionName   = QStringLiteral("");
  d->_pEventName      = QStringLiteral("");
}

NXEvent::NXEvent(QString eventName, QString functionName, QObject *parent)
    : QObject { parent }
    , d_ptr(new NXEventPrivate())
{
  Q_D(NXEvent);
  d->q_ptr            = this;
  d->_pConnectionType = Qt::AutoConnection;
  d->_pEventName      = eventName;
  d->_pFunctionName   = functionName;
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

NXEventBusType::EventBusReturnType NXEventBus::post(const QString &eventName, const QVariantMap &data)
{
  Q_D(NXEventBus);
  if (eventName.isEmpty()) {
    return NXEventBusType::EventBusReturnType::EventNameInvalid;
  }
  if (d->_eventMap.contains(eventName)) {
    QList<NXEvent *> eventList = d->_eventMap.value(eventName);
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
  Q_D(const NXEventBus);
  if (d->_eventMap.count() == 0) {
    return QStringList();
  }
  QStringList eventsNameList = d->_eventMap.keys();
  return eventsNameList;
}
