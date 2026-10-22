#include "NXStatCard.h"

#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXStatCardPrivate.h"

Q_PROPERTY_CREATE_CPP(NXStatCard, QS_SET_CREF(QString), Title)
Q_PROPERTY_CREATE_CPP(NXStatCard, QS_SET_CREF(QString), Value)
Q_PROPERTY_CREATE_CPP(NXStatCard, QS_SET_CREF(QString), Description)
Q_PROPERTY_CREATE_CPP(NXStatCard, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXStatCard, NXIconType::IconName, CardIcon)

NXStatCard::NXStatCard(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXStatCardPrivate())
{
  Q_D(NXStatCard);
  d->q_ptr          = this;
  d->_pBorderRadius = 8;
  d->_pCardIcon     = NXIconType::None;
  setObjectName("NXStatCard");
  setFixedSize(220, 120);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXStatCard::~NXStatCard()
{
}

void NXStatCard::setTrend(TrendType trend)
{
  Q_D(NXStatCard);
  d->_trend = trend;
  update();
}

NXStatCard::TrendType NXStatCard::getTrend() const
{
  return d_ptr->_trend;
}

void NXStatCard::setTrendText(const QString &text)
{
  Q_D(NXStatCard);
  d->_trendText = text;
  update();
}

QString NXStatCard::getTrendText() const
{
  return d_ptr->_trendText;
}

void NXStatCard::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  Q_D(NXStatCard);

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  // Background rounded rect
  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(QRectF(0.5, 0.5, width() - 1, height() - 1), d->_pBorderRadius, d->_pBorderRadius);
  painter.fillPath(backgroundPath, NXThemeColor(d->_themeMode, BasicBase));
  painter.setPen(QPen(NXThemeColor(d->_themeMode, BasicBorder), 1));
  painter.drawPath(backgroundPath);

  int padding  = 16;
  int currentX = padding;
  int titleY   = 16;

  // Icon + Title row
  if (d->_pCardIcon != NXIconType::None) {
    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(20);
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(d->_themeMode, PrimaryNormal));
    QChar iconChar = QChar(static_cast<int>(d->_pCardIcon));
    painter.drawText(currentX, titleY + 18, QString(iconChar));
    currentX += 24;
  }

  if (!d->_pTitle.isEmpty()) {
    QFont titleFont = font();
    titleFont.setPixelSize(13);
    painter.setFont(titleFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicDetailsText));
    painter.drawText(currentX, titleY + 16, d->_pTitle);
  }

  // Value (big number)
  if (!d->_pValue.isEmpty()) {
    QFont valueFont = font();
    valueFont.setPixelSize(28);
    valueFont.setBold(true);
    painter.setFont(valueFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicText));
    painter.drawText(padding, 42 + 26, d->_pValue);
  }

  // Trend + Description row
  int bottomY = 88 + 14;
  int trendX  = padding;

  if (d->_trend != None) {
    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(12);

    QFont trendFont = font();
    trendFont.setPixelSize(12);

    if (d->_trend == Up) {
      QColor greenColor(0x0F, 0x7B, 0x0F);
      painter.setFont(iconFont);
      painter.setPen(greenColor);
      QChar arrowChar = QChar(static_cast<int>(NXIconType::AngleUp));
      painter.drawText(trendX, bottomY, QString(arrowChar));
      trendX += 14;

      if (!d->_trendText.isEmpty()) {
        painter.setFont(trendFont);
        painter.drawText(trendX, bottomY, d->_trendText);
        trendX += QFontMetrics(trendFont).horizontalAdvance(d->_trendText) + 6;
      }
    } else if (d->_trend == Down) {
      QColor redColor = NXThemeColor(d->_themeMode, StatusDanger);
      painter.setFont(iconFont);
      painter.setPen(redColor);
      QChar arrowChar = QChar(static_cast<int>(NXIconType::AngleDown));
      painter.drawText(trendX, bottomY, QString(arrowChar));
      trendX += 14;

      if (!d->_trendText.isEmpty()) {
        painter.setFont(trendFont);
        painter.drawText(trendX, bottomY, d->_trendText);
        trendX += QFontMetrics(trendFont).horizontalAdvance(d->_trendText) + 6;
      }
    } else if (d->_trend == Neutral) {
      QColor neutralColor = NXThemeColor(d->_themeMode, BasicDetailsText);
      painter.setFont(trendFont);
      painter.setPen(neutralColor);
      painter.drawText(trendX, bottomY, QStringLiteral("\u2014"));
      trendX += QFontMetrics(trendFont).horizontalAdvance(QStringLiteral("\u2014")) + 6;

      if (!d->_trendText.isEmpty()) {
        painter.drawText(trendX, bottomY, d->_trendText);
        trendX += QFontMetrics(trendFont).horizontalAdvance(d->_trendText) + 6;
      }
    }
  }

  // Description
  if (!d->_pDescription.isEmpty()) {
    QFont descFont = font();
    descFont.setPixelSize(12);
    painter.setFont(descFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicDetailsText));
    painter.drawText(trendX, bottomY, d->_pDescription);
  }
}

NXStatCardPrivate::NXStatCardPrivate(QObject *parent)
    : QObject { parent }
{
}

NXStatCardPrivate::~NXStatCardPrivate()
{
}
