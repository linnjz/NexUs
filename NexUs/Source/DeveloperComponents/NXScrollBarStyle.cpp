#include "NXScrollBarStyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QStyleOptionSlider>
#include <QtMath>

#include "NXScrollBar.h"
#include "NXTheme.h"

NXScrollBarStyle::NXScrollBarStyle(QStyle *style)
    : m_Opacity { 0.0 }
    , m_SliderExtent { 2.4 }
    , m_ScrollBar { nullptr }
    , m_IsExpand { false }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXScrollBarStyle::~NXScrollBarStyle()
{
}

void NXScrollBarStyle::drawComplexControl(ComplexControl control, QStyleOptionComplex const *option, QPainter *painter, QWidget const *widget) const
{
  //QStyle::SC_ScrollBarGroove QStyle::SC_ScrollBarAddLine   QStyle::SC_ScrollBarSubLine上指示器
  switch (control) {
  case QStyle::CC_ScrollBar : {
    if (QStyleOptionSlider const *sopt = qstyleoption_cast<QStyleOptionSlider const *>(option)) {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setPen(Qt::NoPen);
      QRect scrollBarRect = sopt->rect;
      if (m_IsExpand) {
        // 背景绘制
        painter->setOpacity(m_Opacity);
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicBase));
        painter->drawRoundedRect(scrollBarRect, 6, 6);
        //指示器绘制 center()在此处不适用 存在外围边距宽度 需手动计算
        int sideLength = 8;
        painter->setBrush(NXThemeColor(m_ThemeMode, ScrollBarHandle));
        if (sopt->orientation == Qt::Horizontal) {
          QRect leftIndicatorRect  = subControlRect(control, sopt, QStyle::SC_ScrollBarSubLine, widget);
          QRect rightIndicatorRect = subControlRect(control, sopt, QStyle::SC_ScrollBarAddLine, widget);
          // 左三角
          qreal centerLeftX  = leftIndicatorRect.x() + leftIndicatorRect.width() / 2;
          qreal centerRightX = rightIndicatorRect.x() + rightIndicatorRect.width() / 2;
          qreal centerY      = leftIndicatorRect.height() / 2;
          QPainterPath leftPath;
          leftPath.moveTo(centerLeftX - qCos(30 * M_PI / 180.0) * sideLength / 2, centerY);
          leftPath.lineTo(centerLeftX + qCos(30 * M_PI / 180.0) * sideLength / 2, centerY + sideLength / 2);
          leftPath.lineTo(centerLeftX + qCos(30 * M_PI / 180.0) * sideLength / 2, centerY - sideLength / 2);
          leftPath.closeSubpath();
          painter->drawPath(leftPath);

          // 右三角
          QPainterPath rightPath;
          rightPath.moveTo(centerRightX + qCos(30 * M_PI / 180.0) * sideLength / 2, centerY);
          rightPath.lineTo(centerRightX - qCos(30 * M_PI / 180.0) * sideLength / 2, centerY + sideLength / 2);
          rightPath.lineTo(centerRightX - qCos(30 * M_PI / 180.0) * sideLength / 2, centerY - sideLength / 2);
          rightPath.closeSubpath();
          painter->drawPath(rightPath);
        } else {
          QRect upIndicatorRect   = subControlRect(control, sopt, QStyle::SC_ScrollBarSubLine, widget);
          QRect downIndicatorRect = subControlRect(control, sopt, QStyle::SC_ScrollBarAddLine, widget);
          qreal centerToTop       = (sideLength / 2) / qCos(30 * M_PI / 180.0);
          qreal centerToBottom    = (sideLength / 2) * qTan(30 * M_PI / 180.0);
          // 上三角
          qreal centerX     = upIndicatorRect.width() / 2.0;
          qreal centerUpY   = upIndicatorRect.center().y() + 2;
          qreal centerDownY = downIndicatorRect.center().y() + 2;
          QPainterPath upPath;
          upPath.moveTo(centerX, centerUpY - centerToTop);
          upPath.lineTo(centerX + sideLength / 2, centerUpY + centerToBottom);
          upPath.lineTo(centerX - sideLength / 2, centerUpY + centerToBottom);
          upPath.closeSubpath();
          painter->drawPath(upPath);

          // 下三角
          QPainterPath downPath;
          downPath.moveTo(centerX, centerDownY + centerToBottom);
          downPath.lineTo(centerX + sideLength / 2, centerDownY - centerToTop);
          downPath.lineTo(centerX - sideLength / 2, centerDownY - centerToTop);
          downPath.closeSubpath();
          painter->drawPath(downPath);
        }
      }
      painter->setOpacity(1);
      //滑块绘制
      QRectF sliderRect = subControlRect(control, sopt, QStyle::SC_ScrollBarSlider, widget);
      painter->setBrush(NXThemeColor(m_ThemeMode, ScrollBarHandle));
      if (sopt->orientation == Qt::Horizontal) {
        sliderRect.setRect(sliderRect.x(), sliderRect.bottom() - m_SliderMargin - m_SliderExtent, sliderRect.width(), m_SliderExtent);
      } else {
        sliderRect.setRect(sliderRect.right() - m_SliderMargin - m_SliderExtent, sliderRect.y(), m_SliderExtent, sliderRect.height());
      }
      painter->drawRoundedRect(sliderRect, m_SliderExtent / 2.0, m_SliderExtent / 2.0);
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

int NXScrollBarStyle::pixelMetric(PixelMetric metric, QStyleOption const *option, QWidget const *widget) const
{
  // qDebug() << metric << QProxyStyle::pixelMetric(metric, option, widget);
  switch (metric) {
  case QStyle::PM_ScrollBarExtent : {
    return m_ScrollBarExtent;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}

int NXScrollBarStyle::styleHint(StyleHint hint, QStyleOption const *option, QWidget const *widget, QStyleHintReturn *returnData) const
{
  if (hint == QStyle::SH_ScrollBar_LeftClickAbsolutePosition) {
    return true;
  }
  return QProxyStyle::styleHint(hint, option, widget, returnData);
}

void NXScrollBarStyle::startExpandAnimation(bool isExpand)
{
  if (isExpand) {
    m_IsExpand                           = true;
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(this, "Opacity");
    connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      m_ScrollBar->update();
    });
    opacityAnimation->setDuration(250);
    opacityAnimation->setEasingCurve(QEasingCurve::InOutSine);
    opacityAnimation->setStartValue(m_Opacity);
    opacityAnimation->setEndValue(1);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *extentAnimation = new QPropertyAnimation(this, "SliderExtent");
    extentAnimation->setDuration(250);
    extentAnimation->setEasingCurve(QEasingCurve::InOutSine);
    extentAnimation->setStartValue(m_SliderExtent);
    extentAnimation->setEndValue(m_ScrollBarExtent - 2 * m_SliderMargin);
    extentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  } else {
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(this, "Opacity");
    connect(opacityAnimation, &QPropertyAnimation::finished, this, [=]() {
      m_IsExpand = false;
    });
    connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      m_ScrollBar->update();
    });
    opacityAnimation->setDuration(250);
    opacityAnimation->setEasingCurve(QEasingCurve::InOutSine);
    opacityAnimation->setStartValue(m_Opacity);
    opacityAnimation->setEndValue(0);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *extentAnimation = new QPropertyAnimation(this, "SliderExtent");
    extentAnimation->setDuration(250);
    extentAnimation->setEasingCurve(QEasingCurve::InOutSine);
    extentAnimation->setStartValue(m_SliderExtent);
    extentAnimation->setEndValue(2.4);
    extentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }
}
