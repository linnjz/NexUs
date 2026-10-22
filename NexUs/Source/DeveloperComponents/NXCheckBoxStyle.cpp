#include "NXCheckBoxStyle.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>

#include "NXTheme.h"

NXCheckBoxStyle::NXCheckBoxStyle(QStyle *style)
{
  _pCheckIndicatorWidth = 21;
  _pBorderRadius        = 2;
  _themeMode            = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXCheckBoxStyle::~NXCheckBoxStyle()
{
}

void NXCheckBoxStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
  // qDebug() << element << option->rect;
  switch (element) {
  case QStyle::CE_CheckBox : {
    if (const QStyleOptionButton *bopt = qstyleoption_cast<const QStyleOptionButton *>(option)) {
      bool isEnabled = bopt->state.testFlag(QStyle::State_Enabled);
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
      QRect checkRect = subElementRect(SE_CheckBoxIndicator, bopt, widget);
      checkRect.adjust(1, 1, -1, -1);
      //复选框绘制
      painter->setPen(Qt::NoPen);
      if (bopt->state.testFlag(QStyle::State_On) || bopt->state.testFlag(QStyle::State_NoChange)) {
        painter->setPen(Qt::NoPen);
        if (bopt->state.testFlag(QStyle::State_Sunken)) {
          painter->setBrush(NXThemeColor(_themeMode, PrimaryPress));
        } else {
          if (bopt->state.testFlag(QStyle::State_MouseOver)) {
            painter->setBrush(NXThemeColor(_themeMode, PrimaryHover));
          } else {
            painter->setBrush(NXThemeColor(_themeMode, PrimaryNormal));
          }
        }
      } else {
        if (bopt->state.testFlag(QStyle::State_Sunken)) {
          painter->setPen(NXThemeColor(_themeMode, BasicBorderDeep));
          painter->setBrush(NXThemeColor(_themeMode, BasicPress));
        } else {
          painter->setPen(NXThemeColor(_themeMode, BasicBorderDeep));
          if (bopt->state.testFlag(QStyle::State_MouseOver)) {
            painter->setBrush(NXThemeColor(_themeMode, BasicHover));
          } else {
            painter->setBrush(NXThemeColor(_themeMode, BasicBase));
          }
        }
      }
      painter->drawRoundedRect(checkRect, _pBorderRadius, _pBorderRadius);
      //图标绘制
      painter->setPen(NXThemeColor(NXThemeType::Dark, BasicText));
      if (bopt->state.testFlag(QStyle::State_On)) {
        painter->save();
        QFont iconFont = QFont(QStringLiteral("NXAwesome"));
        iconFont.setPixelSize(_pCheckIndicatorWidth * 0.75);
        painter->setFont(iconFont);
        painter->drawText(checkRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Check));
        painter->restore();
      } else if (bopt->state.testFlag(QStyle::State_NoChange)) {
        QLine checkLine(checkRect.x() + 3, checkRect.center().y(), checkRect.right() - 3, checkRect.center().y());
        painter->drawLine(checkLine);
      }
      //文字绘制
      painter->setPen(isEnabled ? NXThemeColor(_themeMode, BasicText) : NXThemeColor(_themeMode, BasicTextDisable));
      QRect textRect = subElementRect(SE_CheckBoxContents, bopt, widget);
      painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, bopt->text);
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

int NXCheckBoxStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
  switch (metric) {
  case QStyle::PM_IndicatorWidth :
  case QStyle::PM_IndicatorHeight : {
    return _pCheckIndicatorWidth;
  }
  case QStyle::PM_CheckBoxLabelSpacing : {
    return _labelSpacing;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}

QRect NXCheckBoxStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
  QRect subRect = QProxyStyle::subElementRect(element, option, widget);
  if (!widget) {
    return subRect;
  }
  if (element == SE_CheckBoxIndicator) {
    subRect.setSize(QSize(_pCheckIndicatorWidth, _pCheckIndicatorWidth));
  } else if (element == SE_CheckBoxContents) {
    const QRect indicatorRect = subElementRect(SE_CheckBoxIndicator, option, widget);
    subRect.setLeft(indicatorRect.right() + _labelSpacing);
  }
  return subRect;
}
