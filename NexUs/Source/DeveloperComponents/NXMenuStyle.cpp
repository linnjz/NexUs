#include "NXMenuStyle.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include "NXApplication.h"
#include "NXMenu.h"
#include "NXTheme.h"

NXMenuStyle::NXMenuStyle(QStyle *style)
    : m_MenuItemHeight { 32 }
    , m_BorderRadius { 6 }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXMenuStyle::~NXMenuStyle()
{
}

void NXMenuStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::PE_PanelMenu : {
    // 高性能阴影
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
    QRect foregroundRect = option->rect;
#else
    nxTheme->drawEffectShadow(painter, option->rect, m_ShadowBorderWidth, 6);
    // 背景绘制
    QRect foregroundRect(m_ShadowBorderWidth, m_ShadowBorderWidth, option->rect.width() - 2 * m_ShadowBorderWidth, option->rect.height() - 2 * m_ShadowBorderWidth);
#endif
    painter->setPen(NXThemeColor(m_ThemeMode, PopupBorder));
    painter->setBrush(NXThemeColor(m_ThemeMode, PopupBase));
    painter->drawRoundedRect(foregroundRect, m_BorderRadius, m_BorderRadius);
    painter->restore();
    return;
  }
  case QStyle::PE_FrameMenu : {
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void NXMenuStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::CE_MenuItem : {
    //内容绘制 区分类型
    if (QStyleOptionMenuItem const *mopt = qstyleoption_cast<QStyleOptionMenuItem const *>(option)) {
      if (mopt->menuItemType == QStyleOptionMenuItem::Separator) {
        QRect separatorRect = mopt->rect;
        painter->save();
        painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicBaseLine));
        painter->drawRoundedRect(QRectF(separatorRect.x() + separatorRect.width() * 0.055, separatorRect.center().y(), separatorRect.width() - separatorRect.width() * 0.11, 1.5), 1, 1);
        painter->restore();
        return;
      } else {
        QRect menuRect        = mopt->rect;
        qreal contentPadding  = menuRect.width() * 0.055;
        qreal textLeftSpacing = 8;
        painter->save();
        painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
        bool isHover = mopt->state.testFlag(QStyle::State_Enabled) &&
                       (mopt->state.testFlag(QStyle::State_MouseOver) || mopt->state.testFlag(QStyle::State_Selected));
        //覆盖效果
        if (isHover) {
          QRect hoverRect = menuRect;
          hoverRect.adjust(0, 2, 0, -2);
          painter->setPen(Qt::NoPen);
          painter->setBrush(NXThemeColor(m_ThemeMode, PopupHover));
          painter->drawRoundedRect(hoverRect, 5, 5);

          // 标识
          qreal heightOffset = hoverRect.height() / 5;
          painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
          painter->drawRoundedRect(
              QRectF(hoverRect.x() + 3, hoverRect.y() + heightOffset, 3, hoverRect.height() - 2 * heightOffset), 3, 3);
        }
        //Icon绘制
        QIcon menuIcon = mopt->icon;
        //check绘制
        if (mopt->menuHasCheckableItems) {
          painter->save();
          painter->setPen(!mopt->state.testFlag(QStyle::State_Enabled) ? Qt::gray : m_ThemeMode == NXThemeType::Light ? Qt::black
                                                                                                                     : Qt::white);
          QFont iconFont = QFont(QStringLiteral("NXAwesome"));
          iconFont.setPixelSize(m_MenuItemHeight * 0.57);
          painter->setFont(iconFont);
          painter->drawText(
              QRectF(menuRect.x() + contentPadding, menuRect.y(), m_IconWidth, menuRect.height()), Qt::AlignCenter,
              mopt->checked ? QChar((unsigned short) NXIconType::Check) : QChar((unsigned short) NXIconType::None));
          painter->restore();
        } else {
          QString iconText;
          NXMenu const *menu = dynamic_cast<NXMenu const *>(widget);
          if (menu) {
            QAction *action = menu->actionAt(menuRect.center());
            if (action) {
              iconText = action->property("NXIconType").toString();
            }
          }
          if (!iconText.isEmpty()) {
            painter->save();
            isHover
                ? painter->setPen(!mopt->state.testFlag(QStyle::State_Enabled) ? Qt::gray : NXThemeColor(m_ThemeMode, PrimaryPress))
                : painter->setPen(!mopt->state.testFlag(QStyle::State_Enabled) ? Qt::gray : m_ThemeMode == NXThemeType::Light ? Qt::black
                                                                                                                             : Qt::white);
            QFont iconFont = QFont(QStringLiteral("NXAwesome"));
            iconFont.setPixelSize(m_MenuItemHeight * 0.57);
            painter->setFont(iconFont);

            qreal ditherOffset = isHover ? m_MenuItemHeight * 0.021 : 0;
            painter->drawText(QRectF(menuRect.x() + contentPadding + ditherOffset, menuRect.y() - ditherOffset * 1.5, m_IconWidth, menuRect.height()), Qt::AlignCenter, iconText);
            painter->restore();
          } else {
            if (!menuIcon.isNull()) {
              painter->drawPixmap(
                  QRect(menuRect.x() + contentPadding, menuRect.center().y() - m_IconWidth / 2, m_IconWidth, m_IconWidth),
                  menuIcon.pixmap(m_IconWidth, m_IconWidth));
            }
          }
        }
        //文字和快捷键绘制
        if (!mopt->text.isEmpty()) {
          QStringList textList = mopt->text.split(QStringLiteral("\t"));
          isHover
              ? painter->setPen(!mopt->state.testFlag(QStyle::State_Enabled) ? Qt::gray : NXThemeColor(m_ThemeMode, PrimaryHover))
              : painter->setPen(!mopt->state.testFlag(QStyle::State_Enabled) ? Qt::gray : m_ThemeMode == NXThemeType::Light ? Qt::black
                                                                                                                           : Qt::white);
          qreal ditherOffset = isHover ? m_MenuItemHeight * 0.021 : 0;
          painter->drawText(QRectF(menuRect.x() + (m_IsAnyoneItemHasIcon ? contentPadding + textLeftSpacing : 0) + m_IconWidth + ditherOffset, menuRect.y() - ditherOffset, menuRect.width(), menuRect.height()), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, textList[0]);
          if (textList.count() > 1) {
            painter->drawText(QRectF(menuRect.x() + contentPadding + m_IconWidth + textLeftSpacing, menuRect.y(), menuRect.width() - (contentPadding * 2 + m_IconWidth + textLeftSpacing), menuRect.height()), Qt::AlignRight | Qt::AlignVCenter | Qt::TextSingleLine, textList[1]);
          }
        }
        //展开图标
        if (mopt->menuItemType == QStyleOptionMenuItem::SubMenu) {
          painter->save();
          painter->setPen(!mopt->state.testFlag(QStyle::State_Enabled) ? Qt::gray : m_ThemeMode == NXThemeType::Light ? Qt::black
                                                                                                                     : Qt::white);
          QFont iconFont = QFont(QStringLiteral("NXAwesome"));
          iconFont.setPixelSize(nxApp->getFontPixelSize() + 5);
          painter->setFont(iconFont);
          painter->drawText(QRect(menuRect.right() - 25, menuRect.y(), 25, menuRect.height()), Qt::AlignVCenter, QChar((unsigned short) NXIconType::AngleRight));
          painter->restore();
        }
        painter->restore();
      }
    }

    return;
  }
  case QStyle::CE_MenuEmptyArea : {
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawControl(element, option, painter, widget);
}

int NXMenuStyle::pixelMetric(PixelMetric metric, QStyleOption const *option, QWidget const *widget) const
{
  switch (metric) {
  case QStyle::PM_SmallIconSize : {
    //图标宽度
    m_IconWidth = m_MenuItemHeight * 0.7;
    return m_IconWidth;
  }
  case QStyle::PM_MenuPanelWidth : {
    //外围容器宽度
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
    return 0;
#else
    return m_ShadowBorderWidth * 1.5;
#endif
  }
  default : {
    break;
  }
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}

QSize NXMenuStyle::sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const
{
  switch (type) {
  case QStyle::CT_MenuItem : {
    if (QStyleOptionMenuItem const *mopt = qstyleoption_cast<QStyleOptionMenuItem const *>(option)) {
      if (mopt->menuItemType == QStyleOptionMenuItem::Separator) {
        break;
      }
      QSize menuItemSize = QProxyStyle::sizeFromContents(type, option, size, widget);
      NXMenu const *menu = dynamic_cast<NXMenu const *>(widget);
      int extraWidth     = 0;
      if (menu->isHasIcon() || mopt->menuHasCheckableItems) {
        m_IsAnyoneItemHasIcon = true;
        // 为图标、间距和内边距预留额外空间
        int iconWidth       = m_MenuItemHeight * 0.7;
        int textLeftSpacing = 8;
        extraWidth          = iconWidth + textLeftSpacing;
      }
      if (menu->isHasChildMenu()) {
        return QSize(menuItemSize.width() + extraWidth + 20, m_MenuItemHeight);
      } else {
        return QSize(menuItemSize.width() + extraWidth, m_MenuItemHeight);
      }
    }
  }
  default : {
    break;
  }
  }
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}
