#include "NXDashboardGauge.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QtMath>

#include "NXTheme.h"
#include "private/NXDashboardGaugePrivate.h"

Q_PROPERTY_CREATE_CPP(NXDashboardGauge, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, QPS_SCREF_PAEMIT(QString), Unit)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, QPS_PAEMIT(int), MajorTickCount)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, QPS_PAEMIT(int), MinorTickCount)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, QPS_PAEMIT(int), StartAngle)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, QPS_PAEMIT(int), SpanAngle)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, QPS_PAEMIT(int), ArcWidth)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, QPS_PAEMIT(int), ValuePixelSize)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, QPS_PAEMIT(int), Decimals)
Q_PROPERTY_CREATE_CPP(NXDashboardGauge, QPS_PAEMIT(bool), IsAnimated)

void NXDashboardGauge::setMinimum(qreal minimum)
{
  Q_D(NXDashboardGauge);
  d->m_Minimum = minimum;
  update();
  Q_EMIT signalMinimumChanged();
}

qreal NXDashboardGauge::getMinimum() const noexcept(std::is_nothrow_copy_constructible_v<qreal>)
{
  return d_ptr->m_Minimum;
}

void NXDashboardGauge::setMaximum(qreal maximum)
{
  Q_D(NXDashboardGauge);
  d->m_Maximum = maximum;
  update();
  Q_EMIT signalMaximumChanged();
}

qreal NXDashboardGauge::getMaximum() const noexcept(std::is_nothrow_copy_constructible_v<qreal>)
{
  return d_ptr->m_Maximum;
}

void NXDashboardGauge::setValue(qreal value)
{
  Q_D(NXDashboardGauge);
  value = qBound(d->m_Minimum, value, d->m_Maximum);
  if (qFuzzyCompare(d->m_Value, value)) {
    return;
  }
  d->m_Value = value;

  if (d->m_IsAnimated) {
    QPropertyAnimation *animation = new QPropertyAnimation(d, "AnimatedValue");
    animation->setDuration(600);
    animation->setStartValue(d->m_AnimatedValue);
    animation->setEndValue(value);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    connect(animation, &QPropertyAnimation::valueChanged, this, [this]() {
      update();
    });
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  } else {
    d->m_AnimatedValue = value;
    update();
  }
  Q_EMIT valueChanged(value);
  Q_EMIT signalValueChanged();
}

qreal NXDashboardGauge::getValue() const noexcept(std::is_nothrow_copy_constructible_v<qreal>)
{
  return d_ptr->m_Value;
}

NXDashboardGauge::NXDashboardGauge(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXDashboardGaugePrivate())
{
  Q_D(NXDashboardGauge);
  d->q_ptr            = this;
  d->m_Minimum        = 0;
  d->m_Maximum        = 100;
  d->m_Value          = 0;
  d->m_AnimatedValue   = 0;
  d->m_MajorTickCount = 10;
  d->m_MinorTickCount = 5;
  d->m_StartAngle     = 225;
  d->m_SpanAngle      = 270;
  d->m_ArcWidth       = 12;
  d->m_ValuePixelSize = 22;
  d->m_IsAnimated     = true;
  d->m_Decimals       = 0;
  setObjectName("NXDashboardGauge");
  setFixedSize(260, 260);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXDashboardGauge::~NXDashboardGauge()
{
}

void NXDashboardGauge::setDangerPercent(qreal percent)
{
  Q_D(NXDashboardGauge);
  d->m_DangerPercent = qBound(0.0, percent, 1.0);
  update();
}

qreal NXDashboardGauge::getDangerPercent() const
{
  return d_ptr->m_DangerPercent;
}

void NXDashboardGauge::setWarningPercent(qreal percent)
{
  Q_D(NXDashboardGauge);
  d->m_WarningPercent = qBound(0.0, percent, 1.0);
  update();
}

qreal NXDashboardGauge::getWarningPercent() const
{
  return d_ptr->m_WarningPercent;
}

void NXDashboardGauge::setTickWarningPercent(qreal percent)
{
  Q_D(NXDashboardGauge);
  d->m_TickWarningPercent = qBound(0.0, percent, 1.0);
  update();
}

qreal NXDashboardGauge::getTickWarningPercent() const
{
  return d_ptr->m_TickWarningPercent;
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
  qreal arcWidth  = d->m_ArcWidth;
  qreal arcRadius = radius - arcWidth / 2.0;

  QPen trackPen;
  trackPen.setWidthF(arcWidth);
  trackPen.setCapStyle(Qt::RoundCap);
  trackPen.setColor(NXThemeColor(d->m_ThemeMode, BasicChute));
  painter.setPen(trackPen);
  painter.setBrush(Qt::NoBrush);
  QRectF arcRect(-arcRadius, -arcRadius, arcRadius * 2, arcRadius * 2);
  painter.drawArc(arcRect, d->m_StartAngle * 16, -d->m_SpanAngle * 16);

  qreal range        = d->m_Maximum - d->m_Minimum;
  qreal displayValue = d->m_IsAnimated ? d->m_AnimatedValue : d->m_Value;
  qreal percent      = range > 0 ? (displayValue - d->m_Minimum) / range : 0;
  int valueSpan      = static_cast<int>(d->m_SpanAngle * percent);

  QColor arcColor;
  if (percent >= d->m_DangerPercent) {
    arcColor = NXThemeColor(d->m_ThemeMode, StatusDanger);
  } else if (percent >= d->m_WarningPercent) {
    arcColor = QColor(0xF7, 0x94, 0x0B);
  } else {
    arcColor = NXThemeColor(d->m_ThemeMode, PrimaryNormal);
  }

  QPen valuePen;
  valuePen.setWidthF(arcWidth);
  valuePen.setCapStyle(Qt::RoundCap);
  valuePen.setColor(arcColor);
  painter.setPen(valuePen);
  painter.drawArc(arcRect, d->m_StartAngle * 16, -valueSpan * 16);

  qreal tickOuterRadius = radius - arcWidth - 4;
  qreal majorTickLength = 10;
  qreal minorTickLength = 5;
  int totalTicks        = d->m_MajorTickCount * d->m_MinorTickCount;

  for (int i = 0; i <= totalTicks; i++) {
    qreal tickAngle = d->m_StartAngle - (d->m_SpanAngle * i / static_cast<qreal>(totalTicks));
    qreal rad       = qDegreesToRadians(tickAngle);
    bool isMajor    = (i % d->m_MinorTickCount == 0);

    qreal len = isMajor ? majorTickLength : minorTickLength;
    qreal x1  = tickOuterRadius * qCos(rad);
    qreal y1  = -tickOuterRadius * qSin(rad);
    qreal x2  = (tickOuterRadius - len) * qCos(rad);
    qreal y2  = -(tickOuterRadius - len) * qSin(rad);

    qreal tickPercent = i / static_cast<qreal>(totalTicks);
    QColor tickColor;
    if (tickPercent >= d->m_DangerPercent) {
      tickColor = NXThemeColor(d->m_ThemeMode, StatusDanger);
    } else if (tickPercent >= d->m_TickWarningPercent) {
      tickColor = QColor(0xF7, 0x94, 0x0B);
    } else {
      tickColor = NXThemeColor(d->m_ThemeMode, BasicText);
    }

    QPen tickPen;
    tickPen.setWidthF(isMajor ? 2.0 : 1.0);
    tickPen.setColor(tickColor);
    painter.setPen(tickPen);
    painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));

    if (isMajor) {
      qreal tickValue   = d->m_Minimum + range * tickPercent;
      QString tickLabel = QString::number(tickValue, 'f', d->m_Decimals);

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

  qreal needleAngle      = d->m_StartAngle - d->m_SpanAngle * percent;
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

  painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicText));
  painter.drawEllipse(QPointF(0, 0), 6, 6);
  painter.setBrush(NXThemeColor(d->m_ThemeMode, WindowBase));
  painter.drawEllipse(QPointF(0, 0), 3, 3);
  painter.restore();

  QFont valueFont = font();
  valueFont.setPixelSize(d->m_ValuePixelSize);
  valueFont.setBold(true);
  painter.setFont(valueFont);
  painter.setPen(arcColor);

  QString valueText = QString::number(displayValue, 'f', d->m_Decimals);
  if (!d->m_Unit.isEmpty()) {
    valueText += QStringLiteral(" ") + d->m_Unit;
  }
  painter.drawText(QRectF(-80, 15, 160, 40), Qt::AlignCenter, valueText);

  if (!d->m_Title.isEmpty()) {
    QFont titleFont = font();
    titleFont.setPixelSize(13);
    painter.setFont(titleFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
    painter.drawText(QRectF(-80, 50, 160, 20), Qt::AlignCenter, d->m_Title);
  }
}

NXDashboardGaugePrivate::NXDashboardGaugePrivate(QObject *parent)
    : QObject(parent)
{
}

NXDashboardGaugePrivate::~NXDashboardGaugePrivate()
{
}
