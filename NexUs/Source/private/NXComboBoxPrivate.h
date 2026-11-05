#ifndef NXCOMBOBOXPRIVATE_H
#define NXCOMBOBOXPRIVATE_H

#include <QObject>

#include "NXDef.h"

class QLineEdit;
class NXComboBox;
class NXComboBoxStyle;

class NXComboBoxPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXComboBox);
  Q_PROPERTY_CREATE_D(int, BorderRadius)

public:
  explicit NXComboBoxPrivate(QObject *parent = nullptr);
  ~NXComboBoxPrivate();

  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  bool m_IsAllowHidePopup { false };
  bool m_IsKeyEvent { false };
  NXThemeType::ThemeMode m_ThemeMode;
  NXComboBoxStyle *m_ComboBoxStyle { nullptr };
};

#endif // NXCOMBOBOXPRIVATE_H
