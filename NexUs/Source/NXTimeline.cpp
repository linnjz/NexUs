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
  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXTimeline::~NXTimeline()
{
}

void NXTimeline::addItem(const TimelineItem &item)
{
  Q_D(NXTimeline);
  d->_items.append(item);
  updateGeometry();
  update();
}

void NXTimeline::clearItems()
{
  Q_D(NXTimeline);
  d->_items.clear();
  updateGeometry();
  update();
}

int NXTimeline::getItemCount() const
{
  return d_ptr->_items.count();
}

void NXTimeline::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXTimeline);
  if (d->_items.isEmpty()) {
    return;
  }

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  const int leftMargin         = 80;
  const int circleDiameter     = 12;
  const int circleRadius       = circleDiameter / 2;
  const int iconCircleDiameter = 24;
  const int iconCircleRadius   = iconCircleDiameter / 2;
  const int lineX              = leftMargin + circleRadius; // x=86
  const int contentX           = 100;
  const int contentWidth       = width() - 120;
  const int itemGap            = 8;

  QColor primaryColor     = NXThemeColor(d->_themeMode, PrimaryNormal);
  QColor borderColor      = NXThemeColor(d->_themeMode, BasicBorder);
  QColor textColor        = NXThemeColor(d->_themeMode, BasicText);
  QColor detailsTextColor = NXThemeColor(d->_themeMode, BasicDetailsText);

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

  for (int i = 0; i < d->_items.count(); ++i) {
    const TimelineItem &item  = d->_items[i];
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
    if (i < d->_items.count() - 1) {
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
  Q_D(const NXTimeline);
  if (d->_items.isEmpty()) {
    return QSize(400, 0);
  }

  QFont titleFont = font();
  titleFont.setPixelSize(14);
  titleFont.setBold(true);

  QFont contentFont = font();
  contentFont.setPixelSize(13);

  int contentWidth  = 400 - 120;
  int totalHeight   = 0;
  const int itemGap = 8;

  for (int i = 0; i < d->_items.count(); ++i) {
    QFontMetrics titleFm(titleFont);
    int titleHeight = titleFm.height();

    int contentTextHeight = 0;
    if (!d->_items[i].content.isEmpty()) {
      QFontMetrics contentFm(contentFont);
      QRect contentBound =
          contentFm.boundingRect(QRect(0, 0, contentWidth, 10000), Qt::TextWordWrap, d->_items[i].content);
      contentTextHeight = contentBound.height();
    }

    int itemContentHeight = titleHeight + (contentTextHeight > 0 ? 4 + contentTextHeight : 0);
    int itemHeight        = qMax(60, itemContentHeight + 10);
    totalHeight += itemHeight;
    if (i < d->_items.count() - 1) {
      totalHeight += itemGap;
    }
  }

  return QSize(400, totalHeight);
}
