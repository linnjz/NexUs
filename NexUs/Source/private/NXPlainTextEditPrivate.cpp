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

void NXPlainTextEditPrivate::onWMWindowClickedEvent(QVariantMap const &data)
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
  m_ThemeMode       = themeMode;
  QPalette palette = q->palette();
  palette.setColor(QPalette::Text, NXThemeColor(m_ThemeMode, BasicText));
  palette.setColor(QPalette::PlaceholderText, m_ThemeMode == NXThemeType::Light ? QColor(0x00, 0x00, 0x00, 128) : QColor(0xBA, 0xBA, 0xBA));
  q->setPalette(palette);
}
