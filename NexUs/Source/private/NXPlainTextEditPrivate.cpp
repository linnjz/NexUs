#include "NXPlainTextEditPrivate.h"

#include "NXApplication.h"
#include "NXPlainTextEdit.h"
#include "NXTheme.h"

NXPlainTextEditPrivate::NXPlainTextEditPrivate(QObject *parent)
    : QObject { parent }
{
}

NXPlainTextEditPrivate::~NXPlainTextEditPrivate()
{
}

void NXPlainTextEditPrivate::onWMWindowClickedEvent(const QVariantMap &data)
{
  Q_Q(NXPlainTextEdit);
  NXAppBarType::WMMouseActionType actionType =
      data.value(QStringLiteral("WMClickType")).value<NXAppBarType::WMMouseActionType>();
  if (actionType == NXAppBarType::WMLBUTTONDOWN) {
    if (!q->toPlainText().isEmpty() && q->hasFocus()) {
      q->clearFocus();
    }
  } else if (actionType == NXAppBarType::WMLBUTTONUP || actionType == NXAppBarType::WMNCLBUTTONDOWN) {
    if (NXApplication::containsCursorToItem(q) ||
        (actionType == NXAppBarType::WMLBUTTONUP && !q->toPlainText().isEmpty())) {
      return;
    }
    if (q->hasFocus()) {
      q->clearFocus();
    }
  }
}

void NXPlainTextEditPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXPlainTextEdit);
  _themeMode       = themeMode;
  QPalette palette = q->palette();
  palette.setColor(QPalette::Text, NXThemeColor(_themeMode, BasicText));
  palette.setColor(QPalette::PlaceholderText, _themeMode == NXThemeType::Light ? QColor(0x00, 0x00, 0x00, 128) : QColor(0xBA, 0xBA, 0xBA));
  q->setPalette(palette);
}
