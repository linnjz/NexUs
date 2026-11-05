#include "NXRadioButtonStyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QStyleOption>

#include "NXTheme.h"

NXRadioButtonStyle::NXRadioButtonStyle(QStyle *style)
    : m_CircleScale { 2.0 / 4.0 }
    , m_ThemeMode { nxTheme->getThemeMode() }
    , m_CircleScaleAnimation { new QPropertyAnimation(this, "CircleScale", this) }
{
  m_CircleScaleAnimation->setDuration(200);
  m_CircleScaleAnimation->setEasingCurve(QEasingCurve::InOutSine);
  connect(this, &NXRadioButtonStyle::signalCircleScaleChanged, this, [this]() {
    if (m_AnimationWidget) {
      m_AnimationWidget->update();
    }
  });

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXRadioButtonStyle::~NXRadioButtonStyle()
{
}

void NXRadioButtonStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case PE_IndicatorRadioButton : {
    QStyleOptionButton const *bopt = qstyleoption_cast<QStyleOptionButton const *>(option);
    if (!bopt) {
      break;
    }
    QRectF buttonRect = bopt->rect;
    buttonRect.adjust(1, 1, -1, -1);
    QPointF const center = buttonRect.center();
    qreal const radius   = qMax<qreal>(0, qMin(buttonRect.width(), buttonRect.height()) / 2);
    const_cast<NXRadioButtonStyle *>(this)->_updateCircleScale(bopt->state, widget);
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (bopt->state & QStyle::State_Off) {
      painter->setPen(QPen(NXThemeColor(m_ThemeMode, BasicBorder), 1.5));
      if (bopt->state & QStyle::State_MouseOver) {
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicHover));
      } else {
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicBase));
      }
      qreal const borderRadius = qMax<qreal>(0, radius - 0.75);
      painter->drawEllipse(center, borderRadius, borderRadius);
    } else {
      painter->setPen(Qt::NoPen);
      // 外圆形
      painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
      painter->drawEllipse(center, radius, radius);
      // 内圆形
      painter->setBrush(NXThemeColor(m_ThemeMode, BasicTextInvert));
      qreal const circleRadius = radius * m_CircleScale;
      painter->drawEllipse(center, circleRadius, circleRadius);
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

int NXRadioButtonStyle::pixelMetric(PixelMetric metric, QStyleOption const *option, QWidget const *widget) const
{
  switch (metric) {
  case QStyle::PM_ExclusiveIndicatorWidth :
  case QStyle::PM_ExclusiveIndicatorHeight : {
    return widget ? widget->fontMetrics().height() : QProxyStyle::pixelMetric(metric, option, widget);
  }
  default : {
    break;
  }
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}

void NXRadioButtonStyle::_updateCircleScale(QStyle::State state, QWidget const *widget)
{
  qreal targetScale = 2.0 / 3.8;
  if (state.testFlag(QStyle::State_Enabled) && state.testFlag(QStyle::State_On)) {
    if (state.testFlag(QStyle::State_Sunken)) {
      targetScale = 2.0 / 4.5;
    } else if (state.testFlag(QStyle::State_MouseOver)) {
      targetScale = 2.0 / 3.0;
    }
  }
  if (!widget || !widget->isVisible() || !state.testFlag(QStyle::State_Enabled) || !state.testFlag(QStyle::State_On)) {
    m_CircleScaleAnimation->stop();
    m_TargetCircleScale = targetScale;
    m_AnimationWidget.clear();
    if (!qFuzzyCompare(m_CircleScale, targetScale)) {
      setCircleScale(targetScale);
    }
    return;
  }
  m_AnimationWidget = const_cast<QWidget *>(widget);
  if (qFuzzyCompare(m_TargetCircleScale, targetScale)) {
    return;
  }
  m_TargetCircleScale = targetScale;
  // 复用同一个动画，并从当前比例续接，快速切换悬停或按下状态时不会跳变。
  m_CircleScaleAnimation->stop();
  m_CircleScaleAnimation->setStartValue(m_CircleScale);
  m_CircleScaleAnimation->setEndValue(targetScale);
  m_CircleScaleAnimation->start();
}
