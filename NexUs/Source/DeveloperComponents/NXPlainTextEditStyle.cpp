#include "NXPlainTextEditStyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QtMath>

#include "NXTheme.h"

NXPlainTextEditStyle::NXPlainTextEditStyle(QStyle *style)
    : m_ExpandMarkWidth { 0 }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXPlainTextEditStyle::~NXPlainTextEditStyle()
{
}

void NXPlainTextEditStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    if (QStyleOptionFrame const *fopt = qstyleoption_cast<QStyleOptionFrame const *>(option)) {
      //背景绘制
      QRect editRect = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing);
      // 边框绘制
      painter->setPen(NXThemeColor(m_ThemeMode, BasicBorder));
      painter->setBrush(Qt::NoBrush);
      painter->drawRoundedRect(editRect.adjusted(1, 1, -1, -1), 6, 6);
      painter->setPen(Qt::NoPen);

      // 背景绘制
      painter->setBrush(NXThemeColor(m_ThemeMode, BasicBaseAlpha));
      painter->drawRoundedRect(
          QRectF(editRect.x() + 1.5, editRect.y() + 1.5, editRect.width() - 3, editRect.height() - 3), 6, 6);

      // 底边线绘制
      painter->setBrush(NXThemeColor(m_ThemeMode, BasicHemline));
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
      painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
      painter->drawRoundedRect(
          QRectF(editRect.width() / 2 - m_ExpandMarkWidth, editRect.height() - 2.5, m_ExpandMarkWidth * 2, 2.5), 2, 2);

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
