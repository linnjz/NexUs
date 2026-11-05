#include "NXLCDNumberStyle.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>

#include "NXTheme.h"

NXLCDNumberStyle::NXLCDNumberStyle(QStyle *style)
    : m_IsTransparent { false }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXLCDNumberStyle::~NXLCDNumberStyle()
{
}

void NXLCDNumberStyle::drawControl(QStyle::ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    if (QStyleOptionFrame const *fopt = qstyleoption_cast<QStyleOptionFrame const *>(option)) {
      if (!m_IsTransparent) {
        // 背景绘制
        QRect numberRect = option->rect;
        painter->save();
        painter->setRenderHints(QPainter::Antialiasing);
        // 边框绘制
        painter->setPen(NXThemeColor(m_ThemeMode, BasicBorder));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(numberRect.adjusted(1, 1, -1, -1), 6, 6);
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicBaseAlpha));
        painter->drawRoundedRect(
            QRectF(numberRect.x() + 1.5, numberRect.y() + 1.5, numberRect.width() - 3, numberRect.height() - 3), 6, 6);
        painter->restore();
      }
    }
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawControl(element, option, painter, widget);
}
