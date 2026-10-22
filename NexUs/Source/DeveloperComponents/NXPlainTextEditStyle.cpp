#include "NXPlainTextEditStyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QtMath>

#include "NXTheme.h"

NXPlainTextEditStyle::NXPlainTextEditStyle(QStyle *style)
{
  _pExpandMarkWidth = 0;
  _themeMode        = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXPlainTextEditStyle::~NXPlainTextEditStyle()
{
}

void NXPlainTextEditStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    if (const QStyleOptionFrame *fopt = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
      //背景绘制
      QRect editRect = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing);
      // 边框绘制
      painter->setPen(NXThemeColor(_themeMode, BasicBorder));
      painter->setBrush(Qt::NoBrush);
      painter->drawRoundedRect(editRect.adjusted(1, 1, -1, -1), 6, 6);
      painter->setPen(Qt::NoPen);

      // 背景绘制
      painter->setBrush(NXThemeColor(_themeMode, BasicBaseAlpha));
      painter->drawRoundedRect(
          QRectF(editRect.x() + 1.5, editRect.y() + 1.5, editRect.width() - 3, editRect.height() - 3), 6, 6);

      // 底边线绘制
      painter->setBrush(NXThemeColor(_themeMode, BasicHemline));
      QPainterPath path;
      path.moveTo(6, editRect.height());
      path.lineTo(editRect.width() - 6, editRect.height());
      path.arcTo(QRectF(editRect.width() - 12, editRect.height() - 12, 12, 12), -90, 45);
      path.lineTo(6 - 3 * std::sqrt(2), editRect.height() - (6 - 3 * std::sqrt(2)));
      path.arcTo(QRectF(0, editRect.height() - 12, 12, 12), 270, 45);
      path.closeSubpath();
      painter->drawPath(path);

      //焦点指示器
      painter->setPen(Qt::NoPen);
      painter->setBrush(NXThemeColor(_themeMode, PrimaryNormal));
      painter->drawRoundedRect(
          QRectF(editRect.width() / 2 - _pExpandMarkWidth, editRect.height() - 2.5, _pExpandMarkWidth * 2, 2.5), 2, 2);

      painter->restore();
    }
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawControl(element, option, painter, widget);
}
