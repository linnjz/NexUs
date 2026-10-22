#include "NXSpinBoxStyle.h"

#include <cmath>

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionSpinBox>

#include "NXApplication.h"
#include "NXTheme.h"

NXSpinBoxStyle::NXSpinBoxStyle(QStyle *style)
{
  _pButtonMode = NXSpinBoxType::Inline;
  _themeMode   = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXSpinBoxStyle::~NXSpinBoxStyle()
{
}

void NXSpinBoxStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
  switch (control) {
  case QStyle::CC_SpinBox : {
    const QStyleOptionSpinBox *sopt = qstyleoption_cast<const QStyleOptionSpinBox *>(option);
    if (!sopt) {
      break;
    }
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    //背景
    QRect spinBoxRect = sopt->rect.adjusted(1, 1, -1, -1);
    painter->setPen(NXThemeColor(_themeMode, BasicBorder));
    bool isEnable = sopt->state.testFlag(QStyle::State_Enabled);
    if (isEnable) {
      if (sopt->state & QStyle::State_MouseOver) {
        painter->setBrush(NXThemeColor(_themeMode, BasicHover));
      } else {
        painter->setBrush(NXThemeColor(_themeMode, BasicBase));
      }
    } else {
      painter->setBrush(NXThemeColor(_themeMode, BasicDisable));
    }
    painter->drawRoundedRect(spinBoxRect, 4, 4);
    //添加按钮
    QRect addLineRect = subControlRect(control, sopt, SC_ScrollBarAddLine, widget);
    //减少按钮
    QRect subLineRect = subControlRect(control, sopt, SC_ScrollBarSubLine, widget);
    if (isEnable) {
      //添加按钮
      if (sopt->activeSubControls == SC_ScrollBarAddLine) {
        painter->setPen(Qt::NoPen);
        if (sopt->state & QStyle::State_Sunken && sopt->state & QStyle::State_MouseOver) {
          painter->setBrush(NXThemeColor(_themeMode, BasicPressAlpha));
        } else {
          if (sopt->state & QStyle::State_MouseOver) {
            painter->setBrush(NXThemeColor(_themeMode, BasicHoverAlpha));
          }
        }
        painter->drawRoundedRect(addLineRect, 4, 4);
      }
      //减少按钮
      if (sopt->activeSubControls == SC_ScrollBarSubLine) {
        painter->setPen(Qt::NoPen);
        if (sopt->state & QStyle::State_Sunken && sopt->state & QStyle::State_MouseOver) {
          painter->setBrush(NXThemeColor(_themeMode, BasicPressAlpha));
        } else {
          if (sopt->state & QStyle::State_MouseOver) {
            painter->setBrush(NXThemeColor(_themeMode, BasicHoverAlpha));
          }
        }
        painter->drawRoundedRect(subLineRect, 4, 4);
      }
    }
    //底边线
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(_themeMode, BasicHemline));
    QPainterPath path;
    path.moveTo(4, spinBoxRect.y() + spinBoxRect.height());
    path.lineTo(spinBoxRect.width() - 4, spinBoxRect.y() + spinBoxRect.height());
    path.arcTo(QRectF(spinBoxRect.width() - 8, spinBoxRect.y() + spinBoxRect.height() - 8, 8, 8), -90, 45);
    path.lineTo(4 - 2 * std::sqrt(2), spinBoxRect.y() + spinBoxRect.height() - (4 - 2 * std::sqrt(2)));
    path.arcTo(QRectF(0, spinBoxRect.y() + spinBoxRect.height() - 8, 8, 8), 225, 45);
    path.closeSubpath();
    painter->drawPath(path);

    //添加图标
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(nxApp->getFontPixelSize() + 4);
    painter->setFont(iconFont);
    painter->setPen(isEnable ? NXThemeColor(_themeMode, BasicText) : NXThemeColor(_themeMode, BasicTextDisable));
    painter->drawText(addLineRect, Qt::AlignCenter, _pButtonMode == NXSpinBoxType::PMSide ? QChar((unsigned short) NXIconType::Plus) : QChar((unsigned short) NXIconType::AngleUp));
    //减小图标
    painter->drawText(subLineRect, Qt::AlignCenter, _pButtonMode == NXSpinBoxType::PMSide ? QChar((unsigned short) NXIconType::Minus) : QChar((unsigned short) NXIconType::AngleDown));
    painter->restore();
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawComplexControl(control, option, painter, widget);
}

QRect NXSpinBoxStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget) const
{
  QRect rect = QProxyStyle::subControlRect(cc, opt, sc, widget);
  switch (cc) {
  case CC_SpinBox : {
    switch (sc) {
    case SC_ScrollBarAddLine : {
      //增加按钮
      QRect spinBoxRect = QProxyStyle::subControlRect(cc, opt, SC_SpinBoxFrame, widget).adjusted(1, 1, -1, -1);
      switch (_pButtonMode) {
      case NXSpinBoxType::Inline : {
        return QRect(spinBoxRect.width() - spinBoxRect.height(), spinBoxRect.y(), spinBoxRect.height(), spinBoxRect.height())
            .adjusted(3, 4, -3, -5);
      }
      case NXSpinBoxType::Compact : {
        return QRect(spinBoxRect.width() - spinBoxRect.height(), spinBoxRect.y(), spinBoxRect.height(), spinBoxRect.height() / 2)
            .adjusted(3, 4, -3, 0);
      }
      case NXSpinBoxType::Side :
      case NXSpinBoxType::PMSide : {
        return QRect(spinBoxRect.width() - spinBoxRect.height(), spinBoxRect.y(), spinBoxRect.height(), spinBoxRect.height())
            .adjusted(3, 4, -3, -5);
      }
      }
    }
    case SC_ScrollBarSubLine : {
      //减少按钮
      QRect spinBoxRect = QProxyStyle::subControlRect(cc, opt, SC_SpinBoxFrame, widget).adjusted(1, 1, -1, -1);
      switch (_pButtonMode) {
      case NXSpinBoxType::Inline : {
        return QRect(spinBoxRect.width() - 2 * spinBoxRect.height(), spinBoxRect.y(), spinBoxRect.height(), spinBoxRect.height())
            .adjusted(6, 4, 0, -5);
      }
      case NXSpinBoxType::Compact : {
        return QRect(spinBoxRect.width() - spinBoxRect.height(), spinBoxRect.center().y(), spinBoxRect.height(), spinBoxRect.height() / 2)
            .adjusted(3, 0, -3, -5);
      }
      case NXSpinBoxType::Side :
      case NXSpinBoxType::PMSide : {
        return QRect(spinBoxRect.x(), spinBoxRect.y(), spinBoxRect.height(), spinBoxRect.height())
            .adjusted(3, 4, -3, -5);
      }
      }
    }
    case SC_SpinBoxEditField : {
      QRect spinBoxRect = QProxyStyle::subControlRect(cc, opt, SC_SpinBoxFrame, widget);
      switch (_pButtonMode) {
      case NXSpinBoxType::Inline : {
        return { spinBoxRect.x(), spinBoxRect.y(), spinBoxRect.width() - 2 * spinBoxRect.height() + 6,
                 spinBoxRect.height() };
      }
      case NXSpinBoxType::Compact : {
        return { spinBoxRect.x(), spinBoxRect.y(), spinBoxRect.width() - spinBoxRect.height() + 3,
                 spinBoxRect.height() };
      }
      case NXSpinBoxType::Side :
      case NXSpinBoxType::PMSide : {
        return { spinBoxRect.height(), spinBoxRect.y(), spinBoxRect.width() - 2 * spinBoxRect.height(),
                 spinBoxRect.height() };
      }
      }
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
  return rect;
}
