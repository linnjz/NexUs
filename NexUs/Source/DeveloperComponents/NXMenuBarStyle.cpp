#include "NXMenuBarStyle.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>

#include "NXApplication.h"
#include "NXMenuBar.h"
#include "NXTheme.h"

NXMenuBarStyle::NXMenuBarStyle(QStyle *style)
    : m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXMenuBarStyle::~NXMenuBarStyle()
{
}

void NXMenuBarStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::PE_PanelButtonTool : {
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void NXMenuBarStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::CE_ToolButtonLabel : {
    if (QStyleOptionToolButton const *topt = qstyleoption_cast<QStyleOptionToolButton const *>(option)) {
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
      painter->setPen(Qt::NoPen);
      if (topt->state.testFlag(QStyle::State_Enabled) && topt->state.testFlag(QStyle::State_MouseOver)) {
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicHoverAlpha));
        painter->drawRect(topt->rect);
      }
      //展开图标
      painter->setPen(!topt->state.testFlag(QStyle::State_Enabled) ? Qt::gray : NXThemeColor(m_ThemeMode, BasicText));
      QFont iconFont = QFont(QStringLiteral("NXAwesome"));
      iconFont.setPixelSize(nxApp->getFontPixelSize() + 5);
      painter->setFont(iconFont);
      painter->drawText(topt->rect, Qt::AlignCenter, QChar((unsigned short) NXIconType::AngleRight));
      painter->restore();
    }
    return;
  }
  case QStyle::CE_MenuBarEmptyArea : {
    return;
  }
  case QStyle::CE_MenuBarItem : {
    if (QStyleOptionMenuItem const *mopt = qstyleoption_cast<QStyleOptionMenuItem const *>(option)) {
      if (mopt->menuItemType == QStyleOptionMenuItem::Separator) {
        return;
      }
      int menuBarHeight  = widget->height();
      QRect menuItemRect = mopt->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
      //背景绘制
      painter->setPen(Qt::NoPen);
      if (mopt->state.testFlag(QStyle::State_Enabled)) {
        if (mopt->state.testFlag(QStyle::State_Sunken)) {
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicPressAlpha));
        } else if (mopt->state.testFlag(QStyle::State_Selected)) {
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicSelectedAlpha));
        }
        painter->drawRoundedRect(menuItemRect, 3, 3);
      }
      //图标和文字绘制
      QIcon icon       = mopt->icon;
      QString menuText = mopt->text;
      QString iconText;
      menuText                 = menuText.replace(QStringLiteral("&"), QStringLiteral(""));
      NXMenuBar const *menuBar = dynamic_cast<NXMenuBar const *>(widget);
      if (menuBar) {
        QAction *action = menuBar->actionAt(menuItemRect.center());
        if (action) {
          iconText = action->property("NXIconType").toString();
        }
      }
      if (menuText.isEmpty()) {
        //图标
        if (!iconText.isEmpty()) {
          painter->save();
          painter->setPen(!mopt->state.testFlag(QStyle::State_Enabled) ? NXThemeColor(m_ThemeMode, BasicTextDisable) : NXThemeColor(m_ThemeMode, BasicText));
          QFont iconFont = QFont(QStringLiteral("NXAwesome"));
          iconFont.setPixelSize(menuBarHeight * 0.7);
          painter->setFont(iconFont);
          painter->drawText(menuItemRect, Qt::AlignCenter, iconText);
          painter->restore();
        } else if (!icon.isNull()) {
          QRect menuIconRect(menuItemRect.center().x() - menuBarHeight * 0.4, menuItemRect.center().y() - menuBarHeight * 0.4, menuBarHeight * 0.8, menuBarHeight * 0.8);
          painter->drawPixmap(menuIconRect, icon.pixmap(menuBarHeight * 0.8, menuBarHeight * 0.8));
        }
      } else {
        //图标 + 文字
        painter->setPen(!mopt->state.testFlag(QStyle::State_Enabled) ? NXThemeColor(m_ThemeMode, BasicTextDisable) : NXThemeColor(m_ThemeMode, BasicText));
        if (icon.isNull() && iconText.isEmpty()) {
          painter->drawText(menuItemRect, Qt::AlignCenter, menuText);
        } else {
          if (!iconText.isEmpty()) {
            QRectF menuIconRect(menuItemRect.x() + menuBarHeight * 0.1 + m_MenuBarItemMargin, menuBarHeight * 0.1, menuBarHeight * 0.8, menuBarHeight * 0.8);
            painter->save();
            QFont iconFont = QFont(QStringLiteral("NXAwesome"));
            iconFont.setPixelSize(menuBarHeight * 0.7);
            painter->setFont(iconFont);
            painter->drawText(menuIconRect, Qt::AlignCenter, iconText);
            painter->restore();
            painter->drawText(QRect(menuIconRect.right(), menuItemRect.y(), menuItemRect.width() - menuIconRect.width(), menuBarHeight), Qt::AlignCenter, menuText);
          } else {
            QRect menuIconRect(menuItemRect.x() + menuBarHeight * 0.1 + m_MenuBarItemMargin, menuBarHeight * 0.1, menuBarHeight * 0.8, menuBarHeight * 0.8);
            painter->drawPixmap(menuIconRect, icon.pixmap(menuBarHeight * 0.8, menuBarHeight * 0.8));
            painter->drawText(QRect(menuIconRect.right(), menuItemRect.y(), menuItemRect.width() - menuIconRect.width(), menuBarHeight), Qt::AlignCenter, menuText);
          }
        }
      }
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

QSize NXMenuBarStyle::sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const
{
  switch (type) {
  case QStyle::CT_MenuBar : {
    QSize menuBarHeight = QProxyStyle::sizeFromContents(type, option, size, widget);
    menuBarHeight.setHeight(widget->height());
    return menuBarHeight;
  }
  case QStyle::CT_MenuBarItem : {
    if (QStyleOptionMenuItem const *mopt = qstyleoption_cast<QStyleOptionMenuItem const *>(option)) {
      QSize menuBarItemSize = QProxyStyle::sizeFromContents(type, option, size, widget);
      menuBarItemSize.setHeight(widget->height());
      if ((!mopt->icon.isNull()) && !mopt->text.isEmpty()) {
        menuBarItemSize.setWidth(menuBarItemSize.width() + mopt->fontMetrics.horizontalAdvance(mopt->text) + 2 * m_MenuBarItemMargin);
      }
      if (!mopt->text.contains(QStringLiteral("&"))) {
        menuBarItemSize.setWidth(menuBarItemSize.width() + 5);
      }
      return menuBarItemSize;
    }
  }
  default : {
    break;
  }
  }
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}

int NXMenuBarStyle::pixelMetric(PixelMetric metric, QStyleOption const *option, QWidget const *widget) const
{
  switch (metric) {
  case QStyle::PM_MenuBarHMargin : {
    return 0;
  }
  case QStyle::PM_MenuBarItemSpacing : {
    return 6;
  }
  case QStyle::PM_ToolBarExtensionExtent : {
    return 26;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}
