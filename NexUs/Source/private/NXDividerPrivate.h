#ifndef NXDIVIDERPRIVATE_H
#define NXDIVIDERPRIVATE_H

#include <QObject>

#include "NXDef.h"
#include "NXDivider.h"

class NXDividerPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXDivider)
  Q_PROPERTY_CREATE_D(QString, Text)
  Q_PROPERTY_CREATE_D(Qt::Orientation, Orientation)
  Q_PROPERTY_CREATE_D(int, ContentPosition)

public:
  explicit NXDividerPrivate(QObject *parent = nullptr);
  ~NXDividerPrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXDIVIDERPRIVATE_H
