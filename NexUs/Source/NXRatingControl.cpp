#include "NXRatingControl.h"

#include <QMouseEvent>
#include <QPainter>

#include "NXDef.h"
#include "NXTheme.h"
#include "private/NXRatingControlPrivate.h"

Q_PROPERTY_CREATE_CPP(NXRatingControl, QPS_PAEMIT(int), MaxRating)
Q_PROPERTY_CREATE_CPP(NXRatingControl, QPS_PAEMIT(int), StarSize)
Q_PROPERTY_CREATE_CPP(NXRatingControl, QPS_PAEMIT(int), Spacing)
Q_PROPERTY_CREATE_CPP(NXRatingControl, QPS_PAEMIT(bool), IsReadOnly)

void NXRatingControl::setRating(int Rating)
{
  Q_D(NXRatingControl);
  if (d->m_Rating == Rating) {
    return;
  }
  d->m_Rating = Rating;
  Q_EMIT signalRatingChanged();
  Q_EMIT ratingChanged(Rating);
  update();
}

int NXRatingControl::getRating() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  return d_ptr->m_Rating;
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
  d->m_MaxRating  = 5;
  d->m_Rating     = 0;
  d->m_StarSize   = 24;
  d->m_Spacing    = 4;
  d->m_IsReadOnly = false;
  setObjectName("NXRatingControl");
  setMouseTracking(true);
  setFixedHeight(d->m_StarSize + 4);
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
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
  iconFont.setPixelSize(d->m_StarSize - 4);
  painter.setFont(iconFont);

  int effectiveRating = d->m_Rating;
  if (!d->m_IsReadOnly && d->m_HoveredStar >= 0) {
    effectiveRating = d->m_HoveredStar + 1;
  }

  for (int i = 0; i < d->m_MaxRating; ++i) {
    QRect starRect(i * (d->m_StarSize + d->m_Spacing), 2, d->m_StarSize, d->m_StarSize);
    if (i < effectiveRating) {
      painter.setPen(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
    } else {
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorderDeep));
    }
    painter.drawText(starRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Star));
  }
}

void NXRatingControl::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXRatingControl);
  if (d->m_IsReadOnly) {
    QWidget::mouseMoveEvent(event);
    return;
  }
  int x           = event->pos().x();
  int starUnit    = d->m_StarSize + d->m_Spacing;
  int hoveredStar = -1;
  if (x >= 0 && starUnit > 0) {
    hoveredStar = x / starUnit;
    if (hoveredStar >= d->m_MaxRating) {
      hoveredStar = -1;
    }
  }
  if (d->m_HoveredStar != hoveredStar) {
    d->m_HoveredStar = hoveredStar;
    update();
  }
  QWidget::mouseMoveEvent(event);
}

void NXRatingControl::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXRatingControl);
  if (!d->m_IsReadOnly && d->m_HoveredStar >= 0) {
    setRating(d->m_HoveredStar + 1);
  }
  QWidget::mousePressEvent(event);
}

void NXRatingControl::leaveEvent(QEvent *event)
{
  Q_D(NXRatingControl);
  d->m_HoveredStar = -1;
  update();
  QWidget::leaveEvent(event);
}

QSize NXRatingControl::sizeHint() const
{
  Q_CONST_D(NXRatingControl);
  int w = d->m_MaxRating * (d->m_StarSize + d->m_Spacing) - d->m_Spacing;
  int h = d->m_StarSize + 4;
  return QSize(w, h);
}
