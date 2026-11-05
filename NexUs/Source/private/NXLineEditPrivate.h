#ifndef NXLINEEDITPRIVATE_H
#define NXLINEEDITPRIVATE_H

#include <QMargins>
#include <QObject>
#include <QVariantMap>

#include "NXDef.h"
class NXEvent;
class NXLineEdit;
class NXLineEditStyle;

class NXLineEditPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXLineEdit)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ExpandMarkWidth)
  Q_PROPERTY_CREATE_D(bool, IsClearButtonEnable)

public:
  explicit NXLineEditPrivate(QObject *parent = nullptr);
  ~NXLineEditPrivate();
  Q_INVOKABLE void onWMWindowClickedEvent(QVariantMap const &data);
  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  NXThemeType::ThemeMode m_ThemeMode;
  qreal m_TextSpacing { 0.5 };
  NXEvent *m_FocusEvent { nullptr };
  NXLineEditStyle *m_LineEditStyle { nullptr };
};

#endif // NXLINEEDITPRIVATE_H
