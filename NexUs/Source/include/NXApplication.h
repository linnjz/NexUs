#ifndef NXAPPLICATION_H
#define NXAPPLICATION_H

#include <QIcon>
#include <QObject>

#include "LinnSingleton.h"
#include "NXDef.h"

#define nxApp NXApplication::getInstance()
class NXApplicationPrivate;

class NX_EXPORT NXApplication : public QObject
{
  Q_OBJECT
  Q_Q_CREATE(NXApplication)
  Q_SINGLETON_CREATE(QSS_UNIQUE(NXApplication))
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), NXMicaImagePath)
  Q_PROPERTY_CREATE_H(NXApplicationType::WindowDisplayMode, WindowDisplayMode)
  Q_PROPERTY_CREATE_H(int, FontPixelSize)

private:
  explicit NXApplication(QObject *parent = nullptr);
  ~NXApplication();

public:
  void init();
  void syncWindowDisplayMode(QWidget *widget, bool isSync = true);
  static bool containsCursorToItem(QWidget *item);
};

#endif // NXAPPLICATION_H
