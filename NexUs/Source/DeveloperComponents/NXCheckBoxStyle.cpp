#include "NXCheckBoxStyle.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>

#include "NXTheme.h"

namespace
{
static constexpr int s_LabelSpacing { 10 };
}

NXCheckBoxStyle::NXCheckBoxStyle(QStyle *style)
    : m_CheckIndicatorWidth { 21 }
    , m_BorderRadius { 2 }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXCheckBoxStyle::~NXCheckBoxStyle()
{
}

void NXCheckBoxStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::CE_CheckBox : {
    if (QStyleOptionButton const *bopt = qstyleoption_cast<QStyleOptionButton const *>(option)) {
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
          painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryPress));
        } else {
          if (bopt->state.testFlag(QStyle::State_MouseOver)) {
            painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryHover));
          } else {
            painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
          }
        }
      } else {
        if (bopt->state.testFlag(QStyle::State_Sunken)) {
          painter->setPen(NXThemeColor(m_ThemeMode, BasicBorderDeep));
          painter->setBrush(NXThemeColor(m_ThemeMode, BasicPress));
        } else {
          painter->setPen(NXThemeColor(m_ThemeMode, BasicBorderDeep));
          if (bopt->state.testFlag(QStyle::State_MouseOver)) {
            painter->setBrush(NXThemeColor(m_ThemeMode, BasicHover));
          } else {
            painter->setBrush(NXThemeColor(m_ThemeMode, BasicBase));
          }
        }
      }
      painter->drawRoundedRect(checkRect, m_BorderRadius, m_BorderRadius);
      //图标绘制
      painter->setPen(NXThemeColor(NXThemeType::Dark, BasicText));
      if (bopt->state.testFlag(QStyle::State_On)) {
        painter->save();
        QFont iconFont = QFont(QStringLiteral("NXAwesome"));
        iconFont.setPixelSize(m_CheckIndicatorWidth * 0.75);
        painter->setFont(iconFont);
        painter->drawText(checkRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Check));
        painter->restore();
      } else if (bopt->state.testFlag(QStyle::State_NoChange)) {
        QLine checkLine(checkRect.x() + 3, checkRect.center().y(), checkRect.right() - 3, checkRect.center().y());
        painter->drawLine(checkLine);
      }
      //文字绘制
      painter->setPen(isEnabled ? NXThemeColor(m_ThemeMode, BasicText) : NXThemeColor(m_ThemeMode, BasicTextDisable));
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

int NXCheckBoxStyle::pixelMetric(PixelMetric metric, QStyleOption const *option, QWidget const *widget) const
{
  switch (metric) {
  case QStyle::PM_IndicatorWidth :
  case QStyle::PM_IndicatorHeight : {
    return m_CheckIndicatorWidth;
  }
  case QStyle::PM_CheckBoxLabelSpacing : {
    return s_LabelSpacing;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}

QRect NXCheckBoxStyle::subElementRect(SubElement element, QStyleOption const *option, QWidget const *widget) const
{
  QRect subRect = QProxyStyle::subElementRect(element, option, widget);
  if (!widget) {
    return subRect;
  }
  if (element == SE_CheckBoxIndicator) {
    subRect.setSize(QSize(m_CheckIndicatorWidth, m_CheckIndicatorWidth));
  } else if (element == SE_CheckBoxContents) {
    QRect const indicatorRect = subElementRect(SE_CheckBoxIndicator, option, widget);
    subRect.setLeft(indicatorRect.right() + s_LabelSpacing);
  }
  return subRect;
}
