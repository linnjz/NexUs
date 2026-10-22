#include "NXLCDNumberStyle.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>

#include "NXTheme.h"

NXLCDNumberStyle::NXLCDNumberStyle(QStyle *style)
{
  _pIsTransparent = false;
  _themeMode      = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXLCDNumberStyle::~NXLCDNumberStyle()
{
}

void NXLCDNumberStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    if (const QStyleOptionFrame *fopt = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
      if (!_pIsTransparent) {
        // 背景绘制
        QRect numberRect = option->rect;
        painter->save();
        painter->setRenderHints(QPainter::Antialiasing);
        // 边框绘制
        painter->setPen(NXThemeColor(_themeMode, BasicBorder));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(numberRect.adjusted(1, 1, -1, -1), 6, 6);
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(_themeMode, BasicBaseAlpha));
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
