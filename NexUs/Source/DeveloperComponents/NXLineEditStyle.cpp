#include "NXLineEditStyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QtMath>

#include "NXTheme.h"

NXLineEditStyle::NXLineEditStyle(QStyle *style)
{
  _pContentsMargins = { 10, 0, 0, 0 };
  _pIconMargin      = 10;
  _pBorderRadius    = 6;
  _themeMode        = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXLineEditStyle::~NXLineEditStyle()
{
}

void NXLineEditStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  switch (element) {
  case PE_PanelLineEdit : {
    if (const QStyleOptionFrame *fopt = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
      QRect lineEditRect = fopt->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing);
      // 边框绘制
      painter->setPen(NXThemeColor(_themeMode, BasicBorder));
      painter->setBrush(Qt::NoBrush);
      painter->drawRoundedRect(lineEditRect, _pBorderRadius, _pBorderRadius);
      painter->setPen(Qt::NoPen);
      //  背景绘制
      if (fopt->state & QStyle::State_HasFocus) {
        painter->setBrush(NXThemeColor(_themeMode, DialogBase));
      } else if (fopt->state & QStyle::State_MouseOver) {
        painter->setBrush(NXThemeColor(_themeMode, BasicHoverAlpha));
      } else {
        painter->setBrush(NXThemeColor(_themeMode, BasicBaseAlpha));
      }
      painter->drawRoundedRect(
          QRectF(lineEditRect.x() + 1.5, lineEditRect.y() + 1.5, lineEditRect.width() - 3, lineEditRect.height() - 3),
          _pBorderRadius, _pBorderRadius);

      // 底边线绘制
      painter->setBrush(NXThemeColor(_themeMode, BasicHemline));
      QPainterPath path;
      path.moveTo(6, lineEditRect.height());
      path.lineTo(lineEditRect.width() - 6, lineEditRect.height());
      path.arcTo(QRectF(lineEditRect.width() - 12, lineEditRect.height() - 12, 12, 12), -90, 45);
      path.lineTo(6 - 3 * std::sqrt(2), lineEditRect.height() - (6 - 3 * std::sqrt(2)));
      path.arcTo(QRectF(0, lineEditRect.height() - 12, 12, 12), 225, 45);
      path.closeSubpath();
      painter->drawPath(path);
      painter->restore();
    }
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawPrimitive(element, option, painter, widget);
}

QRect NXLineEditStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
  if (element == SE_LineEditContents) {
    QRect r = QProxyStyle::subElementRect(element, option, widget);
    r.adjust(_pContentsMargins.left(), _pContentsMargins.top(), -_pContentsMargins.right(), -_pContentsMargins.bottom());
    return r;
  }
  return QProxyStyle::subElementRect(element, option, widget);
}

int NXLineEditStyle::pixelMetric(PixelMetric metric, const QStyleOption *option /*= nullptr*/, const QWidget *widget /*= nullptr*/) const
{
  switch (metric) {
  case PM_LineEditIconMargin : return _pIconMargin;
  default                    : return QProxyStyle::pixelMetric(metric, option, widget);
  }
}
