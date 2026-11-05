#ifndef NXQRCODEPRIVATE_H
#define NXQRCODEPRIVATE_H

#include <QObject>
#include <QVector>

#include "NXDef.h"

class NXQRCode;

class NXQRCodePrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXQRCode)
  Q_PROPERTY_CREATE_D(QColor, ForegroundColor)
  Q_PROPERTY_CREATE_D(QColor, BackgroundColor)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, QuietZone)

public:
  int m_ModuleCount { 0 };
  int m_EcLevel { 1 }; // 0=L, 1=M, 2=Q, 3=H
  NXThemeType::ThemeMode m_ThemeMode;
  QString m_Text;
  explicit NXQRCodePrivate(QObject *parent = nullptr);
  ~NXQRCodePrivate();

  void encode(QString const &text);

  QVector<QVector<bool>> m_Modules;
};

#endif // NXQRCODEPRIVATE_H
