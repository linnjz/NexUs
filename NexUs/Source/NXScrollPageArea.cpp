#include "NXScrollPageArea.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>

#include "NXTheme.h"
#include "private/NXScrollPageAreaPrivate.h"
Q_PROPERTY_CREATE_CPP(NXScrollPageArea, QPS_PAEMIT(int), BorderRadius)

NXScrollPageArea::NXScrollPageArea(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXScrollPageAreaPrivate())
{
  Q_D(NXScrollPageArea);
  d->q_ptr          = this;
  d->m_BorderRadius = 6;
  setFixedHeight(75);
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
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
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorder));
  painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBaseAlpha));
  QRect foregroundRect(1, 1, width() - 2, height() - 2);
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  painter.restore();
}
