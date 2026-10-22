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
  d->_themeMode = themeMode;
  Q_EMIT themeModeChanged(d->_themeMode);
}

NXThemeType::ThemeMode NXTheme::getThemeMode() const
{
  Q_D(const NXTheme);
  return d->_themeMode;
}

void NXTheme::setIsFollowSystemTheme(bool isFollow)
{
  Q_D(NXTheme);
  if (d->_pIsFollowSystemTheme == isFollow) {
    return;
  }
  d->_pIsFollowSystemTheme = isFollow;
  if (isFollow) {
    if (QCoreApplication::instance()) {
      QCoreApplication::instance()->installEventFilter(d);
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    d->_colorSchemeConnection =
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
    if (d->_colorSchemeConnection) {
      disconnect(d->_colorSchemeConnection);
      d->_colorSchemeConnection = {};
    }
#endif
  }
  Q_EMIT pIsFollowSystemThemeChanged(isFollow);
}

bool NXTheme::getIsFollowSystemTheme() const
{
  Q_D(const NXTheme);
  return d->_pIsFollowSystemTheme;
}

void NXTheme::setThemeColor(NXThemeType::ThemeMode themeMode, NXThemeType::ThemeColor themeColor, const QColor &newColor)
{
  Q_D(NXTheme);
  if (themeMode == NXThemeType::Light) {
    d->_lightThemeColorList[themeColor] = newColor;
  } else {
    d->_darkThemeColorList[themeColor] = newColor;
  }
}

QColor NXTheme::getThemeColor(NXThemeType::ThemeMode themeMode, NXThemeType::ThemeColor themeColor)
{
  Q_D(NXTheme);
  if (themeMode == NXThemeType::Light) {
    return d->_lightThemeColorList[themeColor];
  } else {
    return d->_darkThemeColorList[themeColor];
  }
}

void NXTheme::drawEffectShadow(QPainter *painter, QRect widgetRect, int shadowBorderWidth, int borderRadius, int maxAlpha, int extendPixels, const QColor &lightColor, const QColor &darkColor)
{
  Q_D(NXTheme);
  painter->save();
  painter->setRenderHints(QPainter::Antialiasing);
  painter->setPen(Qt::NoPen);

  QColor color = d->_themeMode == NXThemeType::Light ? lightColor : darkColor;

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
