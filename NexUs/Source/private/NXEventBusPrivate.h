#ifndef NXEVENTBUSPRIVATE_H
#define NXEVENTBUSPRIVATE_H

#include <QMap>
#include <QObject>

#include "NXDef.h"
#include "NXProperty.h"
class NXEvent;

class NXEventPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXEvent)
  Q_PROPERTY_CREATE_D(QString, EventName)
  Q_PROPERTY_CREATE_D(QString, FunctionName)
  Q_PROPERTY_CREATE_D(Qt::ConnectionType, ConnectionType)

public:
  explicit NXEventPrivate(QObject *parent = nullptr);
  ~NXEventPrivate();
};

class NXEventBus;

class NXEventBusPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXEventBus)

public:
  explicit NXEventBusPrivate(QObject *parent = nullptr);
  ~NXEventBusPrivate();
  NXEventBusType::EventBusReturnType registerEvent(NXEvent *event);
  void unRegisterEvent(NXEvent *event);

private:
  QMap<QString, QList<NXEvent *>> m_EventMap;
};

#endif // NXEVENTBUSPRIVATE_H
