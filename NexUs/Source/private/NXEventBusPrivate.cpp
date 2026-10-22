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
  if (_eventMap.contains(event->getEventName())) {
    QList<NXEvent *> eventList = _eventMap.value(event->getEventName());
    if (eventList.contains(event)) {
      return NXEventBusType::EventBusReturnType::EventInvalid;
    }
    eventList.append(event);
    _eventMap[event->getEventName()] = eventList;
  } else {
    QList<NXEvent *> eventList;
    eventList.append(event);
    _eventMap.insert(event->getEventName(), eventList);
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
  if (_eventMap.contains(event->getEventName())) {
    if (_eventMap[event->getEventName()].count() == 1) {
      _eventMap.remove(event->getEventName());
    } else {
      QList<NXEvent *> eventList = _eventMap.value(event->getEventName());
      eventList.removeOne(event);
      _eventMap[event->getEventName()] = eventList;
    }
  }
}
