#include "NXImageCard.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXImageCardPrivate.h"

Q_PROPERTY_CREATE_CPP(NXImageCard, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXImageCard, bool, IsPreserveAspectCrop)

NXImageCard::NXImageCard(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXImageCardPrivate())
{
  Q_D(NXImageCard);
  d->q_ptr                  = this;
  d->_pBorderRadius         = 6;
  d->_pIsPreserveAspectCrop = true;
  setMinimumSize(350, 260);
  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXImageCard::~NXImageCard()
{
}

void NXImageCard::setCardImage(const QImage &cardImage)
{
  Q_D(NXImageCard);
  d->_pCardImage = cardImage;
  update();
  Q_EMIT pCardImageChanged();
}

QImage NXImageCard::getCardImage() const
{
  Q_D(const NXImageCard);
  return d->_pCardImage;
}

void NXImageCard::paintEvent(QPaintEvent *event)
{
  Q_D(NXImageCard);
  QPainter painter(this);
  painter.save();
  painter.setPen(Qt::NoPen);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
  QPainterPath path;
  path.addRoundedRect(rect(), d->_pBorderRadius, d->_pBorderRadius);
  painter.setClipPath(path);
  // 图片绘制
  if (d->_pIsPreserveAspectCrop) {
    qreal windowAspectRatio = (qreal) rect().width() / rect().height();
    qreal pixAspectRatio    = (qreal) d->_pCardImage.width() / d->_pCardImage.height();
    int targetPixWidth, targetPixHeight;
    if (windowAspectRatio < pixAspectRatio) {
      targetPixWidth  = qRound(d->_pCardImage.width() * windowAspectRatio / pixAspectRatio);
      targetPixHeight = d->_pCardImage.height();
    } else {
      targetPixWidth  = d->_pCardImage.width();
      targetPixHeight = qRound(d->_pCardImage.height() * pixAspectRatio / windowAspectRatio);
    }
    painter.drawImage(rect(), d->_pCardImage, QRect((d->_pCardImage.width() - targetPixWidth) / 2, (d->_pCardImage.height() - targetPixHeight) / 2, targetPixWidth, targetPixHeight));
  } else {
    painter.drawImage(rect(), d->_pCardImage);
  }
  painter.restore();
}
