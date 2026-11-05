#include "NXComboBoxStyle.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include "NXApplication.h"
#include "NXTheme.h"

NXComboBoxStyle::NXComboBoxStyle(QStyle *style)
    : m_ExpandIconRotate { 0 }
    , m_ExpandMarkWidth { 0 }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXComboBoxStyle::~NXComboBoxStyle()
{
}

int NXComboBoxStyle::getShadowBorderWidth() const
{
  return m_ShadowBorderWidth;
}

void NXComboBoxStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::PE_Widget :
  case QStyle::PE_PanelLineEdit : {
    return;
  }
#ifndef Q_OS_WIN
  case PE_PanelMenu : {
    return;
  }
  case PE_IndicatorArrowDown : {
    return;
  }
#endif
  default : {
    break;
  }
  }
  QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void NXComboBoxStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::CE_ComboBoxLabel : {
    return;
  }
  case QStyle::CE_ShapedFrame : {
    //container区域
    if (widget->objectName() == QStringLiteral("NXComboBoxContainer")) {
      QRect viewRect = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
      QRect foregroundRect = viewRect;
#else
      nxTheme->drawEffectShadow(painter, viewRect, m_ShadowBorderWidth, 6);
      QRect foregroundRect(viewRect.x() + m_ShadowBorderWidth, viewRect.y(), viewRect.width() - 2 * m_ShadowBorderWidth, viewRect.height() - m_ShadowBorderWidth);
#endif
      painter->setPen(NXThemeColor(m_ThemeMode, PopupBorder));
      painter->setBrush(NXThemeColor(m_ThemeMode, PopupBase));
      painter->drawRoundedRect(foregroundRect, 3, 3);
      painter->restore();
    }
    return;
  }
  case QStyle::CE_ItemViewItem : {
    //覆盖高亮
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      int margin = 2;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
      painter->setPen(Qt::NoPen);
      QPainterPath path;
      QRect optionRect = option->rect;
      optionRect.adjust(margin, margin, -margin, -margin);
#ifndef Q_OS_WIN
      optionRect.adjust(6, 0, -6, 0);
#endif
      path.addRoundedRect(optionRect, 5, 5);
      if (option->state & QStyle::State_Selected) {
        if (option->state & QStyle::State_MouseOver) {
          // 选中时覆盖
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicSelectedHoverAlpha));
          painter->drawPath(path);
        } else {
          // 选中
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicSelectedAlpha));
          painter->drawPath(path);
        }
        //选中Mark
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
        painter->drawRoundedRect(QRectF(optionRect.x() + 3, optionRect.y() + optionRect.height() * 0.2, 3, optionRect.height() - +optionRect.height() * 0.4), 1.5, 1.5);
      } else {
        if (option->state & QStyle::State_MouseOver) {
          // 覆盖时颜色
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicHoverAlpha));
          painter->drawPath(path);
        }
      }
      // 文字绘制
      painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
      painter->drawText(
          QRect(option->rect.x() + 15, option->rect.y(), option->rect.width() - 15, option->rect.height()),
          Qt::AlignVCenter, vopt->text);
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

void NXComboBoxStyle::drawComplexControl(ComplexControl control, QStyleOptionComplex const *option, QPainter *painter, QWidget const *widget) const
{
  switch (control) {
  case QStyle::CC_ComboBox : {
    //主体显示绘制
    if (QStyleOptionComboBox const *copt = qstyleoption_cast<QStyleOptionComboBox const *>(option)) {
      painter->save();
      painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
      //背景绘制
      bool isEnabled = copt->state.testFlag(QStyle::State_Enabled);
      painter->setPen(NXThemeColor(m_ThemeMode, BasicBorder));
      painter->setBrush(isEnabled ? (copt->state.testFlag(QStyle::State_HasFocus) && copt->editable) ? NXThemeColor(m_ThemeMode, DialogBase) : copt->state.testFlag(QStyle::State_MouseOver) ? NXThemeColor(m_ThemeMode, BasicHover)
                                                                                                                                                                                            : NXThemeColor(m_ThemeMode, BasicBase)
                                  : NXThemeColor(m_ThemeMode, BasicDisable));
      QRect comboBoxRect = copt->rect;
      comboBoxRect.adjust(1, 1, -1, -1);
      painter->drawRoundedRect(comboBoxRect, 3, 3);
      // 底边线绘制
      painter->setPen(NXThemeColor(m_ThemeMode, BasicBaseLine));
      painter->drawLine(comboBoxRect.x() + 3, comboBoxRect.y() + comboBoxRect.height(), comboBoxRect.x() + comboBoxRect.width() - 3, comboBoxRect.y() + comboBoxRect.height());

      //文字绘制
      if (!copt->editable) {
        QRect textRect = subControlRect(QStyle::CC_ComboBox, copt, QStyle::SC_ScrollBarSubLine, widget);
        painter->setPen(isEnabled ? NXThemeColor(m_ThemeMode, BasicText) : NXThemeColor(m_ThemeMode, BasicTextDisable));
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, copt->currentText);
      }
      //展开指示器绘制
      painter->setPen(Qt::NoPen);
      painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
      painter->drawRoundedRect(QRectF(comboBoxRect.center().x() - m_ExpandMarkWidth, comboBoxRect.y() + comboBoxRect.height() - 3, m_ExpandMarkWidth * 2, 3), 2, 2);
      // 展开图标绘制
      QRect expandIconRect = subControlRect(QStyle::CC_ComboBox, copt, QStyle::SC_ScrollBarAddPage, widget);
      if (expandIconRect.isValid()) {
        QFont iconFont = QFont(QStringLiteral("NXAwesome"));
        iconFont.setPixelSize(nxApp->getFontPixelSize() + 4);
        painter->setFont(iconFont);
        painter->setPen(isEnabled ? NXThemeColor(m_ThemeMode, BasicText) : NXThemeColor(m_ThemeMode, BasicTextDisable));
        painter->translate(expandIconRect.x() + (qreal) expandIconRect.width() / 2, expandIconRect.y() + (qreal) expandIconRect.height() / 2);
        painter->rotate(m_ExpandIconRotate);
        painter->translate(-expandIconRect.x() - (qreal) expandIconRect.width() / 2, -expandIconRect.y() - (qreal) expandIconRect.height() / 2);
        painter->drawText(expandIconRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::AngleDown));
      }
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

QRect NXComboBoxStyle::subControlRect(ComplexControl cc, QStyleOptionComplex const *opt, SubControl sc, QWidget const *widget) const
{
  switch (cc) {
  case QStyle::CC_ComboBox : {
    switch (sc) {
    case QStyle::SC_ScrollBarSubLine : {
      //文字区域
      QRect textRect = QProxyStyle::subControlRect(cc, opt, sc, widget);
      textRect.setLeft(10);
      textRect.setRight(textRect.right() - 15);
      return textRect;
    }
    case QStyle::SC_ScrollBarAddPage : {
      //展开图标区域
      QRect expandIconRect = QProxyStyle::subControlRect(cc, opt, sc, widget);
      expandIconRect.setLeft(expandIconRect.left() - 25);
      return expandIconRect;
    }
    default : {
      break;
    }
    }
    break;
  }
  default : {
    break;
  }
  }

  return QProxyStyle::subControlRect(cc, opt, sc, widget);
}

QSize NXComboBoxStyle::sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const
{
  switch (type) {
  case QStyle::CT_ItemViewItem : {
    QSize itemSize = QProxyStyle::sizeFromContents(type, option, size, widget);
    itemSize.setHeight(35);
    return itemSize;
  }
  case QStyle::CT_ComboBox : {
    QSize comboBoxSize = QProxyStyle::sizeFromContents(type, option, size, widget);
    comboBoxSize.setWidth(comboBoxSize.width() + 26);
    return comboBoxSize;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}
