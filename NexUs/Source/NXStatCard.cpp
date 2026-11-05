#include "NXStatCard.h"

#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXStatCardPrivate.h"

Q_PROPERTY_CREATE_CPP(NXStatCard, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXStatCard, QPS_SCREF_PAEMIT(QString), Value)
Q_PROPERTY_CREATE_CPP(NXStatCard, QPS_SCREF_PAEMIT(QString), Description)
Q_PROPERTY_CREATE_CPP(NXStatCard, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXStatCard, QPS_PAEMIT(NXIconType::IconName), CardIcon)

NXStatCard::NXStatCard(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXStatCardPrivate())
{
  Q_D(NXStatCard);
  d->q_ptr          = this;
  d->m_BorderRadius = 8;
  d->m_CardIcon     = NXIconType::None;
  setObjectName("NXStatCard");
  setFixedSize(220, 120);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXStatCard::~NXStatCard()
{
}

void NXStatCard::setTrend(TrendType trend)
{
  Q_D(NXStatCard);
  d->m_Trend = trend;
  update();
}

NXStatCard::TrendType NXStatCard::getTrend() const
{
  return d_ptr->m_Trend;
}

void NXStatCard::setTrendText(QString const &text)
{
  Q_D(NXStatCard);
  d->m_TrendText = text;
  update();
}

QString NXStatCard::getTrendText() const
{
  return d_ptr->m_TrendText;
}

void NXStatCard::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  Q_D(NXStatCard);

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  // Background rounded rect
  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(QRectF(0.5, 0.5, width() - 1, height() - 1), d->m_BorderRadius, d->m_BorderRadius);
  painter.fillPath(backgroundPath, NXThemeColor(d->m_ThemeMode, BasicBase));
  painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
  painter.drawPath(backgroundPath);

  int padding  = 16;
  int currentX = padding;
  int titleY   = 16;

  // Icon + Title row
  if (d->m_CardIcon != NXIconType::None) {
    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(20);
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
    QChar iconChar = QChar(static_cast<int>(d->m_CardIcon));
    painter.drawText(currentX, titleY + 18, QString(iconChar));
    currentX += 24;
  }

  if (!d->m_Title.isEmpty()) {
    QFont titleFont = font();
    titleFont.setPixelSize(13);
    painter.setFont(titleFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
    painter.drawText(currentX, titleY + 16, d->m_Title);
  }

  // Value (big number)
  if (!d->m_Value.isEmpty()) {
    QFont valueFont = font();
    valueFont.setPixelSize(28);
    valueFont.setBold(true);
    painter.setFont(valueFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    painter.drawText(padding, 42 + 26, d->m_Value);
  }

  // Trend + Description row
  int bottomY = 88 + 14;
  int trendX  = padding;

  if (d->m_Trend != None) {
    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(12);

    QFont trendFont = font();
    trendFont.setPixelSize(12);

    if (d->m_Trend == Up) {
      QColor greenColor(0x0F, 0x7B, 0x0F);
      painter.setFont(iconFont);
      painter.setPen(greenColor);
      QChar arrowChar = QChar(static_cast<int>(NXIconType::AngleUp));
      painter.drawText(trendX, bottomY, QString(arrowChar));
      trendX += 14;

      if (!d->m_TrendText.isEmpty()) {
        painter.setFont(trendFont);
        painter.drawText(trendX, bottomY, d->m_TrendText);
        trendX += QFontMetrics(trendFont).horizontalAdvance(d->m_TrendText) + 6;
      }
    } else if (d->m_Trend == Down) {
      QColor redColor = NXThemeColor(d->m_ThemeMode, StatusDanger);
      painter.setFont(iconFont);
      painter.setPen(redColor);
      QChar arrowChar = QChar(static_cast<int>(NXIconType::AngleDown));
      painter.drawText(trendX, bottomY, QString(arrowChar));
      trendX += 14;

      if (!d->m_TrendText.isEmpty()) {
        painter.setFont(trendFont);
        painter.drawText(trendX, bottomY, d->m_TrendText);
        trendX += QFontMetrics(trendFont).horizontalAdvance(d->m_TrendText) + 6;
      }
    } else if (d->m_Trend == Neutral) {
      QColor neutralColor = NXThemeColor(d->m_ThemeMode, BasicDetailsText);
      painter.setFont(trendFont);
      painter.setPen(neutralColor);
      painter.drawText(trendX, bottomY, QStringLiteral("\u2014"));
      trendX += QFontMetrics(trendFont).horizontalAdvance(QStringLiteral("\u2014")) + 6;

      if (!d->m_TrendText.isEmpty()) {
        painter.drawText(trendX, bottomY, d->m_TrendText);
        trendX += QFontMetrics(trendFont).horizontalAdvance(d->m_TrendText) + 6;
      }
    }
  }

  // Description
  if (!d->m_Description.isEmpty()) {
    QFont descFont = font();
    descFont.setPixelSize(12);
    painter.setFont(descFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
    painter.drawText(trendX, bottomY, d->m_Description);
  }
}

NXStatCardPrivate::NXStatCardPrivate(QObject *parent)
    : QObject { parent }
{
}

NXStatCardPrivate::~NXStatCardPrivate()
{
}
