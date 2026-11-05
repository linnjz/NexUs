#include "NXImageCard.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXImageCardPrivate.h"

Q_PROPERTY_CREATE_CPP(NXImageCard, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXImageCard, QPS_PAEMIT(bool), IsPreserveAspectCrop)

NXImageCard::NXImageCard(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXImageCardPrivate())
{
  Q_D(NXImageCard);
  d->q_ptr                  = this;
  d->m_BorderRadius         = 6;
  d->m_IsPreserveAspectCrop = true;
  setMinimumSize(350, 260);
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXImageCard::~NXImageCard()
{
}

void NXImageCard::setCardImage(QImage const &cardImage)
{
  Q_D(NXImageCard);
  d->m_CardImage = cardImage;
  update();
  Q_EMIT signalCardImageChanged();
}

QImage NXImageCard::getCardImage() const noexcept(std::is_nothrow_copy_constructible_v<QImage>)
{
  Q_CONST_D(NXImageCard);
  return d->m_CardImage;
}

void NXImageCard::paintEvent(QPaintEvent *event)
{
  Q_D(NXImageCard);
  QPainter painter(this);
  painter.save();
  painter.setPen(Qt::NoPen);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
  QPainterPath path;
  path.addRoundedRect(rect(), d->m_BorderRadius, d->m_BorderRadius);
  painter.setClipPath(path);
  // 图片绘制
  if (d->m_IsPreserveAspectCrop) {
    qreal windowAspectRatio = (qreal) rect().width() / rect().height();
    qreal pixAspectRatio    = (qreal) d->m_CardImage.width() / d->m_CardImage.height();
    int targetPixWidth, targetPixHeight;
    if (windowAspectRatio < pixAspectRatio) {
      targetPixWidth  = qRound(d->m_CardImage.width() * windowAspectRatio / pixAspectRatio);
      targetPixHeight = d->m_CardImage.height();
    } else {
      targetPixWidth  = d->m_CardImage.width();
      targetPixHeight = qRound(d->m_CardImage.height() * pixAspectRatio / windowAspectRatio);
    }
    painter.drawImage(rect(), d->m_CardImage, QRect((d->m_CardImage.width() - targetPixWidth) / 2, (d->m_CardImage.height() - targetPixHeight) / 2, targetPixWidth, targetPixHeight));
  } else {
    painter.drawImage(rect(), d->m_CardImage);
  }
  painter.restore();
}
