#ifndef NXDXGIMANAGERPRIVATE_H
#define NXDXGIMANAGERPRIVATE_H
#include <QObject>
#ifdef Q_OS_WIN
#  include <QImage>
#  include "NXProperty.h"
class NXDxgi;
class NXDxgiManager;

class NXDxgiManagerPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXDxgiManager)

public:
  explicit NXDxgiManagerPrivate(QObject *parent = nullptr);
  ~NXDxgiManagerPrivate();

private:
  Q_SIGNAL void grabScreen();
  bool m_IsAllowedGrabScreen { false };
  NXDxgi *m_Dxgi { nullptr };
  QThread *m_DxgiThread { nullptr };
};
#endif // Q_OS_WIN
#endif // NXDXGIMANAGERPRIVATE_H
