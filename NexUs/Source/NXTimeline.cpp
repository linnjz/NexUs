#include "NXTimeline.h"

#include <QFontMetrics>
#include <QPainter>

#include "NXTheme.h"
#include "private/NXTimelinePrivate.h"

NXTimelinePrivate::NXTimelinePrivate(QObject *parent)
    : QObject(parent)
{
}

NXTimelinePrivate::~NXTimelinePrivate()
{
}

NXTimeline::NXTimeline(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXTimelinePrivate())
{
  Q_D(NXTimeline);
  d->q_ptr = this;
  setObjectName("NXTimeline");
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXTimeline::~NXTimeline()
{
}

void NXTimeline::addItem(TimelineItem const &item)
{
  Q_D(NXTimeline);
  d->m_Items.append(item);
  updateGeometry();
  update();
}

void NXTimeline::clearItems()
{
  Q_D(NXTimeline);
  d->m_Items.clear();
  updateGeometry();
  update();
}

int NXTimeline::getItemCount() const
{
  return d_ptr->m_Items.count();
}

void NXTimeline::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXTimeline);
  if (d->m_Items.isEmpty()) {
    return;
  }

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  int const leftMargin         = 80;
  int const circleDiameter     = 12;
  int const circleRadius       = circleDiameter / 2;
  int const iconCircleDiameter = 24;
  int const iconCircleRadius   = iconCircleDiameter / 2;
  int const lineX              = leftMargin + circleRadius; // x=86
  int const contentX           = 100;
  int const contentWidth       = width() - 120;
  int const itemGap            = 8;

  QColor primaryColor     = NXThemeColor(d->m_ThemeMode, PrimaryNormal);
  QColor borderColor      = NXThemeColor(d->m_ThemeMode, BasicBorder);
  QColor textColor        = NXThemeColor(d->m_ThemeMode, BasicText);
  QColor detailsTextColor = NXThemeColor(d->m_ThemeMode, BasicDetailsText);

  // Prepare fonts
  QFont timestampFont = font();
  timestampFont.setPixelSize(12);

  QFont titleFont = font();
  titleFont.setPixelSize(14);
  titleFont.setBold(true);

  QFont contentFont = font();
  contentFont.setPixelSize(13);

  QFont iconFont(QStringLiteral("NXAwesome"));

  int currentY = 0;

  for (int i = 0; i < d->m_Items.count(); ++i) {
    TimelineItem const &item  = d->m_Items[i];
    bool hasIcon              = (item.icon != NXIconType::None);
    int currentCircleDiameter = hasIcon ? iconCircleDiameter : circleDiameter;
    int currentCircleRadius   = currentCircleDiameter / 2;

    // Calculate content height
    painter.setFont(titleFont);
    QFontMetrics titleFm(titleFont);
    int titleHeight = titleFm.height();

    painter.setFont(contentFont);
    QFontMetrics contentFm(contentFont);
    int contentTextHeight = 0;
    if (!item.content.isEmpty()) {
      QRect contentBound = contentFm.boundingRect(QRect(0, 0, contentWidth, 10000), Qt::TextWordWrap, item.content);
      contentTextHeight  = contentBound.height();
    }

    int itemContentHeight = titleHeight + (contentTextHeight > 0 ? 4 + contentTextHeight : 0);
    int itemHeight        = qMax(60, itemContentHeight + 10);

    // Circle center Y
    int circleCenterY = currentY + currentCircleRadius;

    // Draw timestamp (left side, right-aligned)
    painter.setFont(timestampFont);
    painter.setPen(detailsTextColor);
    QFontMetrics timestampFm(timestampFont);
    int timestampHeight = timestampFm.height();
    QRect timestampRect(0, circleCenterY - timestampHeight / 2, leftMargin - 10, timestampHeight);
    painter.drawText(timestampRect, Qt::AlignRight | Qt::AlignVCenter, item.timestamp);

    // Draw circle
    painter.setPen(Qt::NoPen);
    if (i == 0) {
      painter.setBrush(primaryColor);
    } else {
      painter.setBrush(borderColor);
    }

    if (hasIcon) {
      // Draw larger circle with icon
      painter.drawEllipse(lineX - iconCircleRadius, circleCenterY - iconCircleRadius, iconCircleDiameter, iconCircleDiameter);

      // Draw icon inside circle
      iconFont.setPixelSize(12);
      painter.setFont(iconFont);
      painter.setPen(Qt::white);
      QRect iconRect(lineX - iconCircleRadius, circleCenterY - iconCircleRadius, iconCircleDiameter, iconCircleDiameter);
      painter.drawText(iconRect, Qt::AlignCenter, QChar((unsigned short) item.icon));
    } else {
      painter.drawEllipse(leftMargin, circleCenterY - circleRadius, circleDiameter, circleDiameter);
    }

    // Draw vertical line to next item
    if (i < d->m_Items.count() - 1) {
      QPen linePen(borderColor, 1);
      painter.setPen(linePen);
      int lineStartY = circleCenterY + currentCircleRadius;
      int lineEndY   = currentY + itemHeight + itemGap;
      painter.drawLine(lineX, lineStartY, lineX, lineEndY);
    }

    // Draw title
    painter.setFont(titleFont);
    painter.setPen(textColor);
    QRect titleRect(contentX, currentY, contentWidth, titleHeight);
    painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, item.title);

    // Draw content
    if (!item.content.isEmpty()) {
      painter.setFont(contentFont);
      painter.setPen(detailsTextColor);
      QRect contentRect(contentX, currentY + titleHeight + 4, contentWidth, contentTextHeight);
      painter.drawText(contentRect, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop, item.content);
    }

    currentY += itemHeight + itemGap;
  }
}

QSize NXTimeline::sizeHint() const
{
  Q_CONST_D(NXTimeline);
  if (d->m_Items.isEmpty()) {
    return QSize(400, 0);
  }

  QFont titleFont = font();
  titleFont.setPixelSize(14);
  titleFont.setBold(true);

  QFont contentFont = font();
  contentFont.setPixelSize(13);

  int contentWidth  = 400 - 120;
  int totalHeight   = 0;
  int const itemGap = 8;

  for (int i = 0; i < d->m_Items.count(); ++i) {
    QFontMetrics titleFm(titleFont);
    int titleHeight = titleFm.height();

    int contentTextHeight = 0;
    if (!d->m_Items[i].content.isEmpty()) {
      QFontMetrics contentFm(contentFont);
      QRect contentBound =
          contentFm.boundingRect(QRect(0, 0, contentWidth, 10000), Qt::TextWordWrap, d->m_Items[i].content);
      contentTextHeight = contentBound.height();
    }

    int itemContentHeight = titleHeight + (contentTextHeight > 0 ? 4 + contentTextHeight : 0);
    int itemHeight        = qMax(60, itemContentHeight + 10);
    totalHeight += itemHeight;
    if (i < d->m_Items.count() - 1) {
      totalHeight += itemGap;
    }
  }

  return QSize(400, totalHeight);
}
