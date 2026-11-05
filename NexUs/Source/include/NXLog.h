#ifndef NXLOG_H
#define NXLOG_H

#include <QObject>

#include "LinnSingleton.h"
#include "NXProperty.h"

class NXLogPrivate;

class NX_EXPORT NXLog : public QObject
{
  Q_OBJECT
  Q_Q_CREATE(NXLog)
  Q_SINGLETON_CREATE(QSS_UNIQUE(NXLog))
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), LogSavePath)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), LogFileName)
  Q_PROPERTY_CREATE_H(bool, IsLogFileNameWithTime)

private:
  explicit NXLog(QObject *parent = nullptr);
  ~NXLog();

public:
  void initMessageLog(bool isEnable);
  Q_SIGNAL void logMessage(QString const &log);
};

#endif // NXLOG_H
