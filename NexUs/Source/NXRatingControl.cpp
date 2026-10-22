#include "NXRatingControl.h"

#include <QMouseEvent>
#include <QPainter>

#include "NXDef.h"
#include "NXTheme.h"
#include "private/NXRatingControlPrivate.h"

Q_PROPERTY_CREATE_CPP(NXRatingControl, int, MaxRating)
Q_PROPERTY_CREATE_CPP(NXRatingControl, int, StarSize)
Q_PROPERTY_CREATE_CPP(NXRatingControl, int, Spacing)
Q_PROPERTY_CREATE_CPP(NXRatingControl, bool, IsReadOnly)

void NXRatingControl::setRating(int Rating)
{
  Q_D(NXRatingControl);
  if (d->_pRating == Rating) {
    return;
  }
  d->_pRating = Rating;
  Q_EMIT pRatingChanged();
  Q_EMIT ratingChanged(Rating);
  update();
}

int NXRatingControl::getRating() const
{
  return d_ptr->_pRating;
}

NXRatingControlPrivate::NXRatingControlPrivate(QObject *parent)
    : QObject(parent)
{
}

NXRatingControlPrivate::~NXRatingControlPrivate()
{
}

NXRatingControl::NXRatingControl(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXRatingControlPrivate())
{
  Q_D(NXRatingControl);
  d->q_ptr        = this;
  d->_pMaxRating  = 5;
  d->_pRating     = 0;
  d->_pStarSize   = 24;
  d->_pSpacing    = 4;
  d->_pIsReadOnly = false;
  setObjectName("NXRatingControl");
  setMouseTracking(true);
  setFixedHeight(d->_pStarSize + 4);
  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXRatingControl::~NXRatingControl()
{
}

void NXRatingControl::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXRatingControl);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(d->_pStarSize - 4);
  painter.setFont(iconFont);

  int effectiveRating = d->_pRating;
  if (!d->_pIsReadOnly && d->_hoveredStar >= 0) {
    effectiveRating = d->_hoveredStar + 1;
  }

  for (int i = 0; i < d->_pMaxRating; ++i) {
    QRect starRect(i * (d->_pStarSize + d->_pSpacing), 2, d->_pStarSize, d->_pStarSize);
    if (i < effectiveRating) {
      painter.setPen(NXThemeColor(d->_themeMode, PrimaryNormal));
    } else {
      painter.setPen(NXThemeColor(d->_themeMode, BasicBorderDeep));
    }
    painter.drawText(starRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Star));
  }
}

void NXRatingControl::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXRatingControl);
  if (d->_pIsReadOnly) {
    QWidget::mouseMoveEvent(event);
    return;
  }
  int x           = event->pos().x();
  int starUnit    = d->_pStarSize + d->_pSpacing;
  int hoveredStar = -1;
  if (x >= 0 && starUnit > 0) {
    hoveredStar = x / starUnit;
    if (hoveredStar >= d->_pMaxRating) {
      hoveredStar = -1;
    }
  }
  if (d->_hoveredStar != hoveredStar) {
    d->_hoveredStar = hoveredStar;
    update();
  }
  QWidget::mouseMoveEvent(event);
}

void NXRatingControl::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXRatingControl);
  if (!d->_pIsReadOnly && d->_hoveredStar >= 0) {
    setRating(d->_hoveredStar + 1);
  }
  QWidget::mousePressEvent(event);
}

void NXRatingControl::leaveEvent(QEvent *event)
{
  Q_D(NXRatingControl);
  d->_hoveredStar = -1;
  update();
  QWidget::leaveEvent(event);
}

QSize NXRatingControl::sizeHint() const
{
  Q_D(const NXRatingControl);
  int w = d->_pMaxRating * (d->_pStarSize + d->_pSpacing) - d->_pSpacing;
  int h = d->_pStarSize + 4;
  return QSize(w, h);
}
