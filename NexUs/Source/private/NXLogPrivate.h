#ifndef NXLOGPRIVATE_H
#define NXLOGPRIVATE_H

#include <QObject>

#include "NXProperty.h"
class NXLog;

class NXLogPrivate : public QObject
{
  Q_OBJECT
  Q_PROPERTY_CREATE_D(QString, LogSavePath)
  Q_PROPERTY_CREATE_D(QString, LogFileName)
  Q_PROPERTY_CREATE_D(bool, IsLogFileNameWithTime)
  Q_D_CREATE(NXLog)

public:
  explicit NXLogPrivate(QObject *parent = nullptr);
  ~NXLogPrivate();

private:
  static void _messageLogHander(QtMsgType type, QMessageLogContext const &ctx, QString const &msg);
  void _clearLogFile();
};

#endif // NXLOGPRIVATE_H
