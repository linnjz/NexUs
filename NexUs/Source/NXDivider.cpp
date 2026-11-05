#include "NXDivider.h"

#include <QFontMetrics>
#include <QPainter>

#include "NXTheme.h"
#include "private/NXDividerPrivate.h"

Q_PROPERTY_CREATE_CPP(NXDivider, QPS_SCREF_PAEMIT(QString), Text)
Q_PROPERTY_CREATE_CPP(NXDivider, QPS_PAEMIT(Qt::Orientation), Orientation)
Q_PROPERTY_CREATE_CPP(NXDivider, QPS_PAEMIT(int), ContentPosition)

NXDividerPrivate::NXDividerPrivate(QObject *parent)
    : QObject(parent)
{
}

NXDividerPrivate::~NXDividerPrivate()
{
}

NXDivider::NXDivider(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXDividerPrivate())
{
  Q_D(NXDivider);
  d->q_ptr             = this;
  d->m_Orientation     = Qt::Horizontal;
  d->m_Text            = QStringLiteral("");
  d->m_ContentPosition = Center;
  setObjectName("NXDivider");
  setFixedHeight(20);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
  connect(this, &NXDivider::signalOrientationChanged, this, [=]() {
    if (d->m_Orientation == Qt::Horizontal) {
      setFixedHeight(20);
      setMinimumWidth(0);
      setMaximumWidth(QWIDGETSIZE_MAX);
      setMaximumHeight(20);
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else {
      setFixedWidth(20);
      setMinimumHeight(0);
      setMaximumHeight(QWIDGETSIZE_MAX);
      setMaximumWidth(20);
      setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
    updateGeometry();
    update();
  });
  connect(this, &NXDivider::signalTextChanged, this, [=]() {
    update();
  });
  connect(this, &NXDivider::signalContentPositionChanged, this, [=]() {
    update();
  });
}

NXDivider::NXDivider(QString const &text, QWidget *parent)
    : NXDivider(parent)
{
  Q_D(NXDivider);
  d->m_Text = text;
}

NXDivider::~NXDivider()
{
}

void NXDivider::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXDivider);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QColor lineColor = NXThemeColor(d->m_ThemeMode, BasicBorder);
  QPen linePen(lineColor, 1);

  if (d->m_Orientation == Qt::Vertical) {
    // Vertical line at horizontal center
    int centerX = width() / 2;
    painter.setPen(linePen);
    painter.drawLine(centerX, 0, centerX, height());
    return;
  }

  // Horizontal mode
  int centerY = height() / 2;

  if (d->m_Text.isEmpty()) {
    // Simple horizontal line
    painter.setPen(linePen);
    painter.drawLine(0, centerY, width(), centerY);
    return;
  }

  // Draw text with gap in line
  QFont textFont = this->font();
  textFont.setPixelSize(12);
  painter.setFont(textFont);
  QFontMetrics fm(textFont);
  int textWidth  = fm.horizontalAdvance(d->m_Text);
  int textHeight = fm.height();
  int padding    = 8;

  int textX = 0;
  if (d->m_ContentPosition == Left) {
    textX = 20;
  } else if (d->m_ContentPosition == Center) {
    textX = (width() - textWidth) / 2;
  } else {
    // Right
    textX = width() - 20 - textWidth;
  }

  int gapLeft  = textX - padding;
  int gapRight = textX + textWidth + padding;

  // Draw left line segment
  painter.setPen(linePen);
  if (gapLeft > 0) {
    painter.drawLine(0, centerY, gapLeft, centerY);
  }

  // Draw right line segment
  if (gapRight < width()) {
    painter.drawLine(gapRight, centerY, width(), centerY);
  }

  // Draw text
  QColor textColor = NXThemeColor(d->m_ThemeMode, BasicDetailsText);
  painter.setPen(textColor);
  QRectF textRect(textX, (height() - textHeight) / 2, textWidth, textHeight);
  painter.drawText(textRect, Qt::AlignCenter, d->m_Text);
}

QSize NXDivider::sizeHint() const
{
  Q_CONST_D(NXDivider);
  if (d->m_Orientation == Qt::Horizontal) {
    return QSize(200, 20);
  }
  return QSize(20, 200);
}
