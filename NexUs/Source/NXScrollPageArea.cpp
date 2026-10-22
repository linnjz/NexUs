#include "NXScrollPageArea.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>

#include "NXTheme.h"
#include "private/NXScrollPageAreaPrivate.h"
Q_PROPERTY_CREATE_CPP(NXScrollPageArea, int, BorderRadius)

NXScrollPageArea::NXScrollPageArea(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXScrollPageAreaPrivate())
{
  Q_D(NXScrollPageArea);
  d->q_ptr          = this;
  d->_pBorderRadius = 6;
  setFixedHeight(75);
  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXScrollPageArea::~NXScrollPageArea()
{
}

void NXScrollPageArea::paintEvent(QPaintEvent *event)
{
  Q_D(NXScrollPageArea);
  QPainter painter(this);
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(NXThemeColor(d->_themeMode, BasicBorder));
  painter.setBrush(NXThemeColor(d->_themeMode, BasicBaseAlpha));
  QRect foregroundRect(1, 1, width() - 2, height() - 2);
  painter.drawRoundedRect(foregroundRect, d->_pBorderRadius, d->_pBorderRadius);
  painter.restore();
}
