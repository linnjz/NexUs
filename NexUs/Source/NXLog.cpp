#include "NXLog.h"

#include <QDir>

#include "private/NXLogPrivate.h"

Q_PROPERTY_CREATE_CPP(NXLog, QPS_SCREF_PAEMIT(QString), LogSavePath)
Q_PROPERTY_CREATE_CPP(NXLog, QPS_SCREF_PAEMIT(QString), LogFileName)
Q_PROPERTY_CREATE_CPP(NXLog, QPS_PAEMIT(bool), IsLogFileNameWithTime)

NXLog::NXLog(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXLogPrivate())
{
  Q_D(NXLog);
  d->q_ptr                   = this;
  d->m_LogFileName           = QStringLiteral("NXLog");
  d->m_LogSavePath           = QDir::currentPath();
  d->m_IsLogFileNameWithTime = false;
  d->_clearLogFile();
  connect(this, &NXLog::signalLogSavePathChanged, d, &NXLogPrivate::_clearLogFile);
  connect(this, &NXLog::signalLogFileNameChanged, d, &NXLogPrivate::_clearLogFile);
  connect(this, &NXLog::signalIsLogFileNameWithTimeChanged, d, &NXLogPrivate::_clearLogFile);
}

NXLog::~NXLog()
{
}

void NXLog::initMessageLog(bool isEnable)
{
  Q_D(NXLog);
  qInstallMessageHandler(isEnable ? d->_messageLogHander : 0);
}
