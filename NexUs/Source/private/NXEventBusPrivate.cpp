#include "NXEventBusPrivate.h"

#include "NXEventBus.h"

NXEventPrivate::NXEventPrivate(QObject *parent)
    : QObject { parent }
{
}

NXEventPrivate::~NXEventPrivate()
{
}

NXEventBusPrivate::NXEventBusPrivate(QObject *parent)
    : QObject { parent }
{
}

NXEventBusPrivate::~NXEventBusPrivate()
{
}

NXEventBusType::EventBusReturnType NXEventBusPrivate::registerEvent(NXEvent *event)
{
  if (!event) {
    return NXEventBusType::EventBusReturnType::EventInvalid;
  }
  if (event->getEventName().isEmpty()) {
    return NXEventBusType::EventBusReturnType::EventNameInvalid;
  }
  if (m_EventMap.contains(event->getEventName())) {
    QList<NXEvent *> eventList = m_EventMap.value(event->getEventName());
    if (eventList.contains(event)) {
      return NXEventBusType::EventBusReturnType::EventInvalid;
    }
    eventList.append(event);
    m_EventMap[event->getEventName()] = eventList;
  } else {
    QList<NXEvent *> eventList;
    eventList.append(event);
    m_EventMap.insert(event->getEventName(), eventList);
  }
  return NXEventBusType::EventBusReturnType::Success;
}

void NXEventBusPrivate::unRegisterEvent(NXEvent *event)
{
  if (!event) {
    return;
  }
  if (event->getEventName().isEmpty()) {
    return;
  }
  if (m_EventMap.contains(event->getEventName())) {
    if (m_EventMap[event->getEventName()].count() == 1) {
      m_EventMap.remove(event->getEventName());
    } else {
      QList<NXEvent *> eventList = m_EventMap.value(event->getEventName());
      eventList.removeOne(event);
      m_EventMap[event->getEventName()] = eventList;
    }
  }
}
