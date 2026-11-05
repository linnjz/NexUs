#include "NXLineEditPrivate.h"

#include <QTimer>

#include "NXApplication.h"
#include "NXLineEdit.h"
#include "NXTheme.h"

NXLineEditPrivate::NXLineEditPrivate(QObject *parent)
    : QObject { parent }
{
}

NXLineEditPrivate::~NXLineEditPrivate()
{
}

void NXLineEditPrivate::onWMWindowClickedEvent(QVariantMap const &data)
{
  Q_Q(NXLineEdit);
  NXAppBarType::WMMouseActionType actionType = data.value(QStringLiteral("WMClickType")).value<NXAppBarType::WMMouseActionType>();
  if (actionType == NXAppBarType::WMLBUTTONDOWN) {
    if (NXApplication::containsCursorToItem(q)) {
      return;
    }
    if (q->hasSelectedText() && q->hasFocus()) {
      q->clearFocus();
    }
  } else if (actionType == NXAppBarType::WMLBUTTONUP || actionType == NXAppBarType::WMNCLBUTTONDOWN) {
    if (NXApplication::containsCursorToItem(q) || (actionType == NXAppBarType::WMLBUTTONUP && q->hasSelectedText())) {
      return;
    }
    if (q->hasFocus()) {
      q->clearFocus();
    }
  }
}

void NXLineEditPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXLineEdit);
  m_ThemeMode       = themeMode;
  QPalette palette = q->palette();
  palette.setColor(QPalette::Text, NXThemeColor(m_ThemeMode, BasicText));
  palette.setColor(QPalette::PlaceholderText, m_ThemeMode == NXThemeType::Light ? QColor(0x00, 0x00, 0x00, 128) : QColor(0xBA, 0xBA, 0xBA));
  q->setPalette(palette);
}
