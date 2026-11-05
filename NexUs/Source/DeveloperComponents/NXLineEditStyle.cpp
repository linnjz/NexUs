#include "NXLineEditStyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QtMath>

#include "NXTheme.h"

NXLineEditStyle::NXLineEditStyle(QStyle *style)
    : m_ContentsMargins { 10, 0, 0, 0 }
    , m_BorderRadius { 6 }
    , m_IconMargin { 10 }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXLineEditStyle::~NXLineEditStyle()
{
}

void NXLineEditStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case PE_PanelLineEdit : {
    if (QStyleOptionFrame const *fopt = qstyleoption_cast<QStyleOptionFrame const *>(option)) {
      QRect lineEditRect = fopt->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing);
      // 边框绘制
      painter->setPen(NXThemeColor(m_ThemeMode, BasicBorder));
      painter->setBrush(Qt::NoBrush);
      painter->drawRoundedRect(lineEditRect, m_BorderRadius, m_BorderRadius);
      painter->setPen(Qt::NoPen);
      //  背景绘制
      if (fopt->state & QStyle::State_HasFocus) {
        painter->setBrush(NXThemeColor(m_ThemeMode, DialogBase));
      } else if (fopt->state & QStyle::State_MouseOver) {
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicHoverAlpha));
      } else {
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicBaseAlpha));
      }
      painter->drawRoundedRect(
          QRectF(lineEditRect.x() + 1.5, lineEditRect.y() + 1.5, lineEditRect.width() - 3, lineEditRect.height() - 3),
          m_BorderRadius, m_BorderRadius);

      // 底边线绘制
      painter->setBrush(NXThemeColor(m_ThemeMode, BasicHemline));
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

QRect NXLineEditStyle::subElementRect(SubElement element, QStyleOption const *option, QWidget const *widget) const
{
  if (element == SE_LineEditContents) {
    QRect r = QProxyStyle::subElementRect(element, option, widget);
    r.adjust(m_ContentsMargins.left(), m_ContentsMargins.top(), -m_ContentsMargins.right(), -m_ContentsMargins.bottom());
    return r;
  }
  return QProxyStyle::subElementRect(element, option, widget);
}

int NXLineEditStyle::pixelMetric(PixelMetric metric, QStyleOption const *option /*= nullptr*/, QWidget const *widget /*= nullptr*/) const
{
  switch (metric) {
  case PM_LineEditIconMargin : return m_IconMargin;
  default                    : return QProxyStyle::pixelMetric(metric, option, widget);
  }
}
