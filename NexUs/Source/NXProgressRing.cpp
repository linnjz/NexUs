#include "NXProgressRing.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QtMath>

#include "NXTheme.h"
#include "private/NXProgressRingPrivate.h"
Q_PROPERTY_CREATE_CPP(NXProgressRing, QPS_PAEMIT(NXProgressRingType::ValueDisplayMode), ValueDisplayMode)
Q_PROPERTY_CREATE_CPP(NXProgressRing, QPS_PAEMIT(int), ValuePixelSize)
Q_PROPERTY_CREATE_CPP(NXProgressRing, QPS_PAEMIT(bool), IsTransparent)
Q_PROPERTY_CREATE_CPP(NXProgressRing, QPS_PAEMIT(bool), IsDisplayValue)

NXProgressRing::NXProgressRing(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXProgressRingPrivate())
{
  Q_D(NXProgressRing);
  d->q_ptr                 = this;
  d->m_IsDisplayValue      = true;
  d->m_ValuePixelSize      = 17;
  d->m_ValueDisplayMode    = NXProgressRingType::ValueDisplayMode::Actual;
  d->m_Minimum             = 0;
  d->m_Maximum             = 100;
  d->m_Value               = 0;
  d->m_BusyIndex           = 0xe1'00;
  d->m_BusyingDurationTime = 1000;
  d->m_BusyingWidth        = 6;
  d->m_BusyStartDeg        = 0;
  d->m_BusyContentDeg      = 0;
  d->m_IsBusying           = false;
  d->m_IsTransparent       = false;
  setFixedSize(70, 70);
  setObjectName("NXProgressRing");
  setStyleSheet("#NXProgressRing{background-color:transparent;}");

  d->m_BusyStartDegAnimation = new QPropertyAnimation(d, "BusyStartDeg");
  connect(d->m_BusyStartDegAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    update();
  });
  d->m_BusyStartDegAnimation->setEasingCurve(QEasingCurve::Linear);
  d->m_BusyStartDegAnimation->setDuration(1000);
  d->m_BusyStartDegAnimation->setStartValue(0);
  d->m_BusyStartDegAnimation->setEndValue(360);
  d->m_BusyStartDegAnimation->setLoopCount(-1);

  d->m_BusyContentDegAnimation = new QPropertyAnimation(d, "BusyContentDeg");
  d->m_BusyContentDegAnimation->setEasingCurve(QEasingCurve::Linear);
  d->m_BusyContentDegAnimation->setDuration(1800);
  d->m_BusyContentDegAnimation->setStartValue(10);
  d->m_BusyContentDegAnimation->setKeyValueAt(0.5, 140);
  d->m_BusyContentDegAnimation->setEndValue(10);
  d->m_BusyContentDegAnimation->setLoopCount(-1);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXProgressRing::~NXProgressRing()
{
}

void NXProgressRing::setIsBusying(bool isBusying)
{
  Q_D(NXProgressRing);
  d->m_IsBusying = isBusying;
  if (isBusying) {
    d->m_BusyStartDegAnimation->start();
    d->m_BusyContentDegAnimation->start();
  } else {
    d->m_BusyStartDegAnimation->stop();
    d->m_BusyContentDegAnimation->stop();
  }
  update();
  Q_EMIT signalIsBusyingChanged();
}

bool NXProgressRing::getIsBusying() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXProgressRing);
  return d->m_IsBusying;
}

void NXProgressRing::setBusyingWidth(int width)
{
  Q_D(NXProgressRing);
  d->m_BusyingWidth = width;
  update();
  Q_EMIT signalBusyingWidthChanged();
}

int NXProgressRing::getBusyingWidth() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXProgressRing);
  return d->m_BusyingWidth;
}

void NXProgressRing::setBusyingDurationTime(int busyingDurationTime)
{
  Q_D(NXProgressRing);
  d->m_BusyingDurationTime = busyingDurationTime;
  d->m_BusyStartDegAnimation->setDuration(d->m_BusyingDurationTime);
  Q_EMIT signalBusyingDurationTimeChanged();
}

int NXProgressRing::getBusyingDurationTime() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXProgressRing);
  return d->m_BusyingDurationTime;
}

void NXProgressRing::setMinimum(int minimum)
{
  Q_D(NXProgressRing);
  d->m_Minimum = minimum;
  update();
  Q_EMIT signalMinimumChanged();
  Q_EMIT rangeChanged(d->m_Minimum, d->m_Maximum);
}

int NXProgressRing::getMinimum() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXProgressRing);
  return d->m_Minimum;
}

void NXProgressRing::setMaximum(int maximum)
{
  Q_D(NXProgressRing);
  d->m_Maximum = maximum;
  update();
  Q_EMIT signalMaximumChanged();
  Q_EMIT rangeChanged(d->m_Minimum, d->m_Maximum);
}

int NXProgressRing::getMaximum() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXProgressRing);
  return d->m_Maximum;
}

void NXProgressRing::setValue(int value)
{
  Q_D(NXProgressRing);
  if (value < d->m_Minimum || value > d->m_Maximum) {
    return;
  }
  d->m_Value = value;
  update();
  Q_EMIT signalValueChanged();
}

int NXProgressRing::getValue() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXProgressRing);
  return d->m_Value;
}

void NXProgressRing::setRange(int min, int max)
{
  Q_D(NXProgressRing);
  if (min < 0 || max < 0 || min > max) {
    return;
  }
  d->m_Minimum = min;
  d->m_Maximum = max;
  update();
  Q_EMIT signalMinimumChanged();
  Q_EMIT signalMaximumChanged();
  Q_EMIT rangeChanged(min, max);
}

void NXProgressRing::paintEvent(QPaintEvent *event)
{
  Q_D(NXProgressRing);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
  // 文字绘制
  if (d->m_IsDisplayValue && !d->m_IsBusying) {
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    QString valueText;
    if (d->m_ValueDisplayMode == NXProgressRingType::ValueDisplayMode::Actual) {
      valueText = QString::number(d->m_Value);
    } else {
      valueText = QString::number(d->m_Value / (qreal) (d->m_Maximum - d->m_Minimum) * 100) + "%";
    }
    painter.drawText(rect(), Qt::AlignCenter | Qt::TextSingleLine, valueText);
  }
  // Ring绘制
  QRect ringRect = rect().adjusted(1, 1, -1, -1);
  // 遮罩
  QPainterPath basePath;
  basePath.addEllipse(ringRect);
  QPainterPath maskPath;
  maskPath.addEllipse(ringRect.adjusted(d->m_BusyingWidth, d->m_BusyingWidth, -d->m_BusyingWidth, -d->m_BusyingWidth));
  painter.setClipPath(basePath.subtracted(maskPath));
  painter.setPen(Qt::NoPen);
  // 底色
  if (!d->m_IsTransparent) {
    painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBaseDeep));
    painter.drawEllipse(ringRect);
  }
  // 环
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
  if (d->m_IsBusying) {
    painter.drawPie(ringRect, -d->m_BusyStartDeg * 16, d->m_BusyContentDeg * 16);
  } else {
    qreal valueDeg = d->m_Value / (qreal) (d->m_Maximum - d->m_Minimum) * 360;
    painter.drawPie(ringRect, 90 * 16, -valueDeg * 16);
  }
  painter.restore();
}
