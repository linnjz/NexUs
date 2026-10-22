#include "NXProgressRing.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QtMath>

#include "NXTheme.h"
#include "private/NXProgressRingPrivate.h"
Q_PROPERTY_CREATE_CPP(NXProgressRing, NXProgressRingType::ValueDisplayMode, ValueDisplayMode)
Q_PROPERTY_CREATE_CPP(NXProgressRing, int, ValuePixelSize)
Q_PROPERTY_CREATE_CPP(NXProgressRing, bool, IsTransparent)
Q_PROPERTY_CREATE_CPP(NXProgressRing, bool, IsDisplayValue)

NXProgressRing::NXProgressRing(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXProgressRingPrivate())
{
  Q_D(NXProgressRing);
  d->q_ptr                 = this;
  d->_pIsDisplayValue      = true;
  d->_pValuePixelSize      = 17;
  d->_pValueDisplayMode    = NXProgressRingType::ValueDisplayMode::Actual;
  d->_pMinimum             = 0;
  d->_pMaximum             = 100;
  d->_pValue               = 0;
  d->_pBusyIndex           = 0xe1'00;
  d->_pBusyingDurationTime = 1000;
  d->_pBusyingWidth        = 6;
  d->_pBusyStartDeg        = 0;
  d->_pBusyContentDeg      = 0;
  d->_pIsBusying           = false;
  d->_pIsTransparent       = false;
  setFixedSize(70, 70);
  setObjectName("NXProgressRing");
  setStyleSheet("#NXProgressRing{background-color:transparent;}");

  d->_busyStartDegAnimation = new QPropertyAnimation(d, "pBusyStartDeg");
  connect(d->_busyStartDegAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    update();
  });
  d->_busyStartDegAnimation->setEasingCurve(QEasingCurve::Linear);
  d->_busyStartDegAnimation->setDuration(1000);
  d->_busyStartDegAnimation->setStartValue(0);
  d->_busyStartDegAnimation->setEndValue(360);
  d->_busyStartDegAnimation->setLoopCount(-1);

  d->_busyContentDegAnimation = new QPropertyAnimation(d, "pBusyContentDeg");
  d->_busyContentDegAnimation->setEasingCurve(QEasingCurve::Linear);
  d->_busyContentDegAnimation->setDuration(1800);
  d->_busyContentDegAnimation->setStartValue(10);
  d->_busyContentDegAnimation->setKeyValueAt(0.5, 140);
  d->_busyContentDegAnimation->setEndValue(10);
  d->_busyContentDegAnimation->setLoopCount(-1);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXProgressRing::~NXProgressRing()
{
}

void NXProgressRing::setIsBusying(bool isBusying)
{
  Q_D(NXProgressRing);
  d->_pIsBusying = isBusying;
  if (isBusying) {
    d->_busyStartDegAnimation->start();
    d->_busyContentDegAnimation->start();
  } else {
    d->_busyStartDegAnimation->stop();
    d->_busyContentDegAnimation->stop();
  }
  update();
  Q_EMIT pIsBusyingChanged();
}

bool NXProgressRing::getIsBusying() const
{
  Q_D(const NXProgressRing);
  return d->_pIsBusying;
}

void NXProgressRing::setBusyingWidth(int width)
{
  Q_D(NXProgressRing);
  d->_pBusyingWidth = width;
  update();
  Q_EMIT pBusyingWidthChanged();
}

int NXProgressRing::getBusyingWidth() const
{
  Q_D(const NXProgressRing);
  return d->_pBusyingWidth;
}

void NXProgressRing::setBusyingDurationTime(int busyingDurationTime)
{
  Q_D(NXProgressRing);
  d->_pBusyingDurationTime = busyingDurationTime;
  d->_busyStartDegAnimation->setDuration(d->_pBusyingDurationTime);
  Q_EMIT pBusyingDurationTimeChanged();
}

int NXProgressRing::getBusyingDurationTime() const
{
  Q_D(const NXProgressRing);
  return d->_pBusyingDurationTime;
}

void NXProgressRing::setMinimum(int minimum)
{
  Q_D(NXProgressRing);
  d->_pMinimum = minimum;
  update();
  Q_EMIT pMinimumChanged();
  Q_EMIT rangeChanged(d->_pMinimum, d->_pMaximum);
}

int NXProgressRing::getMinimum() const
{
  Q_D(const NXProgressRing);
  return d->_pMinimum;
}

void NXProgressRing::setMaximum(int maximum)
{
  Q_D(NXProgressRing);
  d->_pMaximum = maximum;
  update();
  Q_EMIT pMaximumChanged();
  Q_EMIT rangeChanged(d->_pMinimum, d->_pMaximum);
}

int NXProgressRing::getMaximum() const
{
  Q_D(const NXProgressRing);
  return d->_pMaximum;
}

void NXProgressRing::setValue(int value)
{
  Q_D(NXProgressRing);
  if (value < d->_pMinimum || value > d->_pMaximum) {
    return;
  }
  d->_pValue = value;
  update();
  Q_EMIT pValueChanged();
}

int NXProgressRing::getValue() const
{
  Q_D(const NXProgressRing);
  return d->_pValue;
}

void NXProgressRing::setRange(int min, int max)
{
  Q_D(NXProgressRing);
  if (min < 0 || max < 0 || min > max) {
    return;
  }
  d->_pMinimum = min;
  d->_pMaximum = max;
  update();
  Q_EMIT pMinimumChanged();
  Q_EMIT pMaximumChanged();
  Q_EMIT rangeChanged(min, max);
}

void NXProgressRing::paintEvent(QPaintEvent *event)
{
  Q_D(NXProgressRing);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
  // 文字绘制
  if (d->_pIsDisplayValue && !d->_pIsBusying) {
    painter.setPen(NXThemeColor(d->_themeMode, BasicText));
    QString valueText;
    if (d->_pValueDisplayMode == NXProgressRingType::ValueDisplayMode::Actual) {
      valueText = QString::number(d->_pValue);
    } else {
      valueText = QString::number(d->_pValue / (qreal) (d->_pMaximum - d->_pMinimum) * 100) + "%";
    }
    painter.drawText(rect(), Qt::AlignCenter | Qt::TextSingleLine, valueText);
  }
  // Ring绘制
  QRect ringRect = rect().adjusted(1, 1, -1, -1);
  // 遮罩
  QPainterPath basePath;
  basePath.addEllipse(ringRect);
  QPainterPath maskPath;
  maskPath.addEllipse(ringRect.adjusted(d->_pBusyingWidth, d->_pBusyingWidth, -d->_pBusyingWidth, -d->_pBusyingWidth));
  painter.setClipPath(basePath.subtracted(maskPath));
  painter.setPen(Qt::NoPen);
  // 底色
  if (!d->_pIsTransparent) {
    painter.setBrush(NXThemeColor(d->_themeMode, BasicBaseDeep));
    painter.drawEllipse(ringRect);
  }
  // 环
  painter.setBrush(NXThemeColor(d->_themeMode, PrimaryNormal));
  if (d->_pIsBusying) {
    painter.drawPie(ringRect, -d->_pBusyStartDeg * 16, d->_pBusyContentDeg * 16);
  } else {
    qreal valueDeg = d->_pValue / (qreal) (d->_pMaximum - d->_pMinimum) * 360;
    painter.drawPie(ringRect, 90 * 16, -valueDeg * 16);
  }
  painter.restore();
}
