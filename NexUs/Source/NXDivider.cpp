#include "NXDivider.h"

#include <QFontMetrics>
#include <QPainter>

#include "NXTheme.h"
#include "private/NXDividerPrivate.h"

Q_PROPERTY_CREATE_CPP(NXDivider, QS_SET_CREF(QString), Text)
Q_PROPERTY_CREATE_CPP(NXDivider, Qt::Orientation, Orientation)
Q_PROPERTY_CREATE_CPP(NXDivider, int, ContentPosition)

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
  d->_pOrientation     = Qt::Horizontal;
  d->_pText            = QStringLiteral("");
  d->_pContentPosition = Center;
  setObjectName("NXDivider");
  setFixedHeight(20);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
  connect(this, &NXDivider::pOrientationChanged, this, [=]() {
    if (d->_pOrientation == Qt::Horizontal) {
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
  connect(this, &NXDivider::pTextChanged, this, [=]() {
    update();
  });
  connect(this, &NXDivider::pContentPositionChanged, this, [=]() {
    update();
  });
}

NXDivider::NXDivider(const QString &text, QWidget *parent)
    : NXDivider(parent)
{
  Q_D(NXDivider);
  d->_pText = text;
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

  QColor lineColor = NXThemeColor(d->_themeMode, BasicBorder);
  QPen linePen(lineColor, 1);

  if (d->_pOrientation == Qt::Vertical) {
    // Vertical line at horizontal center
    int centerX = width() / 2;
    painter.setPen(linePen);
    painter.drawLine(centerX, 0, centerX, height());
    return;
  }

  // Horizontal mode
  int centerY = height() / 2;

  if (d->_pText.isEmpty()) {
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
  int textWidth  = fm.horizontalAdvance(d->_pText);
  int textHeight = fm.height();
  int padding    = 8;

  int textX = 0;
  if (d->_pContentPosition == Left) {
    textX = 20;
  } else if (d->_pContentPosition == Center) {
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
  QColor textColor = NXThemeColor(d->_themeMode, BasicDetailsText);
  painter.setPen(textColor);
  QRectF textRect(textX, (height() - textHeight) / 2, textWidth, textHeight);
  painter.drawText(textRect, Qt::AlignCenter, d->_pText);
}

QSize NXDivider::sizeHint() const
{
  Q_D(const NXDivider);
  if (d->_pOrientation == Qt::Horizontal) {
    return QSize(200, 20);
  }
  return QSize(20, 200);
}
