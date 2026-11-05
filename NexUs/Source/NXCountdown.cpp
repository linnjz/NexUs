#include "NXCountdown.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QTimer>

#include "NXTheme.h"
#include "private/NXCountdownPrivate.h"
Q_PROPERTY_CREATE_CPP(NXCountdown, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXCountdown, QPS_PAEMIT(int), DigitWidth)
Q_PROPERTY_CREATE_CPP(NXCountdown, QPS_PAEMIT(int), DigitHeight)
Q_PROPERTY_CREATE_CPP(NXCountdown, QPS_PAEMIT(int), DigitSpacing)
Q_PROPERTY_CREATE_CPP(NXCountdown, QPS_PAEMIT(int), FontPixelSize)
Q_PROPERTY_CREATE_CPP(NXCountdown, QPS_PAEMIT(bool), IsShowDays)
Q_PROPERTY_CREATE_CPP(NXCountdown, QPS_PAEMIT(bool), IsShowHours)
Q_PROPERTY_CREATE_CPP(NXCountdown, QPS_PAEMIT(bool), IsShowMinutes)
Q_PROPERTY_CREATE_CPP(NXCountdown, QPS_PAEMIT(bool), IsShowSeconds)

NXCountdown::NXCountdown(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXCountdownPrivate())
{
  Q_D(NXCountdown);
  d->q_ptr           = this;
  d->m_BorderRadius  = 6;
  d->m_IsShowDays    = true;
  d->m_IsShowHours   = true;
  d->m_IsShowMinutes = true;
  d->m_IsShowSeconds = true;
  d->m_DigitWidth    = 44;
  d->m_DigitHeight   = 56;
  d->m_DigitSpacing  = 6;
  d->m_FontPixelSize = 24;
  d->m_FlipAngle     = 0;
  d->m_ThemeMode      = nxTheme->getThemeMode();

  d->m_Timer = new QTimer(this);
  d->m_Timer->setInterval(1000);
  connect(d->m_Timer, &QTimer::timeout, d, &NXCountdownPrivate::onTimerTick);

  _updateSize();

  connect(this, &NXCountdown::signalIsShowDaysChanged, this, &NXCountdown::_updateSize);
  connect(this, &NXCountdown::signalIsShowHoursChanged, this, &NXCountdown::_updateSize);
  connect(this, &NXCountdown::signalIsShowMinutesChanged, this, &NXCountdown::_updateSize);
  connect(this, &NXCountdown::signalIsShowSecondsChanged, this, &NXCountdown::_updateSize);
  connect(this, &NXCountdown::signalDigitWidthChanged, this, &NXCountdown::_updateSize);
  connect(this, &NXCountdown::signalDigitHeightChanged, this, &NXCountdown::_updateSize);
  connect(this, &NXCountdown::signalDigitSpacingChanged, this, &NXCountdown::_updateSize);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXCountdown::~NXCountdown()
{
}

void NXCountdown::setTargetDateTime(QDateTime const &dateTime)
{
  Q_D(NXCountdown);
  d->m_TargetDateTime   = dateTime;
  d->m_UseTargetMode    = true;
  qint64 secs          = QDateTime::currentDateTime().secsTo(dateTime);
  d->m_RemainingSeconds = qMax(static_cast<qint64>(0), secs);
  update();
}

QDateTime NXCountdown::getTargetDateTime() const
{
  return d_ptr->m_TargetDateTime;
}

void NXCountdown::setRemainingSeconds(qint64 seconds)
{
  Q_D(NXCountdown);
  d->m_RemainingSeconds = qMax(static_cast<qint64>(0), seconds);
  d->m_UseTargetMode    = false;
  update();
}

qint64 NXCountdown::getRemainingSeconds() const
{
  return d_ptr->m_RemainingSeconds;
}

void NXCountdown::start()
{
  Q_D(NXCountdown);
  if (d->m_IsRunning) {
    return;
  }
  d->m_IsRunning = true;
  if (d->m_UseTargetMode) {
    qint64 secs          = QDateTime::currentDateTime().secsTo(d->m_TargetDateTime);
    d->m_RemainingSeconds = qMax(static_cast<qint64>(0), secs);
  }
  d->m_PrevSeconds = -1;
  d->m_PrevMinutes = -1;
  d->m_PrevHours   = -1;
  d->m_PrevDays    = -1;
  d->m_Timer->start();
  update();
}

void NXCountdown::pause()
{
  Q_D(NXCountdown);
  d->m_IsRunning = false;
  d->m_Timer->stop();
}

void NXCountdown::resume()
{
  Q_D(NXCountdown);
  if (!d->m_IsRunning) {
    d->m_IsRunning = true;
    d->m_Timer->start();
  }
}

void NXCountdown::stop()
{
  Q_D(NXCountdown);
  d->m_IsRunning = false;
  d->m_Timer->stop();
  d->m_RemainingSeconds = 0;
  update();
}

bool NXCountdown::isRunning() const
{
  return d_ptr->m_IsRunning;
}

void NXCountdown::paintEvent(QPaintEvent *event)
{
  Q_D(NXCountdown);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  qint64 total = d->m_RemainingSeconds;
  int days     = static_cast<int>(total / 86400);
  int hours    = static_cast<int>((total % 86400) / 3600);
  int minutes  = static_cast<int>((total % 3600) / 60);
  int seconds  = static_cast<int>(total % 60);

  QFont digitFont = font();
  digitFont.setPixelSize(d->m_FontPixelSize);
  digitFont.setBold(true);

  QFont separatorFont = font();
  separatorFont.setPixelSize(d->m_FontPixelSize);
  separatorFont.setBold(true);

  QColor bgColor        = NXThemeColor(d->m_ThemeMode, BasicBaseDeep);
  QColor textColor      = NXThemeColor(d->m_ThemeMode, BasicText);
  QColor separatorColor = NXThemeColor(d->m_ThemeMode, BasicTextNoFocus);
  QColor lineColor      = NXThemeColor(d->m_ThemeMode, BasicBorder);

  int dw      = d->m_DigitWidth;
  int dh      = d->m_DigitHeight;
  int spacing = d->m_DigitSpacing;
  int br      = d->m_BorderRadius;

  auto drawDigitPair = [&](int x, int value) {
    QString text = QString(QStringLiteral("%1")).arg(value, 2, 10, QChar('0'));

    QRectF topRect(x, 0, dw, dh / 2);
    QRectF bottomRect(x, dh / 2, dw, dh / 2);

    QPainterPath topPath;
    topPath.moveTo(topRect.bottomLeft());
    topPath.lineTo(topRect.x(), topRect.y() + br);
    topPath.quadTo(topRect.topLeft(), QPointF(topRect.x() + br, topRect.y()));
    topPath.lineTo(topRect.right() - br, topRect.y());
    topPath.quadTo(topRect.topRight(), QPointF(topRect.right(), topRect.y() + br));
    topPath.lineTo(topRect.bottomRight());
    topPath.closeSubpath();

    QPainterPath bottomPath;
    bottomPath.moveTo(bottomRect.topLeft());
    bottomPath.lineTo(bottomRect.x(), bottomRect.bottom() - br);
    bottomPath.quadTo(bottomRect.bottomLeft(), QPointF(bottomRect.x() + br, bottomRect.bottom()));
    bottomPath.lineTo(bottomRect.right() - br, bottomRect.bottom());
    bottomPath.quadTo(bottomRect.bottomRight(), QPointF(bottomRect.right(), bottomRect.bottom() - br));
    bottomPath.lineTo(bottomRect.topRight());
    bottomPath.closeSubpath();

    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawPath(topPath);
    painter.drawPath(bottomPath);

    painter.setPen(lineColor);
    painter.drawLine(QPointF(x + 1, dh / 2.0), QPointF(x + dw - 1, dh / 2.0));

    painter.setFont(digitFont);
    painter.setPen(textColor);
    QRectF fullRect(x, 0, dw, dh);
    painter.drawText(fullRect, Qt::AlignCenter, text);
  };

  int sepW           = 20;
  auto drawSeparator = [&](int x) {
    painter.setFont(separatorFont);
    painter.setPen(separatorColor);
    painter.drawText(QRectF(x, 0, sepW, dh), Qt::AlignCenter, QStringLiteral(":"));
  };

  struct Section
  {
    int value;
    bool show;
  };

  QList<Section> sections;
  sections.append({ days, d->m_IsShowDays });
  sections.append({ hours, d->m_IsShowHours });
  sections.append({ minutes, d->m_IsShowMinutes });
  sections.append({ seconds, d->m_IsShowSeconds });

  int xPos   = 0;
  bool first = true;
  for (auto const &sec : sections) {
    if (!sec.show)
      continue;
    if (!first) {
      drawSeparator(xPos);
      xPos += sepW;
    }
    drawDigitPair(xPos, sec.value);
    xPos += dw;
    first = false;
  }
}

void NXCountdown::_updateSize()
{
  Q_D(NXCountdown);
  int dw           = d->m_DigitWidth;
  int dh           = d->m_DigitHeight;
  int sepW         = 20;
  int sectionCount = 0;
  if (d->m_IsShowDays) {
    sectionCount++;
  }
  if (d->m_IsShowHours) {
    sectionCount++;
  }
  if (d->m_IsShowMinutes) {
    sectionCount++;
  }
  if (d->m_IsShowSeconds) {
    sectionCount++;
  }
  int totalWidth = sectionCount * dw + qMax(0, sectionCount - 1) * sepW;
  setFixedSize(totalWidth, dh);
}
