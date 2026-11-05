#include "NXLogPrivate.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QMutex>
#ifndef QT_NO_DEBUG
#  include <iostream>
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#  include <QTextStream>
#endif
#include "NXLog.h"
Q_GLOBAL_STATIC(QMutex, messageLogMutex)
Q_GLOBAL_STATIC(QString, logFileNameTime)

NXLogPrivate::NXLogPrivate(QObject *parent)
    : QObject { parent }
{
}

NXLogPrivate::~NXLogPrivate()
{
}

void NXLogPrivate::_messageLogHander(QtMsgType type, QMessageLogContext const &ctx, QString const &msg)
{
  QString logInfo;
  QString logTime = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
  switch (type) {
  case QtDebugMsg : {
    logInfo = QString(QStringLiteral("[信息-%1](函数: %2 , 行数: %3) -> %4"))
                  .arg(logTime, ctx.function, QString::number(ctx.line), msg);
    break;
  }
  case QtWarningMsg : {
    logInfo = QString(QStringLiteral("[警告-%1](函数: %2 , 行数: %3) -> %4"))
                  .arg(logTime, ctx.function, QString::number(ctx.line), msg);
    break;
  }
  case QtCriticalMsg : {
    logInfo = QString(QStringLiteral("[错误-%1](函数: %2 , 行数: %3) -> %4"))
                  .arg(logTime, ctx.function, QString::number(ctx.line), msg);
    break;
  }
  case QtInfoMsg : {
    logInfo = QString(QStringLiteral("[提示-%1](函数: %2 , 行数: %3) -> %4"))
                  .arg(logTime, ctx.function, QString::number(ctx.line), msg);
    break;
  }
  case QtFatalMsg : {
    logInfo = QString(QStringLiteral("[致命-%1](函数: %2 , 行数: %3) -> %4"))
                  .arg(logTime, ctx.function, QString::number(ctx.line), msg);
    break;
  }
  default : {
    return;
  }
  }
  qDebug() << logInfo;
  NXLog *log = NXLog::getInstance();
  Q_EMIT log->logMessage(logInfo);
  messageLogMutex->lock();
  QFile logfile;
  if (log->getIsLogFileNameWithTime()) {
    logfile.setFileName(log->getLogSavePath() + QStringLiteral("\\") + log->getLogFileName() + *logFileNameTime + QStringLiteral(".txt"));
  } else {
    logfile.setFileName(log->getLogSavePath() + QStringLiteral("\\") + log->getLogFileName() + QStringLiteral(".txt"));
  }
  if (logfile.open(QIODevice::WriteOnly | QIODevice::Append)) {
    QTextStream logFileStream(&logfile);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    logFileStream << logInfo << Qt::endl;
#else
    logFileStream << logInfo << endl;
#endif
    logfile.close();
  }
  messageLogMutex->unlock();
}

void NXLogPrivate::_clearLogFile()
{
  if (m_IsLogFileNameWithTime) {
    QString logTime = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh-mm-ss"));
    logTime.prepend(QStringLiteral("_"));
    logTime.replace(QStringLiteral(" "), QStringLiteral("_"));
    logFileNameTime->clear();
    logFileNameTime->append(logTime);
  } else {
    QFile file(m_LogSavePath + QStringLiteral("\\") + m_LogFileName + QStringLiteral(".txt"));
    if (file.exists()) {
      if (file.open(QIODevice::WriteOnly | QIODevice::Text | QFile::Truncate)) {
        file.close();
      }
    }
  }
}
