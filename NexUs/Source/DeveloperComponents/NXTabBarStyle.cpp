#include "NXTabBarStyle.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QTabBar>

#include "NXApplication.h"
#include "NXTheme.h"

NXTabBarStyle::NXTabBarStyle(QStyle *style)
{
  _pTabSize                    = QSize(220, 42);
  _pTabCornerRadius            = 8;
  _pIsSelectedIndicatorVisible = true;
  _pTabBarStyle                = NXTabBarType::Firefox;
  _closeIndicatorSize          = 24;
  _themeMode                   = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXTabBarStyle::~NXTabBarStyle()
{
}

void NXTabBarStyle::setTabBarStyle(NXTabBarType::TabBarStyle tabBarStyle)
{
  _pTabBarStyle       = tabBarStyle;
  _closeIndicatorSize = tabBarStyle == NXTabBarType::Firefox ? 24 : 20;
}

NXTabBarType::TabBarStyle NXTabBarStyle::getTabBarStyle() const
{
  return _pTabBarStyle;
}

void NXTabBarStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
  // qDebug() << pe << opt->rect;
  switch (pe) {
  case QStyle::PE_FrameTabBarBase : {
    // 底边线
    // p->save();
    // p->setRenderHint(QPainter::Antialiasing);
    // QColor lineColor = NXThemeColor(_themeMode, BasicText);
    // lineColor.setAlpha(77); // 约 30% 透明度
    // p->setPen(QPen(lineColor, 1));
    // p->drawLine(opt->rect.bottomLeft(), opt->rect.bottomRight());
    // p->restore();
    return;
  }
  case QStyle::PE_IndicatorArrowLeft :
  case QStyle::PE_IndicatorArrowRight : {
    // QRect tabRect = opt->rect;
    // p->save();
    // p->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // QColor arrowColor = NXThemeColor(_themeMode, BasicText);

    // if (!(opt->state & QStyle::State_Enabled)) { arrowColor.setAlpha(100); }

    // if (opt->state.testFlag(QStyle::State_MouseOver))
    //{
    //   p->setPen(Qt::NoPen);
    //   p->setBrush(opt->state.testFlag(QStyle::State_Selected) ? NXThemeColor(_themeMode, TabBarSelectedCloseHover)
    //                                                           : NXThemeColor(_themeMode, TabBarCloseButtonHover));
    //   p->drawRoundedRect(tabRect, 6, 6);
    // }
    // QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    // iconFont.setPixelSize(16);
    // p->setFont(iconFont);
    // p->setPen(arrowColor);
    // p->drawText(tabRect,
    //             Qt::AlignCenter,
    //             QChar((QStyle::PE_IndicatorArrowLeft == pe ? NXIconType::ChevronLeft
    //                                                                         : NXIconType::ChevronRight)));
    // p->restore();
    return;
  }
  case QStyle::PE_PanelButtonTool : {
    //// 左侧隐藏图标按钮（例如下拉箭头或省略号）
    // p->save();
    // p->setRenderHint(QPainter::Antialiasing);

    //// 绘制背景（悬停时显示）
    // if (opt->state & QStyle::State_MouseOver)
    //{
    //   p->setBrush(NXThemeColor(_themeMode, TabBarHover));
    //   p->setPen(Qt::NoPen);
    //   p->drawRoundedRect(opt->rect, 4, 4);
    // }

    //// 绘制图标（下拉箭头，可改为三个点）
    // QFont iconFont = QFont(QStringLiteral("NXAwesome")); // 若字体不支持可改用多边形
    // iconFont.setPixelSize(14);
    // p->setFont(iconFont);
    // p->setPen(NXThemeColor(_themeMode, BasicText));

    //// 使用 Unicode 字符（例如向下箭头）或三个点
    //// 这里使用下拉箭头（U+25BC）作为示例
    // p->drawText(opt->rect, Qt::AlignCenter, QString(QChar(0x25'BC)));

    // p->restore();
    return;
  }
  case QStyle::PE_IndicatorTabTear : {
    // QRect tabRect = opt->rect;
    // p->save();
    // p->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    // p->setPen(Qt::NoPen);
    // p->setBrush(NXThemeColor(_themeMode, PrimaryNormal));
    // QRectF indicatorRect = QRectF(tabRect.center().x() - 0.5, tabRect.y() + 7, 1, tabRect.height() - 15);
    // p->drawRoundedRect(indicatorRect, 0.5, 0.5);
    // p->restore();
    return;
  }
  case QStyle::PE_IndicatorTabClose : {
    p->save();
    p->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    if (opt->state.testFlag(QStyle::State_MouseOver)) {
      p->setPen(Qt::NoPen);
      p->setBrush(opt->state.testFlag(QStyle::State_Selected) ? NXThemeColor(_themeMode, TabBarSelectedCloseButtonHover) : NXThemeColor(_themeMode, TabBarCloseButtonHover));
      if (_pTabBarStyle == NXTabBarType::Google) {
        p->drawEllipse(opt->rect);
      } else {
        p->drawRoundedRect(opt->rect, 4, 4);
      }
    }
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(nxApp->getFontPixelSize() + 3);
    p->setFont(iconFont);
    p->setPen(NXThemeColor(_themeMode, BasicText));
    p->drawText(opt->rect, Qt::AlignCenter, QChar(NXIconType::Xmark));
    p->restore();
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawPrimitive(pe, opt, p, w);
}

void NXTabBarStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  constexpr int margin = 9;
  switch (element) {
  case QStyle::CE_TabBarTab : {
    if (const QStyleOptionTab *topt = qstyleoption_cast<const QStyleOptionTab *>(option)) {
      drawControl(CE_TabBarTabShape, topt, painter, widget);
      drawControl(CE_TabBarTabLabel, topt, painter, widget);
      return;
    }
    break;
  }
  case QStyle::CE_TabBarTabShape : {
    // 背景绘制
    if (const QStyleOptionTab *topt = qstyleoption_cast<const QStyleOptionTab *>(option)) {
      QRect tabRect = topt->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
      painter->setPen(Qt::NoPen);
      if (topt->state.testFlag(QStyle::State_Selected)) {
        painter->setBrush(NXThemeColor(_themeMode, TabBarSelected));
        if (_pTabBarStyle == NXTabBarType::Google) {
          tabRect = topt->rect.adjusted(6, 6, 0, 0);
          tabRect.setLeft(tabRect.left() - margin);
          tabRect.setRight(tabRect.right() + margin + 1);
          QPainterPath path;
          path.moveTo(tabRect.x(), tabRect.bottom() + 1);
          path.arcTo(QRectF(tabRect.x() - margin, tabRect.bottom() - margin * 2 + 1, margin * 2, margin * 2), -90, 90);
          path.lineTo(tabRect.x() + margin, tabRect.y() + _pTabCornerRadius);
          path.arcTo(QRectF(tabRect.x() + margin, tabRect.y(), _pTabCornerRadius * 2, _pTabCornerRadius * 2), 180, -90);
          path.lineTo(tabRect.right() - margin - _pTabCornerRadius, tabRect.y());
          path.arcTo(QRectF(tabRect.right() - margin - 2 * _pTabCornerRadius, tabRect.y(), _pTabCornerRadius * 2, _pTabCornerRadius * 2), 90, -90);
          path.lineTo(tabRect.right() - margin, tabRect.bottom() - margin);
          path.arcTo(QRectF(tabRect.right() - margin, tabRect.bottom() - 2 * margin + 1, margin * 2, margin * 2), -180, 90);
          path.lineTo(tabRect.right(), tabRect.bottom() + 10);
          path.lineTo(tabRect.x(), tabRect.bottom() + 10);
          path.closeSubpath();
          painter->drawPath(path);
        } else {
          tabRect = topt->rect.adjusted(9, 4, -3, -3);
          nxTheme->drawEffectShadow(painter, tabRect.adjusted(1, 1, -1, -1), 4, _pTabCornerRadius);
          painter->setPen(NXThemeColor(_themeMode, PopupBorder));
          painter->drawRoundedRect(tabRect, _pTabCornerRadius, _pTabCornerRadius);
        }
        if (_pIsSelectedIndicatorVisible) {
          painter->setPen(Qt::NoPen);
          painter->setBrush(NXThemeColor(_themeMode, PrimaryNormal));
          QRectF indicatorRect;
          if (_pTabBarStyle == NXTabBarType::Google) {
            indicatorRect = QRectF(tabRect.left() + margin + 7, tabRect.y() + 6, 3, tabRect.height() - 16);
          } else {
            indicatorRect = QRectF(tabRect.left() + 7, tabRect.y() + 8.5, 3, tabRect.height() - 16);
          }
          painter->drawRoundedRect(indicatorRect, 2, 2);
        }
      } else if (topt->state.testFlag(QStyle::State_MouseOver)) {
        painter->setBrush(NXThemeColor(_themeMode, TabBarHover));
        if (_pTabBarStyle == NXTabBarType::Google) {
          tabRect = topt->rect.adjusted(6, 6, -6, -5);
          painter->drawRoundedRect(tabRect, _pTabCornerRadius, _pTabCornerRadius);
        } else {
          tabRect = topt->rect.adjusted(3, 4, -3, -3);
          painter->drawRoundedRect(tabRect, _pTabCornerRadius, _pTabCornerRadius);
        }
      }
      painter->restore();
      return;
    }
    break;
  }
  case QStyle::CE_TabBarTabLabel : {
    // 文字和图标绘制
    if (const QStyleOptionTab *topt = qstyleoption_cast<const QStyleOptionTab *>(option)) {
      const int indent = _pIsSelectedIndicatorVisible ? 0 : 6;
      QRect textRect   = subElementRect(QStyle::SE_TabBarTabText, topt, widget);
      textRect.setLeft(textRect.left() + 16 - indent);
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
      // 图标绘制
      QIcon icon = topt->icon;
      if (!icon.isNull()) {
        QRectF iconRect(topt->rect.x() + 24 - indent, textRect.center().y() - (qreal) topt->iconSize.height() / 2 + 2, topt->iconSize.width(), topt->iconSize.height());
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QPixmap iconPix = icon.pixmap(topt->iconSize, painter->device()->devicePixelRatio(), (topt->state & State_Enabled) ? QIcon::Normal : QIcon::Disabled, (topt->state & State_Selected) ? QIcon::On : QIcon::Off);
#else
        QPixmap iconPix = icon.pixmap(topt->iconSize, (topt->state & State_Enabled) ? QIcon::Normal : QIcon::Disabled, (topt->state & State_Selected) ? QIcon::On : QIcon::Off);
#endif
        painter->drawPixmap(iconRect.x(), iconRect.y(), iconPix);
      }
      // 文字绘制
      painter->setPen(NXThemeColor(_themeMode, BasicText));
      QString text = painter->fontMetrics().elidedText(topt->text, Qt::ElideRight, textRect.width());
      painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip, text);
      painter->restore();
      return;
    }
    break;
  }
  default : {
    break;
  }
  }

  QProxyStyle::drawControl(element, option, painter, widget);
}

QSize NXTabBarStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
  switch (type) {
  case CT_TabBarTab : {
    return _pTabSize;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}

int NXTabBarStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
  switch (hint) {
  case SH_TabBar_CloseButtonPosition : {
    return QTabBar::RightSide;
  }
  default : break;
  }
  return QProxyStyle::styleHint(hint, option, widget, returnData);
}

int NXTabBarStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
  switch (metric) {
  case PM_TabBarScrollButtonWidth : {
    return 22;
  }
  case PM_TabCloseIndicatorWidth :
  case PM_TabCloseIndicatorHeight : {
    return _closeIndicatorSize;
  }
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}

QRect NXTabBarStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
  // QMargins margins  = widget->contentsMargins();
  // QRect contentRect = option->rect.adjusted(margins.left(), margins.top(), -margins.right(), -margins.bottom());
  //  constexpr QSize scrollButtonSize { 22, 30 };
  switch (element) {
  // case QStyle::SE_TabBarTearIndicatorLeft :
  //{
  //   return QRect(contentRect.x() + 6 + scrollButtonSize.width() + 6, contentRect.y(), 1, contentRect.height());
  // }
  // case QStyle::SE_TabBarTearIndicatorRight :
  //{
  //   return QRect(contentRect.right() - 6 - scrollButtonSize.width() - 6, contentRect.y(), 1, contentRect.height());
  // }
  // case QStyle::SE_TabBarScrollLeftButton :
  //{
  //   return QRect(QPoint { contentRect.x() + 6, 6 }, scrollButtonSize);
  // }
  // case QStyle::SE_TabBarScrollRightButton :
  //{
  //   return QRect(QPoint { contentRect.right() - 6 - scrollButtonSize.width(), 6 }, scrollButtonSize);
  // }
  case QStyle::SE_TabBarTearIndicatorLeft :
  case QStyle::SE_TabBarTearIndicatorRight :
  case QStyle::SE_TabBarScrollLeftButton :
  case QStyle::SE_TabBarScrollRightButton  : {
    return QRect {};
  }
  default : {
    break;
  }
  }
  return QProxyStyle::subElementRect(element, option, widget);
}
