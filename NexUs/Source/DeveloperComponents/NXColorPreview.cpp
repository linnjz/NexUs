#include "NXColorPreview.h"

#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"

NXColorPreview::NXColorPreview(QWidget *parent)
    : QWidget { parent }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  setFixedSize(50, 260);
}

NXColorPreview::~NXColorPreview()
{
}

void NXColorPreview::paintEvent(QPaintEvent *event)
{
  int penWidth     = 3;
  int borderRadius = 5;
  QPainter painter(this);
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing);
  QRect viewRect = rect();
  viewRect.adjust(penWidth, penWidth, -penWidth, -penWidth);
  painter.setPen(QPen(NXThemeColor(m_ThemeMode, BasicBorder), penWidth));
  painter.setBrush(m_BaseColor);
  painter.drawRoundedRect(viewRect, borderRadius, borderRadius);
  painter.restore();
}
