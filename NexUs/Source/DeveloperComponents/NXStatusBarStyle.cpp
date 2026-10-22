#include "NXStatusBarStyle.h"

#include <QDebug>
#include <QPainter>
#include <QStatusBar>
#include <QStyleOption>

#include "NXTheme.h"

NXStatusBarStyle::NXStatusBarStyle(QStyle *style)
{
  _themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXStatusBarStyle::~NXStatusBarStyle()
{
}

void NXStatusBarStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  switch (element) {
  case QStyle::PE_PanelStatusBar : {
    //背景绘制
    QRect statusBarRect = option->rect;
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter->setPen(NXThemeColor(_themeMode, BasicBorder));
    painter->setBrush(NXThemeColor(_themeMode, BasicBaseAlpha));
    painter->drawRect(statusBarRect);
    painter->restore();
    return;
  }
  case QStyle::PE_FrameStatusBarItem : {
    //间隔符绘制
    const QStatusBar *statusBar = dynamic_cast<const QStatusBar *>(widget->parentWidget());
    if (statusBar && statusBar->isSizeGripEnabled()) {
      QRect statusBarItemRect = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
      painter->setPen(Qt::NoPen);
      painter->setBrush(NXThemeColor(_themeMode, PrimaryNormal));
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

void NXStatusBarStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
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

QSize NXStatusBarStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
  //qDebug() << type << QProxyStyle::sizeFromContents(type, option, size, widget);
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}

int NXStatusBarStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
  return QProxyStyle::pixelMetric(metric, option, widget);
}
