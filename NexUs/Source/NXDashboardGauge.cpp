#include "NXDashboardGauge.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QtMath>

#include "NXTheme.h"
#include "private/NXDashboardGaugePrivate.h"

Q_PROPERTY_CREATE_CPP(NXDashboardGauge, QS_SET_CREF(QString), Title)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, QS_SET_CREF(QString), Unit)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, int, MajorTickCount)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, int, MinorTickCount)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, int, StartAngle)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, int, SpanAngle)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, int, ArcWidth)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, int, ValuePixelSize)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, int, Decimals)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, bool, IsAnimated)

void NXDashboardGauge::setMinimum(qreal minimum)
{
  Q_D(NXDashboardGauge);
  d->_pMinimum = minimum;
  update();
  Q_EMIT pMinimumChanged();
}

qreal NXDashboardGauge::getMinimum() const
{
  return d_ptr->_pMinimum;
}

void NXDashboardGauge::setMaximum(qreal maximum)
{
  Q_D(NXDashboardGauge);
  d->_pMaximum = maximum;
  update();
  Q_EMIT pMaximumChanged();
}

qreal NXDashboardGauge::getMaximum() const
{
  return d_ptr->_pMaximum;
}

void NXDashboardGauge::setValue(qreal value)
{
  Q_D(NXDashboardGauge);
  value = qBound(d->_pMinimum, value, d->_pMaximum);
  if (qFuzzyCompare(d->_pValue, value)) {
    return;
  }
  d->_pValue = value;

  if (d->_pIsAnimated) {
    QPropertyAnimation *animation = new QPropertyAnimation(d, "pAnimatedValue");
    animation->setDuration(600);
    animation->setStartValue(d->_animatedValue);
    animation->setEndValue(value);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    connect(animation, &QPropertyAnimation::valueChanged, this, [this]() {
      update();
    });
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  } else {
    d->_animatedValue = value;
    update();
  }
  Q_EMIT valueChanged(value);
  Q_EMIT pValueChanged();
}

qreal NXDashboardGauge::getValue() const
{
  return d_ptr->_pValue;
}

NXDashboardGauge::NXDashboardGauge(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXDashboardGaugePrivate())
{
  Q_D(NXDashboardGauge);
  d->q_ptr            = this;
  d->_pMinimum        = 0;
  d->_pMaximum        = 100;
  d->_pValue          = 0;
  d->_animatedValue   = 0;
  d->_pMajorTickCount = 10;
  d->_pMinorTickCount = 5;
  d->_pStartAngle     = 225;
  d->_pSpanAngle      = 270;
  d->_pArcWidth       = 12;
  d->_pValuePixelSize = 22;
  d->_pIsAnimated     = true;
  d->_pDecimals       = 0;
  setObjectName("NXDashboardGauge");
  setFixedSize(260, 260);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXDashboardGauge::~NXDashboardGauge()
{
}

void NXDashboardGauge::setDangerPercent(qreal percent)
{
  Q_D(NXDashboardGauge);
  d->_dangerPercent = qBound(0.0, percent, 1.0);
  update();
}

qreal NXDashboardGauge::getDangerPercent() const
{
  return d_ptr->_dangerPercent;
}

void NXDashboardGauge::setWarningPercent(qreal percent)
{
  Q_D(NXDashboardGauge);
  d->_warningPercent = qBound(0.0, percent, 1.0);
  update();
}

qreal NXDashboardGauge::getWarningPercent() const
{
  return d_ptr->_warningPercent;
}

void NXDashboardGauge::setTickWarningPercent(qreal percent)
{
  Q_D(NXDashboardGauge);
  d->_tickWarningPercent = qBound(0.0, percent, 1.0);
  update();
}

qreal NXDashboardGauge::getTickWarningPercent() const
{
  return d_ptr->_tickWarningPercent;
}

void NXDashboardGauge::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  Q_D(NXDashboardGauge);

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  int side = qMin(width(), height());
  painter.translate(width() / 2.0, height() / 2.0);
  painter.scale(side / 260.0, side / 260.0);

  qreal radius    = 110.0;
  qreal arcWidth  = d->_pArcWidth;
  qreal arcRadius = radius - arcWidth / 2.0;

  QPen trackPen;
  trackPen.setWidthF(arcWidth);
  trackPen.setCapStyle(Qt::RoundCap);
  trackPen.setColor(NXThemeColor(d->_themeMode, BasicChute));
  painter.setPen(trackPen);
  painter.setBrush(Qt::NoBrush);
  QRectF arcRect(-arcRadius, -arcRadius, arcRadius * 2, arcRadius * 2);
  painter.drawArc(arcRect, d->_pStartAngle * 16, -d->_pSpanAngle * 16);

  qreal range        = d->_pMaximum - d->_pMinimum;
  qreal displayValue = d->_pIsAnimated ? d->_animatedValue : d->_pValue;
  qreal percent      = range > 0 ? (displayValue - d->_pMinimum) / range : 0;
  int valueSpan      = static_cast<int>(d->_pSpanAngle * percent);

  QColor arcColor;
  if (percent >= d->_dangerPercent) {
    arcColor = NXThemeColor(d->_themeMode, StatusDanger);
  } else if (percent >= d->_warningPercent) {
    arcColor = QColor(0xF7, 0x94, 0x0B);
  } else {
    arcColor = NXThemeColor(d->_themeMode, PrimaryNormal);
  }

  QPen valuePen;
  valuePen.setWidthF(arcWidth);
  valuePen.setCapStyle(Qt::RoundCap);
  valuePen.setColor(arcColor);
  painter.setPen(valuePen);
  painter.drawArc(arcRect, d->_pStartAngle * 16, -valueSpan * 16);

  qreal tickOuterRadius = radius - arcWidth - 4;
  qreal majorTickLength = 10;
  qreal minorTickLength = 5;
  int totalTicks        = d->_pMajorTickCount * d->_pMinorTickCount;

  for (int i = 0; i <= totalTicks; i++) {
    qreal tickAngle = d->_pStartAngle - (d->_pSpanAngle * i / static_cast<qreal>(totalTicks));
    qreal rad       = qDegreesToRadians(tickAngle);
    bool isMajor    = (i % d->_pMinorTickCount == 0);

    qreal len = isMajor ? majorTickLength : minorTickLength;
    qreal x1  = tickOuterRadius * qCos(rad);
    qreal y1  = -tickOuterRadius * qSin(rad);
    qreal x2  = (tickOuterRadius - len) * qCos(rad);
    qreal y2  = -(tickOuterRadius - len) * qSin(rad);

    qreal tickPercent = i / static_cast<qreal>(totalTicks);
    QColor tickColor;
    if (tickPercent >= d->_dangerPercent) {
      tickColor = NXThemeColor(d->_themeMode, StatusDanger);
    } else if (tickPercent >= d->_tickWarningPercent) {
      tickColor = QColor(0xF7, 0x94, 0x0B);
    } else {
      tickColor = NXThemeColor(d->_themeMode, BasicText);
    }

    QPen tickPen;
    tickPen.setWidthF(isMajor ? 2.0 : 1.0);
    tickPen.setColor(tickColor);
    painter.setPen(tickPen);
    painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));

    if (isMajor) {
      qreal tickValue   = d->_pMinimum + range * tickPercent;
      QString tickLabel = QString::number(tickValue, 'f', d->_pDecimals);

      QFont tickFont = font();
      tickFont.setPixelSize(10);
      painter.setFont(tickFont);
      painter.setPen(tickColor);

      qreal labelRadius = tickOuterRadius - len - 12;
      qreal lx          = labelRadius * qCos(rad);
      qreal ly          = -labelRadius * qSin(rad);
      QRectF labelRect(lx - 20, ly - 8, 40, 16);
      painter.drawText(labelRect, Qt::AlignCenter, tickLabel);
    }
  }

  qreal needleAngle      = d->_pStartAngle - d->_pSpanAngle * percent;
  qreal needleRad        = qDegreesToRadians(needleAngle);
  qreal needleLength     = tickOuterRadius - majorTickLength - 20;
  qreal needleTailLength = 12;

  painter.save();
  painter.rotate(-(needleAngle - 90));

  QPainterPath needlePath;
  needlePath.moveTo(0, -needleLength);
  needlePath.lineTo(-3, 0);
  needlePath.lineTo(0, needleTailLength);
  needlePath.lineTo(3, 0);
  needlePath.closeSubpath();

  painter.setPen(Qt::NoPen);
  painter.setBrush(arcColor);
  painter.drawPath(needlePath);

  painter.setBrush(NXThemeColor(d->_themeMode, BasicText));
  painter.drawEllipse(QPointF(0, 0), 6, 6);
  painter.setBrush(NXThemeColor(d->_themeMode, WindowBase));
  painter.drawEllipse(QPointF(0, 0), 3, 3);
  painter.restore();

  QFont valueFont = font();
  valueFont.setPixelSize(d->_pValuePixelSize);
  valueFont.setBold(true);
  painter.setFont(valueFont);
  painter.setPen(arcColor);

  QString valueText = QString::number(displayValue, 'f', d->_pDecimals);
  if (!d->_pUnit.isEmpty()) {
    valueText += QStringLiteral(" ") + d->_pUnit;
  }
  painter.drawText(QRectF(-80, 15, 160, 40), Qt::AlignCenter, valueText);

  if (!d->_pTitle.isEmpty()) {
    QFont titleFont = font();
    titleFont.setPixelSize(13);
    painter.setFont(titleFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicDetailsText));
    painter.drawText(QRectF(-80, 50, 160, 20), Qt::AlignCenter, d->_pTitle);
  }
}

NXDashboardGaugePrivate::NXDashboardGaugePrivate(QObject *parent)
    : QObject(parent)
{
}

NXDashboardGaugePrivate::~NXDashboardGaugePrivate()
{
}
