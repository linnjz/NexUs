#include "NXWindowStyle.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QtMath>

#include "NXTheme.h"

NXWindowStyle::NXWindowStyle(QStyle *style)
    : m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXWindowStyle::~NXWindowStyle()
{
}

void NXWindowStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::PE_FrameTabBarBase : {
    return;
  }
  case QStyle::PE_PanelButtonTool : {
    if (option->state.testFlag(QStyle::State_Enabled)) {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setPen(Qt::NoPen);
      if (option->state.testFlag(QStyle::State_Sunken)) {
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicHoverAlpha));
      } else if (option->state.testFlag(QStyle::State_MouseOver)) {
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicPressAlpha));
      } else {
        painter->setBrush(Qt::transparent);
      }
      painter->drawRect(option->rect);
      painter->restore();
    }
    return;
  }
  case QStyle::PE_IndicatorArrowLeft : {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(option->state.testFlag(QStyle::State_Enabled) ? NXThemeColor(m_ThemeMode, BasicText) : NXThemeColor(m_ThemeMode, BasicTextDisable));
    // 左三角
    int sideLength      = 10;
    QRect indicatorRect = option->rect;
    QPainterPath path;
    path.moveTo(indicatorRect.center().x() - qCos(30 * M_PI / 180.0) * sideLength / 2, indicatorRect.center().y());
    path.lineTo(indicatorRect.center().x() + qCos(30 * M_PI / 180.0) * sideLength / 2, indicatorRect.center().y() - sideLength / 2);
    path.lineTo(indicatorRect.center().x() + qCos(30 * M_PI / 180.0) * sideLength / 2, indicatorRect.center().y() + sideLength / 2);
    path.closeSubpath();
    painter->drawPath(path);
    painter->restore();
    return;
  }
  case QStyle::PE_IndicatorArrowRight : {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(option->state.testFlag(QStyle::State_Enabled) ? NXThemeColor(m_ThemeMode, BasicText) : NXThemeColor(m_ThemeMode, BasicTextDisable));
    // 右三角
    int sideLength      = 10;
    QRect indicatorRect = option->rect;
    QPainterPath path;
    path.moveTo(indicatorRect.center().x() - qCos(30 * M_PI / 180.0) * sideLength / 2, indicatorRect.center().y() - sideLength / 2);
    path.lineTo(indicatorRect.center().x() + qCos(30 * M_PI / 180.0) * sideLength / 2, indicatorRect.center().y());
    path.lineTo(indicatorRect.center().x() - qCos(30 * M_PI / 180.0) * sideLength / 2, indicatorRect.center().y() + sideLength / 2);
    path.closeSubpath();
    painter->drawPath(path);
    painter->restore();
    return;
  }
  case QStyle::PE_IndicatorTabTear : {
    return;
  }
  case QStyle::PE_IndicatorDockWidgetResizeHandle : {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(NXThemeColor(m_ThemeMode, BasicBaseLine), 2));
    QRectF handleRect = option->rect;
    if (option->state.testFlag(QStyle::State_Horizontal)) {
      painter->drawLine(handleRect.x(), handleRect.center().y(), handleRect.right(), handleRect.center().y());
    } else {
      painter->drawLine(handleRect.center().x(), handleRect.y(), handleRect.center().x(), handleRect.bottom());
    }
    painter->restore();
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void NXWindowStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  // qDebug() << element << option->rect;
  switch (element) {
  case QStyle::CE_RubberBand : {
    // 预览颜色
    QRect rubberBandRect = option->rect;
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(m_ThemeMode, BasicHoverAlpha));
    painter->drawRect(rubberBandRect);
    painter->restore();
    return;
  }
  case QStyle::CE_TabBarTabShape : {
    // 背景绘制
    if (QStyleOptionTab const *topt = qstyleoption_cast<QStyleOptionTab const *>(option)) {
      QRect tabRect = topt->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
      painter->setPen(Qt::NoPen);
      if (topt->state & QStyle::State_Selected) {
        if (topt->state & QStyle::State_Sunken) {
          // 选中时点击
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicHoverAlpha));
        } else {
          if (topt->state & QStyle::State_MouseOver) {
            // 选中时覆盖
            painter->setBrush(NXThemeColor(m_ThemeMode, BasicSelectedHoverAlpha));
          } else {
            // 选中
            painter->setBrush(NXThemeColor(m_ThemeMode, BasicSelectedAlpha));
          }
        }
      } else {
        if (topt->state & QStyle::State_Sunken) {
          // 点击时颜色
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicSelectedHoverAlpha));
        } else {
          if (topt->state & QStyle::State_MouseOver) {
            // 覆盖时颜色
            painter->setBrush(NXThemeColor(m_ThemeMode, BasicHoverAlpha));
          }
        }
      }
      painter->drawRect(tabRect);
      // 间隔符绘制
      if (topt->position != QStyleOptionTab::End) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
        painter->drawRoundedRect(QRectF(tabRect.right() - 3, tabRect.y() + 7, 3, tabRect.height() - 14), 2, 2);
      }
      painter->restore();
    }
    return;
  }
  case QStyle::CE_TabBarTabLabel : {
    // 文字绘制
    if (QStyleOptionTab const *topt = qstyleoption_cast<QStyleOptionTab const *>(option)) {
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
      painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
      painter->drawText(topt->rect, Qt::AlignCenter, topt->text);
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
