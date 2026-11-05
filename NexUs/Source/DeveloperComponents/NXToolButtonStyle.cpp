#include "NXToolButtonStyle.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QtMath>

#include "NXTheme.h"

NXToolButtonStyle::NXToolButtonStyle(QStyle *style)
    : m_ExpandIconRotate { 0.0 }
    , m_BorderRadius { 4 }
    , m_IsTransparent { true }
    , m_IsSelected { false }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXToolButtonStyle::~NXToolButtonStyle()
{
}

void NXToolButtonStyle::drawComplexControl(ComplexControl control, QStyleOptionComplex const *option, QPainter *painter, QWidget const *widget) const
{
  switch (control) {
  case QStyle::CC_ToolButton : {
    // 内容绘制
    if (QStyleOptionToolButton const *bopt = qstyleoption_cast<QStyleOptionToolButton const *>(option)) {
      if (bopt->arrowType != Qt::NoArrow) {
        break;
      }
      QRect toolButtonRect = bopt->rect;
      if (!m_IsTransparent) {
        // 边框距离
        toolButtonRect.adjust(1, 1, -1, -1);
      }
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
      painter->setPen(m_IsTransparent ? Qt::transparent : NXThemeColor(m_ThemeMode, BasicBorder));
      // 背景绘制
      if (bopt->state.testFlag(QStyle::State_Enabled)) {
        if (bopt->state.testFlag(QStyle::State_Sunken)) {
          painter->setBrush(m_IsTransparent ? NXThemeColor(m_ThemeMode, BasicPressAlpha) : NXThemeColor(m_ThemeMode, BasicPress));
          painter->drawRoundedRect(toolButtonRect, m_BorderRadius, m_BorderRadius);
        } else {
          if (m_IsSelected) {
            painter->setBrush(m_IsTransparent ? NXThemeColor(m_ThemeMode, BasicSelectedAlpha) : NXThemeColor(m_ThemeMode, BasicHover));
            painter->drawRoundedRect(toolButtonRect, m_BorderRadius, m_BorderRadius);
          } else {
            if (bopt->state.testFlag(QStyle::State_MouseOver) || bopt->state.testFlag(QStyle::State_On)) {
              painter->setBrush(m_IsTransparent ? NXThemeColor(m_ThemeMode, BasicHoverAlpha) : NXThemeColor(m_ThemeMode, BasicHover));
              painter->drawRoundedRect(toolButtonRect, m_BorderRadius, m_BorderRadius);
            } else {
              if (!m_IsTransparent) {
                painter->setBrush(NXThemeColor(m_ThemeMode, BasicBase));
                painter->drawRoundedRect(toolButtonRect, m_BorderRadius, m_BorderRadius);
                // 底边线绘制
                painter->setPen(NXThemeColor(m_ThemeMode, BasicBaseLine));
                painter->drawLine(toolButtonRect.x() + m_BorderRadius, toolButtonRect.y() + toolButtonRect.height(), toolButtonRect.x() + toolButtonRect.width() - m_BorderRadius, toolButtonRect.y() + toolButtonRect.height());
              }
            }
          }
        }
      } else {
        if (!m_IsTransparent) {
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicDisable));
          painter->drawRoundedRect(toolButtonRect, m_BorderRadius, m_BorderRadius);
        }
      }
      // 指示器绘制
      _drawIndicator(painter, bopt, widget);

      // 图标绘制
      QRect contentRect = subControlRect(control, bopt, QStyle::SC_ScrollBarAddLine, widget);
      int heightOffset  = contentRect.height() * 0.05;
      contentRect.adjust(0, heightOffset, 0, -heightOffset);
      _drawIcon(painter, contentRect, bopt, widget);

      // 文字绘制
      _drawText(painter, contentRect, bopt);
      painter->restore();
    }
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawComplexControl(control, option, painter, widget);
}

QSize NXToolButtonStyle::sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const
{
  if (type == QStyle::CT_ToolButton) {
    if (QStyleOptionToolButton const *bopt = qstyleoption_cast<QStyleOptionToolButton const *>(option)) {
      QSize toolButtonSize = QProxyStyle::sizeFromContents(type, option, size, widget);
      if (bopt->features.testFlag(QStyleOptionToolButton::HasMenu) &&
          !bopt->features.testFlag(QStyleOptionToolButton::MenuButtonPopup)) {
        toolButtonSize.setWidth(toolButtonSize.width() + m_ContentMargin + 0.65 * std::min(bopt->iconSize.width(), bopt->iconSize.height()));
      }
      return toolButtonSize;
    }
  }
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}

void NXToolButtonStyle::_drawIndicator(QPainter *painter, QStyleOptionToolButton const *bopt, QWidget const *widget) const
{
  if (bopt->features.testFlag(QStyleOptionToolButton::MenuButtonPopup)) {
    QRect indicatorRect = subControlRect(QStyle::CC_ToolButton, bopt, QStyle::SC_ScrollBarSubLine, widget);
    // 指示器区域
    if (bopt->state.testFlag(QStyle::State_Enabled) && bopt->activeSubControls.testFlag(QStyle::SC_ScrollBarSubLine)) {
      painter->setBrush(NXThemeColor(m_ThemeMode, BasicIndicator));
      QPainterPath path;
      path.moveTo(indicatorRect.topLeft());
      path.lineTo(indicatorRect.right() - 4, indicatorRect.y());
      path.arcTo(QRect(indicatorRect.right() - 8, indicatorRect.y(), 8, 8), 90, -90);
      path.lineTo(indicatorRect.right(), indicatorRect.bottom() - 4);
      path.arcTo(QRect(indicatorRect.right() - 8, indicatorRect.bottom() - 8, 8, 8), 0, -90);
      path.lineTo(indicatorRect.bottomLeft());
      path.closeSubpath();
      painter->drawPath(path);
    }
    // 指示器
    painter->setBrush(bopt->state.testFlag(QStyle::State_Enabled) ? NXThemeColor(m_ThemeMode, BasicText) : NXThemeColor(m_ThemeMode, BasicTextDisable));
    QPainterPath indicatorPath;
    qreal indicatorHeight = qCos(30 * M_PI / 180.0) * indicatorRect.width() * 0.85;
    indicatorPath.moveTo(indicatorRect.x() + indicatorRect.width() * 0.15, indicatorRect.center().y());
    indicatorPath.lineTo(indicatorRect.right() - indicatorRect.width() * 0.15, indicatorRect.center().y());
    indicatorPath.lineTo(indicatorRect.center().x(), indicatorRect.center().y() + indicatorHeight / 2);
    indicatorPath.closeSubpath();
    painter->drawPath(indicatorPath);
  } else if (bopt->features.testFlag(QStyleOptionToolButton::HasMenu)) {
    // 展开指示器
    QSize iconSize = bopt->iconSize;
    painter->save();
    QRect toolButtonRect = bopt->rect;
    QFont iconFont       = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(0.75 * std::min(iconSize.width(), iconSize.height()));
    painter->setFont(iconFont);
    int indicatorWidth = painter->fontMetrics().horizontalAdvance(QChar((unsigned short) NXIconType::AngleDown));
    QRect expandIconRect(toolButtonRect.right() - m_ContentMargin - indicatorWidth, toolButtonRect.y() + 1, indicatorWidth, toolButtonRect.height());
    painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
    painter->translate(expandIconRect.center().x(), expandIconRect.y() + (qreal) expandIconRect.height() / 2);
    painter->rotate(m_ExpandIconRotate);
    painter->translate(-expandIconRect.center().x() - 1, -expandIconRect.y() - (qreal) expandIconRect.height() / 2);
    painter->drawText(expandIconRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::AngleDown));
    painter->restore();
  }
}

void NXToolButtonStyle::_drawIcon(QPainter *painter, QRectF iconRect, QStyleOptionToolButton const *bopt, QWidget const *widget) const
{
  if (bopt->toolButtonStyle != Qt::ToolButtonTextOnly) {
    QSize iconSize = bopt->iconSize;
    if (widget->property("NXIconType").toString().isEmpty()) {
      // 绘制QIcon
      QIcon icon = bopt->icon;
      if (!icon.isNull()) {
        QPixmap iconPix = icon.pixmap(iconSize, (bopt->state & State_Enabled) ? QIcon::Normal : QIcon::Disabled, (bopt->state & State_Selected) ? QIcon::On : QIcon::Off);
        switch (bopt->toolButtonStyle) {
        case Qt::ToolButtonIconOnly : {
          painter->drawPixmap(QRect(QPoint(iconRect.center().x() - iconSize.width() / 2.0, iconRect.center().y() - iconSize.height() / 2.0), iconSize), iconPix);
          break;
        }
        case Qt::ToolButtonFollowStyle :
        case Qt::ToolButtonTextBesideIcon : {
          painter->drawPixmap(
              QRect(QPoint(iconRect.x() + m_ContentMargin, iconRect.center().y() - iconSize.height() / 2), iconSize),
              iconPix);
          break;
        }
        case Qt::ToolButtonTextUnderIcon : {
          if (bopt->features.testFlag(QStyleOptionToolButton::HasMenu) &&
              !bopt->features.testFlag(QStyleOptionToolButton::MenuButtonPopup)) {
            iconRect.setRight(iconRect.right() - _calculateExpandIndicatorWidth(bopt, painter));
          }
          painter->drawPixmap(QRect(QPoint(iconRect.center().x() - iconSize.width() / 2, iconRect.y()), iconSize), iconPix);
          break;
        }
        default : {
          break;
        }
        }
      }
    } else {
      // 绘制NXIcon
      painter->save();
      if (bopt->state.testFlag(QStyle::State_Enabled)) {
        painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
      } else {
        painter->setPen(NXThemeColor(m_ThemeMode, BasicTextDisable));
      }
      QFont iconFont = QFont(QStringLiteral("NXAwesome"));
      int iconRotate = widget->property("NXIconRotate").toInt();
      switch (bopt->toolButtonStyle) {
      case Qt::ToolButtonIconOnly : {
        iconFont.setPixelSize(0.75 * std::min(iconSize.width(), iconSize.height()));
        painter->setFont(iconFont);
        if (iconRotate) {
          painter->translate(iconRect.center());
          painter->rotate(iconRotate);
          painter->translate(-iconRect.center());
        }
        painter->drawText(iconRect, Qt::AlignCenter, widget->property("NXIconType").toString());
        break;
      }
      case Qt::ToolButtonFollowStyle :
      case Qt::ToolButtonTextBesideIcon : {
        QRect adjustIconRect(iconRect.x() + m_ContentMargin, iconRect.y(), iconSize.width(), iconRect.height());
        iconFont.setPixelSize(0.75 * std::min(iconSize.width(), iconSize.height()));
        painter->setFont(iconFont);
        if (iconRotate) {
          painter->translate(adjustIconRect.center());
          painter->rotate(iconRotate);
          painter->translate(-adjustIconRect.center());
        }
        painter->drawText(adjustIconRect, Qt::AlignCenter, widget->property("NXIconType").toString());
        break;
      }
      case Qt::ToolButtonTextUnderIcon : {
        if (bopt->features.testFlag(QStyleOptionToolButton::HasMenu) &&
            !bopt->features.testFlag(QStyleOptionToolButton::MenuButtonPopup)) {
          iconRect.setRight(iconRect.right() - _calculateExpandIndicatorWidth(bopt, painter));
        }
        QRect adjustIconRect(iconRect.center().x() - iconSize.width() / 2, iconRect.y() + 0.2 * std::min(iconSize.width(), iconSize.height()), iconSize.width(), iconSize.height());
        iconFont.setPixelSize(0.8 * std::min(iconSize.width(), iconSize.height()));
        painter->setFont(iconFont);
        if (iconRotate) {
          painter->translate(adjustIconRect.center());
          painter->rotate(iconRotate);
          painter->translate(-adjustIconRect.center());
        }
        painter->drawText(adjustIconRect, Qt::AlignHCenter, widget->property("NXIconType").toString());
        break;
      }
      default : {
        break;
      }
      }
      painter->restore();
    }
  }
}

void NXToolButtonStyle::_drawText(QPainter *painter, QRect contentRect, QStyleOptionToolButton const *bopt) const
{
  if (!bopt->text.isEmpty()) {
    if (bopt->state.testFlag(QStyle::State_Enabled)) {
      painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
    } else {
      painter->setPen(NXThemeColor(m_ThemeMode, BasicTextDisable));
    }
    switch (bopt->toolButtonStyle) {
    case Qt::ToolButtonTextOnly : {
      contentRect.setLeft(contentRect.left() + m_ContentMargin);
      painter->drawText(contentRect, Qt::AlignLeft | Qt::AlignVCenter, bopt->text);
      break;
    }
    case Qt::ToolButtonTextBesideIcon : {
      painter->drawText(QRect(contentRect.x() + m_ContentMargin * 2 + bopt->iconSize.width(), contentRect.y(), contentRect.width() - bopt->iconSize.width(), contentRect.height()), Qt::AlignLeft | Qt::AlignVCenter, bopt->text);
      break;
    }
    case Qt::ToolButtonTextUnderIcon : {
      if (bopt->features.testFlag(QStyleOptionToolButton::HasMenu) &&
          !bopt->features.testFlag(QStyleOptionToolButton::MenuButtonPopup)) {
        contentRect.setLeft(contentRect.left() + m_ContentMargin);
        painter->drawText(contentRect, Qt::AlignBottom | Qt::AlignLeft, bopt->text);
      } else {
        painter->drawText(contentRect, Qt::AlignBottom | Qt::AlignHCenter, bopt->text);
      }
      break;
    }
    case Qt::ToolButtonFollowStyle : {
      break;
    }
    default : {
      break;
    }
    }
  }
}

qreal NXToolButtonStyle::_calculateExpandIndicatorWidth(QStyleOptionToolButton const *bopt, QPainter *painter) const
{
  // 展开指示器
  QSize iconSize = bopt->iconSize;
  painter->save();
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(0.75 * std::min(iconSize.width(), iconSize.height()));
  painter->setFont(iconFont);
  int indicatorWidth = painter->fontMetrics().horizontalAdvance(QChar((unsigned short) NXIconType::AngleDown));
  painter->restore();
  return indicatorWidth;
}
