#include "NXSuggestBoxSearchViewContainer.h"

#include <QPainter>

#include "NXTheme.h"

NXSuggestBoxSearchViewContainer::NXSuggestBoxSearchViewContainer(QWidget *parent)
    : QWidget { parent }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  setContentsMargins(2, 2, 2, 2);
#else
  setContentsMargins(8, 8, 8, 8);
#endif
  setObjectName("NXSuggestBoxSearchViewBaseWidget");
  setStyleSheet(QStringLiteral("#NXSuggestBoxSearchViewBaseWidget{background-color:transparent}"));

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXSuggestBoxSearchViewContainer::~NXSuggestBoxSearchViewContainer()
{
}

void NXSuggestBoxSearchViewContainer::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  QRect foregroundRect = rect();
#else
  nxTheme->drawEffectShadow(&painter, rect(), 6, 8);
  QRect foregroundRect(6, 0, rect().width() - 2 * 6, rect().height() - 6);
#endif
  painter.setPen(NXThemeColor(m_ThemeMode, PopupBorder));
  painter.setBrush(NXThemeColor(m_ThemeMode, PopupBase));
  painter.drawRoundedRect(foregroundRect, 8, 8);
  painter.restore();
}
