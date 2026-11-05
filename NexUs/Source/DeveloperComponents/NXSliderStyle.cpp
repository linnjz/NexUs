#include "NXSliderStyle.h"

#include <QPainter>
#include <QPropertyAnimation>
#include <QStyleOptionSlider>

#include "NXTheme.h"

NXSliderStyle::NXSliderStyle(QStyle *style)
    : m_ThemeMode { nxTheme->getThemeMode() }
{
  setProperty("circleRadius", 0.01);
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXSliderStyle::~NXSliderStyle()
{
}

void NXSliderStyle::drawComplexControl(ComplexControl control, QStyleOptionComplex const *option, QPainter *painter, QWidget const *widget) const
{
  switch (control) {
  case QStyle::CC_Slider : {
    QStyleOptionSlider const *sopt = qstyleoption_cast<QStyleOptionSlider const *>(option);
    if (!sopt) {
      break;
    }

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QRect sliderRect       = sopt->rect;
    QRect sliderHandleRect = subControlRect(control, sopt, SC_SliderHandle, widget);
    sliderHandleRect.adjust(1, 1, -1, -1);
    // 滑槽
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(m_ThemeMode, BasicChute));
    if (sopt->orientation == Qt::Horizontal) {
      // 未滑过
      painter->drawRoundedRect(QRect(sliderRect.x() + sliderRect.height() / 8, sliderRect.y() + sliderRect.height() * 0.375, sliderRect.width() - sliderRect.height() / 4, sliderRect.height() / 4), sliderRect.height() / 8, sliderRect.height() / 8);
      // 已滑过
      painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
      painter->drawRoundedRect(QRect(sliderRect.x() + sliderRect.height() / 8, sliderRect.y() + sliderRect.height() * 0.375, sliderHandleRect.x(), sliderRect.height() / 4), sliderRect.height() / 8, sliderRect.height() / 8);
    } else {
      // 未滑过
      painter->drawRoundedRect(QRect(sliderRect.x() + sliderRect.width() * 0.375, sliderRect.y() + sliderRect.width() / 8, sliderRect.width() / 4, sliderRect.height() - sliderRect.width() / 4), sliderRect.width() / 8, sliderRect.width() / 8);
      // 已滑过
      painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
      painter->drawRoundedRect(QRect(sliderRect.x() + sliderRect.width() * 0.375, sliderHandleRect.y(), sliderRect.width() / 4, sliderRect.height() - sliderRect.width() / 8 - sliderHandleRect.y()), sliderRect.width() / 8, sliderRect.width() / 8);
    }
    // 滑块
    // 外圆形
    painter->setPen(NXThemeColor(m_ThemeMode, BasicBorder));
    painter->setBrush(NXThemeColor(m_ThemeMode, BasicBase));
    painter->drawEllipse(QPointF(sliderHandleRect.center().x() + 1, sliderHandleRect.center().y() + 1), sliderHandleRect.width() / 2, sliderHandleRect.width() / 2);
    // 内圆形
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
    if (m_LastState == 0) {
      m_LastState = sopt->state;
    }
    if (m_CircleRadius == 0) {
      m_CircleRadius = sliderHandleRect.width() / 3.8;
    }
    if (sopt->activeSubControls == SC_SliderHandle) {
      if (sopt->state & QStyle::State_Sunken) {
        if (sopt->state & QStyle::State_MouseOver) {
          if (!m_LastState.testFlag(QStyle::State_Sunken)) {
            _startRadiusAnimation(sliderHandleRect.width() / 2.8, sliderHandleRect.width() / 4.5, const_cast<QWidget *>(widget));
            m_LastState = sopt->state;
          }
          painter->drawEllipse(QPointF(sliderHandleRect.center().x() + 1, sliderHandleRect.center().y() + 1), m_CircleRadius, m_CircleRadius);
        }
      } else {
        if (sopt->state & QStyle::State_MouseOver) {
          if (!m_LastState.testFlag(QStyle::State_MouseOver)) {
            _startRadiusAnimation(m_CircleRadius, sliderHandleRect.width() / 2.8, const_cast<QWidget *>(widget));
            m_LastState = sopt->state;
          }
          if (m_LastState.testFlag(QStyle::State_Sunken)) {
            _startRadiusAnimation(m_CircleRadius, sliderHandleRect.width() / 2.8, const_cast<QWidget *>(widget));
            m_LastState = sopt->state;
          }
          painter->drawEllipse(QPointF(sliderHandleRect.center().x() + 1, sliderHandleRect.center().y() + 1), m_CircleRadius, m_CircleRadius);
        }
      }
    } else {
      if (m_LastState.testFlag(QStyle::State_MouseOver) || m_LastState.testFlag(QStyle::State_Sunken)) {
        _startRadiusAnimation(m_CircleRadius, sliderHandleRect.width() / 3.8, const_cast<QWidget *>(widget));
        m_LastState &= ~QStyle::State_MouseOver;
        m_LastState &= ~QStyle::State_Sunken;
      }
      painter->drawEllipse(QPointF(sliderHandleRect.center().x() + 1, sliderHandleRect.center().y() + 1), m_CircleRadius, m_CircleRadius);
    }
    painter->restore();
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawComplexControl(control, option, painter, widget);
}

int NXSliderStyle::pixelMetric(PixelMetric metric, QStyleOption const *option, QWidget const *widget) const
{
  switch (metric) {
  case QStyle::PM_SliderLength : {
    return 20;
  }
  case QStyle::PM_SliderThickness : {
    return 20;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}

int NXSliderStyle::styleHint(StyleHint hint, QStyleOption const *option, QWidget const *widget, QStyleHintReturn *returnData) const
{
  if (hint == QStyle::SH_Slider_AbsoluteSetButtons) {
    return Qt::LeftButton;
  }
  return QProxyStyle::styleHint(hint, option, widget, returnData);
}

void NXSliderStyle::_startRadiusAnimation(qreal startRadius, qreal endRadius, QWidget *widget) const
{
  NXSliderStyle *style                      = const_cast<NXSliderStyle *>(this);
  QPropertyAnimation *circleRadiusAnimation = new QPropertyAnimation(style, "circleRadius");
  connect(circleRadiusAnimation, &QPropertyAnimation::valueChanged, style, [=](QVariant const &value) {
    this->m_CircleRadius = value.toReal();
    widget->update();
  });
  circleRadiusAnimation->setEasingCurve(QEasingCurve::InOutSine);
  circleRadiusAnimation->setStartValue(startRadius);
  circleRadiusAnimation->setEndValue(endRadius);
  circleRadiusAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}
