#include "NXComboBoxPrivate.h"

#include <QEvent>
#include <QLineEdit>

#include "NXComboBox.h"
#include "NXTheme.h"

NXComboBoxPrivate::NXComboBoxPrivate(QObject *parent)
    : QObject { parent }
{
}

bool NXComboBoxPrivate::eventFilter(QObject *watched, QEvent *event)
{
  if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
    _isKeyEvent = true;
  } else if (event->type() == QEvent::MouseMove || event->type() == QEvent::HoverMove) {
    _isKeyEvent = false;
  } else if (event->type() == QEvent::Hide) {
    Q_Q(NXComboBox);
    if (watched == q->findChild<QFrame *>()) {
      q->_resetIndicatorAnimations();
    }
  }
  return QObject::eventFilter(watched, event);
}

NXComboBoxPrivate::~NXComboBoxPrivate()
{
}

void NXComboBoxPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXComboBox);
  _themeMode    = themeMode;
  auto lineEdit = q->lineEdit();
  if (lineEdit) {
    QPalette palette = lineEdit->palette();
    palette.setColor(QPalette::Text, NXThemeColor(_themeMode, BasicText));
    palette.setColor(QPalette::PlaceholderText, _themeMode == NXThemeType::Light ? QColor(0x00, 0x00, 0x00, 128) : QColor(0xBA, 0xBA, 0xBA));
    lineEdit->setPalette(palette);
  }
}
