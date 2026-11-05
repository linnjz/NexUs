#include "NXTheme.h"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QStyleHints>

#include "private/NXThemePrivate.h"

NXTheme::NXTheme(QObject *parent)
    : QObject { parent }
    , d_ptr(new NXThemePrivate())
{
  Q_D(NXTheme);
  d->q_ptr = this;
  d->_initThemeColor();
}

NXTheme::~NXTheme()
{
}

void NXTheme::setThemeMode(NXThemeType::ThemeMode themeMode)
{
  Q_D(NXTheme);
  d->m_ThemeMode = themeMode;
  Q_EMIT themeModeChanged(d->m_ThemeMode);
}

NXThemeType::ThemeMode NXTheme::getThemeMode() const
{
  Q_CONST_D(NXTheme);
  return d->m_ThemeMode;
}

void NXTheme::setIsFollowSystemTheme(bool isFollow)
{
  Q_D(NXTheme);
  if (d->m_IsFollowSystemTheme == isFollow) {
    return;
  }
  d->m_IsFollowSystemTheme = isFollow;
  if (isFollow) {
    if (QCoreApplication::instance()) {
      QCoreApplication::instance()->installEventFilter(d);
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    d->m_ColorSchemeConnection =
        connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, d, [d](Qt::ColorScheme) {
      d->_applySystemTheme();
    });
#endif
    d->_applySystemTheme();
  } else {
    if (QCoreApplication::instance()) {
      QCoreApplication::instance()->removeEventFilter(d);
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    if (d->m_ColorSchemeConnection) {
      disconnect(d->m_ColorSchemeConnection);
      d->m_ColorSchemeConnection = {};
    }
#endif
  }
  Q_EMIT signalIsFollowSystemThemeChanged();
}

bool NXTheme::getIsFollowSystemTheme() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXTheme);
  return d->m_IsFollowSystemTheme;
}

void NXTheme::setThemeColor(NXThemeType::ThemeMode themeMode, NXThemeType::ThemeColor themeColor, QColor const &newColor)
{
  Q_D(NXTheme);
  if (themeMode == NXThemeType::Light) {
    d->m_LightThemeColorList[themeColor] = newColor;
  } else {
    d->m_DarkThemeColorList[themeColor] = newColor;
  }
}

QColor NXTheme::getThemeColor(NXThemeType::ThemeMode themeMode, NXThemeType::ThemeColor themeColor)
{
  Q_D(NXTheme);
  if (themeMode == NXThemeType::Light) {
    return d->m_LightThemeColorList[themeColor];
  } else {
    return d->m_DarkThemeColorList[themeColor];
  }
}

void NXTheme::drawEffectShadow(QPainter *painter, QRect widgetRect, int shadowBorderWidth, int borderRadius, int maxAlpha, int extendPixels, QColor const &lightColor, QColor const &darkColor)
{
  Q_D(NXTheme);
  painter->save();
  painter->setRenderHints(QPainter::Antialiasing);
  painter->setPen(Qt::NoPen);

  QColor color = d->m_ThemeMode == NXThemeType::Light ? lightColor : darkColor;

  QPainterPath path;
  path.setFillRule(Qt::WindingFill);

  for (int i = 0; i < shadowBorderWidth; ++i) {
    int expansion          = i + 1;
    qreal currentRadius    = borderRadius + expansion / 2;
    int currentBorderWidth = shadowBorderWidth - i;

    path.addRoundedRect(widgetRect.x() + currentBorderWidth, widgetRect.y() + currentBorderWidth, widgetRect.width() - currentBorderWidth * 2, widgetRect.height() - currentBorderWidth * 2, currentRadius, currentRadius);
    int alpha = maxAlpha * currentBorderWidth / shadowBorderWidth;
    color.setAlpha(alpha);

    painter->setBrush(color);
    painter->drawPath(path);
  }
  painter->restore();
}
