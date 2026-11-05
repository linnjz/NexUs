#include "NXStatusBarStyle.h"

#include <QDebug>
#include <QPainter>
#include <QStatusBar>
#include <QStyleOption>

#include "NXTheme.h"

NXStatusBarStyle::NXStatusBarStyle(QStyle *style)
    : m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXStatusBarStyle::~NXStatusBarStyle()
{
}

void NXStatusBarStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::PE_PanelStatusBar : {
    //背景绘制
    QRect statusBarRect = option->rect;
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter->setPen(NXThemeColor(m_ThemeMode, BasicBorder));
    painter->setBrush(NXThemeColor(m_ThemeMode, BasicBaseAlpha));
    painter->drawRect(statusBarRect);
    painter->restore();
    return;
  }
  case QStyle::PE_FrameStatusBarItem : {
    //间隔符绘制
    QStatusBar const *statusBar = dynamic_cast<QStatusBar const *>(widget->parentWidget());
    if (statusBar && statusBar->isSizeGripEnabled()) {
      QRect statusBarItemRect = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
      painter->setPen(Qt::NoPen);
      painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
      painter->drawRoundedRect(QRectF(statusBarItemRect.right() - 3, statusBarItemRect.y() + statusBarItemRect.height() * 0.1, 3, statusBarItemRect.height() - statusBarItemRect.height() * 0.2), 2, 2);
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

void NXStatusBarStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::CE_SizeGrip : {
    return;
  }
  default : {
    break;
  }
  }

  QProxyStyle::drawControl(element, option, painter, widget);
}

QSize NXStatusBarStyle::sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const
{
  //qDebug() << type << QProxyStyle::sizeFromContents(type, option, size, widget);
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}

int NXStatusBarStyle::pixelMetric(PixelMetric metric, QStyleOption const *option, QWidget const *widget) const
{
  return QProxyStyle::pixelMetric(metric, option, widget);
}
