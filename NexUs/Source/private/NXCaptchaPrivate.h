#ifndef NXCAPTCHAPRIVATE_H
#define NXCAPTCHAPRIVATE_H

#include <QList>
#include <QObject>

#include "NXDef.h"

class QLineEdit;
class QHBoxLayout;
class NXCaptcha;

class NXCaptchaPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXCaptcha)
  Q_PROPERTY_CREATE_D(int, CodeLength)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, BoxSize)
  Q_PROPERTY_CREATE_D(int, BoxSpacing)

public:
  explicit NXCaptchaPrivate(QObject *parent = nullptr);
  ~NXCaptchaPrivate();
  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_InputMode { 0 }; // 0=DigitOnly, 1=AlphaNumeric
  QList<QLineEdit *> m_Boxes;
  QHBoxLayout *m_BoxLayout { nullptr };
  void _buildBoxes();
  void _updateValidators();
  void _applyThemeStyle();
  void _applyBoxStyle(QLineEdit *box, bool focused);
  void _onTextChanged(QLineEdit *box, int index);
};

#endif // NXCAPTCHAPRIVATE_H
