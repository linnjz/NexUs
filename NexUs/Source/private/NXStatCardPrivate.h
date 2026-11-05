#ifndef NXSTATCARDPRIVATE_H
#define NXSTATCARDPRIVATE_H

#include <QObject>

#include "NXDef.h"
#include "NXProperty.h"

class NXStatCard;

class NXStatCardPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXStatCard)
  Q_PROPERTY_CREATE_D(QString, Title)
  Q_PROPERTY_CREATE_D(QString, Value)
  Q_PROPERTY_CREATE_D(QString, Description)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(NXIconType::IconName, CardIcon)

public:
  explicit NXStatCardPrivate(QObject *parent = nullptr);
  ~NXStatCardPrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
  NXStatCard::TrendType m_Trend { NXStatCard::None };
  QString m_TrendText;
};

#endif // NXSTATCARDPRIVATE_H
