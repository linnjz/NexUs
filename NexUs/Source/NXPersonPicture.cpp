#include "NXPersonPicture.h"

#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXPersonPicturePrivate.h"
Q_PROPERTY_CREATE_CPP(NXPersonPicture, QPS_SCREF_PAEMIT(QString), DisplayName)
Q_PROPERTY_CREATE_CPP(NXPersonPicture, QPS_SCREF_PAEMIT(QPixmap), Picture)

void NXPersonPicture::setPictureSize(int PictureSize)
{
  Q_D(NXPersonPicture);
  d->m_PictureSize = PictureSize;
  setFixedSize(PictureSize, PictureSize);
  Q_EMIT signalPictureSizeChanged();
}

int NXPersonPicture::getPictureSize() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  return d_ptr->m_PictureSize;
}

NXPersonPicture::NXPersonPicture(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXPersonPicturePrivate())
{
  Q_D(NXPersonPicture);
  d->q_ptr         = this;
  d->m_PictureSize = 40;
  setFixedSize(d->m_PictureSize, d->m_PictureSize);
  setObjectName("NXPersonPicture");
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXPersonPicture::~NXPersonPicture()
{
}

QSize NXPersonPicture::sizeHint() const
{
  int size = d_ptr->m_PictureSize;
  return QSize(size, size);
}

void NXPersonPicture::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXPersonPicture);
  QPainter painter(this);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);

  int size          = d->m_PictureSize;
  qreal borderWidth = 2.0;
  QRectF circleRect(borderWidth / 2, borderWidth / 2, size - borderWidth, size - borderWidth);
  QPointF center = circleRect.center();
  qreal radius   = circleRect.width() / 2;

  if (!d->m_Picture.isNull()) {
    // Draw circular-clipped pixmap
    painter.save();
    QPainterPath clipPath;
    clipPath.addEllipse(center, radius, radius);
    painter.setClipPath(clipPath);
    painter.drawPixmap(
        circleRect.toRect(),
        d->m_Picture.scaled(circleRect.size().toSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    painter.restore();
  } else if (!d->m_DisplayName.isEmpty()) {
    // Draw circle with PrimaryNormal background and white initials
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
    painter.drawEllipse(center, radius, radius);
    painter.restore();

    painter.save();
    QString initials = d->m_DisplayName.left(2);
    QFont font       = this->font();
    font.setPixelSize(size / 3);
    font.setWeight(QFont::Bold);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(circleRect.toRect(), Qt::AlignCenter, initials);
    painter.restore();
  } else {
    // Draw circle with BasicBase background and user icon
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBase));
    painter.drawEllipse(center, radius, radius);
    painter.restore();

    painter.save();
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(size / 3);
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    painter.drawText(circleRect.toRect(), Qt::AlignCenter, QChar((unsigned short) NXIconType::User));
    painter.restore();
  }

  // Draw 2px circular border
  painter.save();
  painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), borderWidth));
  painter.setBrush(Qt::NoBrush);
  painter.drawEllipse(center, radius, radius);
  painter.restore();
}

NXPersonPicturePrivate::NXPersonPicturePrivate(QObject *parent)
    : QObject(parent)
{
}

NXPersonPicturePrivate::~NXPersonPicturePrivate()
{
}
