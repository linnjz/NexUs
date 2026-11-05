#ifndef NXMICABASEINITOBJECT_H
#define NXMICABASEINITOBJECT_H

#include <QImage>
#include <QObject>
class NXApplicationPrivate;

class NXMicaBaseInitObject : public QObject
{
  Q_OBJECT

public:
  explicit NXMicaBaseInitObject(NXApplicationPrivate *appPrivate, QObject *parent = nullptr);
  ~NXMicaBaseInitObject();
  Q_SLOT void onInitMicaBase(QImage const &img);
  Q_SIGNAL void initFinished();

private:
  NXApplicationPrivate *m_AppPrivate { nullptr };
};

#endif // NXMICABASEINITOBJECT_H
