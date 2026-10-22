#include "NXCountdown.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QTimer>

#include "NXTheme.h"
#include "private/NXCountdownPrivate.h"
Q_PROPERTY_CREATE_CPP(NXCountdown, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXCountdown, int, DigitWidth)
Q_PROPERTY_CREATE_CPP(NXCountdown, int, DigitHeight)
Q_PROPERTY_CREATE_CPP(NXCountdown, int, DigitSpacing)
Q_PROPERTY_CREATE_CPP(NXCountdown, int, FontPixelSize)
Q_PROPERTY_CREATE_CPP(NXCountdown, bool, IsShowDays)
Q_PROPERTY_CREATE_CPP(NXCountdown, bool, IsShowHours)
Q_PROPERTY_CREATE_CPP(NXCountdown, bool, IsShowMinutes)
Q_PROPERTY_CREATE_CPP(NXCountdown, bool, IsShowSeconds)

NXCountdown::NXCountdown(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXCountdownPrivate())
{
  Q_D(NXCountdown);
  d->q_ptr           = this;
  d->_pBorderRadius  = 6;
  d->_pIsShowDays    = true;
  d->_pIsShowHours   = true;
  d->_pIsShowMinutes = true;
  d->_pIsShowSeconds = true;
  d->_pDigitWidth    = 44;
  d->_pDigitHeight   = 56;
  d->_pDigitSpacing  = 6;
  d->_pFontPixelSize = 24;
  d->_pFlipAngle     = 0;
  d->_themeMode      = nxTheme->getThemeMode();

  d->_timer = new QTimer(this);
  d->_timer->setInterval(1000);
  connect(d->_timer, &QTimer::timeout, d, &NXCountdownPrivate::onTimerTick);

  _updateSize();

  connect(this, &NXCountdown::pIsShowDaysChanged, this, &NXCountdown::_updateSize);
  connect(this, &NXCountdown::pIsShowHoursChanged, this, &NXCountdown::_updateSize);
  connect(this, &NXCountdown::pIsShowMinutesChanged, this, &NXCountdown::_updateSize);
  connect(this, &NXCountdown::pIsShowSecondsChanged, this, &NXCountdown::_updateSize);
  connect(this, &NXCountdown::pDigitWidthChanged, this, &NXCountdown::_updateSize);
  connect(this, &NXCountdown::pDigitHeightChanged, this, &NXCountdown::_updateSize);
  connect(this, &NXCountdown::pDigitSpacingChanged, this, &NXCountdown::_updateSize);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXCountdown::~NXCountdown()
{
}

void NXCountdown::setTargetDateTime(const QDateTime &dateTime)
{
  Q_D(NXCountdown);
  d->_targetDateTime   = dateTime;
  d->_useTargetMode    = true;
  qint64 secs          = QDateTime::currentDateTime().secsTo(dateTime);
  d->_remainingSeconds = qMax(static_cast<qint64>(0), secs);
  update();
}

QDateTime NXCountdown::getTargetDateTime() const
{
  return d_ptr->_targetDateTime;
}

void NXCountdown::setRemainingSeconds(qint64 seconds)
{
  Q_D(NXCountdown);
  d->_remainingSeconds = qMax(static_cast<qint64>(0), seconds);
  d->_useTargetMode    = false;
  update();
}

qint64 NXCountdown::getRemainingSeconds() const
{
  return d_ptr->_remainingSeconds;
}

void NXCountdown::start()
{
  Q_D(NXCountdown);
  if (d->_isRunning) {
    return;
  }
  d->_isRunning = true;
  if (d->_useTargetMode) {
    qint64 secs          = QDateTime::currentDateTime().secsTo(d->_targetDateTime);
    d->_remainingSeconds = qMax(static_cast<qint64>(0), secs);
  }
  d->_prevSeconds = -1;
  d->_prevMinutes = -1;
  d->_prevHours   = -1;
  d->_prevDays    = -1;
  d->_timer->start();
  update();
}

void NXCountdown::pause()
{
  Q_D(NXCountdown);
  d->_isRunning = false;
  d->_timer->stop();
}

void NXCountdown::resume()
{
  Q_D(NXCountdown);
  if (!d->_isRunning) {
    d->_isRunning = true;
    d->_timer->start();
  }
}

void NXCountdown::stop()
{
  Q_D(NXCountdown);
  d->_isRunning = false;
  d->_timer->stop();
  d->_remainingSeconds = 0;
  update();
}

bool NXCountdown::isRunning() const
{
  return d_ptr->_isRunning;
}

void NXCountdown::paintEvent(QPaintEvent *event)
{
  Q_D(NXCountdown);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  qint64 total = d->_remainingSeconds;
  int days     = static_cast<int>(total / 86400);
  int hours    = static_cast<int>((total % 86400) / 3600);
  int minutes  = static_cast<int>((total % 3600) / 60);
  int seconds  = static_cast<int>(total % 60);

  QFont digitFont = font();
  digitFont.setPixelSize(d->_pFontPixelSize);
  digitFont.setBold(true);

  QFont separatorFont = font();
  separatorFont.setPixelSize(d->_pFontPixelSize);
  separatorFont.setBold(true);

  QColor bgColor        = NXThemeColor(d->_themeMode, BasicBaseDeep);
  QColor textColor      = NXThemeColor(d->_themeMode, BasicText);
  QColor separatorColor = NXThemeColor(d->_themeMode, BasicTextNoFocus);
  QColor lineColor      = NXThemeColor(d->_themeMode, BasicBorder);

  int dw      = d->_pDigitWidth;
  int dh      = d->_pDigitHeight;
  int spacing = d->_pDigitSpacing;
  int br      = d->_pBorderRadius;

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
  sections.append({ days, d->_pIsShowDays });
  sections.append({ hours, d->_pIsShowHours });
  sections.append({ minutes, d->_pIsShowMinutes });
  sections.append({ seconds, d->_pIsShowSeconds });

  int xPos   = 0;
  bool first = true;
  for (const auto &sec : sections) {
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
  int dw           = d->_pDigitWidth;
  int dh           = d->_pDigitHeight;
  int sepW         = 20;
  int sectionCount = 0;
  if (d->_pIsShowDays) {
    sectionCount++;
  }
  if (d->_pIsShowHours) {
    sectionCount++;
  }
  if (d->_pIsShowMinutes) {
    sectionCount++;
  }
  if (d->_pIsShowSeconds) {
    sectionCount++;
  }
  int totalWidth = sectionCount * dw + qMax(0, sectionCount - 1) * sepW;
  setFixedSize(totalWidth, dh);
}
