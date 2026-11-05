#ifndef NXPLAINTEXTEDITPRIVATE_H
#define NXPLAINTEXTEDITPRIVATE_H

#include <QObject>
#include <QVariantMap>

#include "NXDef.h"
class NXEvent;
class NXPlainTextEdit;
class NXPlainTextEditStyle;

class NXPlainTextEditPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXPlainTextEdit)

public:
  explicit NXPlainTextEditPrivate(QObject *parent = nullptr);
  ~NXPlainTextEditPrivate();
  Q_INVOKABLE void onWMWindowClickedEvent(QVariantMap const &data);
  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  NXThemeType::ThemeMode m_ThemeMode;
  NXPlainTextEditStyle *m_Style { nullptr };
  NXEvent *m_FocusEvent { nullptr };
};

#endif // NXPLAINTEXTEDITPRIVATE_H
