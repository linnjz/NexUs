#ifndef NXEVENTBUS_H
#define NXEVENTBUS_H

#include <QObject>
#include <QVariantMap>

#include "LinnSingleton.h"
#include "NXDef.h"
#include "NXProperty.h"

class NXEventPrivate;

class NX_EXPORT NXEvent : public QObject
{
  Q_OBJECT
  Q_Q_CREATE(NXEvent)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), EventName)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), FunctionName)
  Q_PROPERTY_CREATE_H(Qt::ConnectionType, ConnectionType)

public:
  explicit NXEvent(QObject *parent = nullptr);
  explicit NXEvent(QString eventName, QString functionName, QObject *parent = nullptr);
  ~NXEvent();
  NXEventBusType::EventBusReturnType registerAndInit();
};

class NXEventBusPrivate;

class NX_EXPORT NXEventBus : public QObject
{
  Q_OBJECT
  Q_Q_CREATE(NXEventBus)
  Q_SINGLETON_CREATE(QSS_UNIQUE(NXEventBus))

private:
  explicit NXEventBus(QObject *parent = nullptr);
  ~NXEventBus();

public:
  NXEventBusType::EventBusReturnType post(QString const &eventName, QVariantMap const &data = {});
  QStringList getRegisteredEventsName() const;

private:
  friend class NXEvent;
};

#endif // NXEVENTBUS_H
