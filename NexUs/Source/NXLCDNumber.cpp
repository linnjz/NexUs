#include "NXLCDNumber.h"

#include <QDateTime>
#include <QDebug>
#include <QTimer>

#include "DeveloperComponents/NXLCDNumberStyle.h"
#include "NXTheme.h"
#include "private/NXLCDNumberPrivate.h"

NXLCDNumber::NXLCDNumber(QWidget *parent)
    : QLCDNumber(parent)
    , d_ptr(new NXLCDNumberPrivate)
{
  Q_D(NXLCDNumber);
  d->q_ptr             = this;
  d->m_IsUseAutoClock  = false;
  d->m_AutoClockFormat = QStringLiteral("yyyy-MM-dd hh:mm:ss");
  setDigitCount(d->m_AutoClockFormat.length());
  setSegmentStyle(QLCDNumber::Flat);
  setObjectName("NXLCDNumber");
  setStyleSheet(QStringLiteral("#NXLCDNumber{background-color:transparent;}"));
  d->m_LcdNumberStyle = new NXLCDNumberStyle();
  setStyle(d->m_LcdNumberStyle);
  d->m_ClockTimer = new QTimer(this);
  connect(d->m_ClockTimer, &QTimer::timeout, this, [=]() {
    display(QDateTime::currentDateTime().toString(d->m_AutoClockFormat));
  });

  d->onThemeModeChanged(nxTheme->getThemeMode());
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXLCDNumberPrivate::onThemeModeChanged);
}

NXLCDNumber::NXLCDNumber(uint numDigits, QWidget *parent)
    : NXLCDNumber(parent)
{
  setDigitCount(numDigits);
}

NXLCDNumber::~NXLCDNumber()
{
  Q_D(NXLCDNumber);
  delete d->m_LcdNumberStyle;
}

void NXLCDNumber::setIsUseAutoClock(bool isUseAutoClock)
{
  Q_D(NXLCDNumber);
  d->m_IsUseAutoClock = isUseAutoClock;
  if (d->m_IsUseAutoClock) {
    setDigitCount(d->m_AutoClockFormat.length());
    display(QDateTime::currentDateTime().toString(d->m_AutoClockFormat));
    d->m_ClockTimer->start(200);
  } else {
    d->m_ClockTimer->stop();
    display(QStringLiteral(""));
  }
  Q_EMIT signalIsUseAutoClockChanged();
}

bool NXLCDNumber::getIsUseAutoClock() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXLCDNumber);
  return d->m_IsUseAutoClock;
}

void NXLCDNumber::setAutoClockFormat(QString const &autoClockFormat)
{
  Q_D(NXLCDNumber);
  d->m_AutoClockFormat = autoClockFormat;
  setDigitCount(d->m_AutoClockFormat.length());
  Q_EMIT signalAutoClockFormatChanged();
}

QString NXLCDNumber::getAutoClockFormat() const noexcept(std::is_nothrow_copy_constructible_v<QString>)
{
  Q_CONST_D(NXLCDNumber);
  return d->m_AutoClockFormat;
}

void NXLCDNumber::setIsTransparent(bool isTransparent)
{
  Q_D(NXLCDNumber);
  d->m_LcdNumberStyle->setIsTransparent(isTransparent);
  update();
  Q_EMIT signalIsTransparentChanged();
}

bool NXLCDNumber::getIsTransparent() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXLCDNumber);
  return d->m_LcdNumberStyle->getIsTransparent();
}

void NXLCDNumber::paintEvent(QPaintEvent *event)
{
  Q_D(NXLCDNumber);
  if (palette().color(QPalette::WindowText) != NXThemeColor(d->m_ThemeMode, BasicText)) {
    d->onThemeModeChanged(d->m_ThemeMode);
  }
  QLCDNumber::paintEvent(event);
}
