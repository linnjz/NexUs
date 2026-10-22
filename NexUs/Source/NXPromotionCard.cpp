#include "NXPromotionCard.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QRadialGradient>
#include <QtMath>

#include "NXTheme.h"
#include "private/NXPromotionCardPrivate.h"
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QS_SET_CREF(QPixmap), CardPixmap)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QS_SET_CREF(QString), CardTitle)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QS_SET_CREF(QString), PromotionTitle)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QS_SET_CREF(QString), Title)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QS_SET_CREF(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QS_SET_CREF(QColor), CardTitleColor)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QS_SET_CREF(QColor), PromotionTitleColor)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QS_SET_CREF(QColor), PromotionTitleBaseColor)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QS_SET_CREF(QColor), TitleColor)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QS_SET_CREF(QColor), SubTitleColor)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, int, CardTitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, int, PromotionTitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, int, TitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, int, SubTitlePixelSize)

NXPromotionCard::NXPromotionCard(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXPromotionCardPrivate())
{
  Q_D(NXPromotionCard);
  d->q_ptr                       = this;
  d->_pBorderRadius              = 5;
  d->_pPressRadius               = 0;
  d->_pHoverOpacity              = 0;
  d->_pHorizontalCardPixmapRatio = 1;
  d->_pVerticalCardPixmapRatio   = 1;
  d->_pCardTitle                 = QStringLiteral("");
  d->_pPromotionTitle            = QStringLiteral("");
  d->_pTitle                     = QStringLiteral("");
  d->_pSubTitle                  = QStringLiteral("");
  d->_pCardTitleColor            = Qt::white;
  d->_pPromotionTitleColor       = Qt::white;
  d->_pTitleColor                = Qt::white;
  d->_pSubTitleColor             = Qt::white;
  d->_pPromotionTitleBaseColor   = QColor(0, 0, 0, 120);
  d->_pCardTitlePixelSize        = 22;
  d->_pPromotionTitlePixelSize   = 12;
  d->_pTitlePixelSize            = 25;
  d->_pSubTitlePixelSize         = 16;
  setMouseTracking(true);
  setObjectName("NXPromotionCard");
  setStyleSheet(QStringLiteral("#NXPromotionCard{background-color:transparent;}"));
  d->_hoverGradient = new QRadialGradient();
  d->_hoverGradient->setRadius(170);
  d->_hoverGradient->setColorAt(0, QColor(0xFF, 0xFF, 0xFF, 40));
  d->_hoverGradient->setColorAt(1, QColor(0xFF, 0xFF, 0xFF, 0));

  d->_pressGradient = new QRadialGradient();
  d->_pressGradient->setRadius(170);
  d->_pressGradient->setColorAt(0, QColor(0xFF, 0xFF, 0xFF, 0));
  d->_pressGradient->setColorAt(1, QColor(0xFF, 0xFF, 0xFF, 40));
}

NXPromotionCard::~NXPromotionCard()
{
  Q_D(const NXPromotionCard);
  delete d->_hoverGradient;
  delete d->_pressGradient;
}

void NXPromotionCard::setHorizontalCardPixmapRatio(qreal pixmapRatio)
{
  Q_D(NXPromotionCard);
  if (pixmapRatio > 0 && pixmapRatio <= 1) {
    Q_EMIT pHorizontalCardPixmapRatioChanged();
    d->_pHorizontalCardPixmapRatio = pixmapRatio;
  }
}

qreal NXPromotionCard::getHorizontalCardPixmapRatio() const
{
  Q_D(const NXPromotionCard);
  return d->_pHorizontalCardPixmapRatio;
}

void NXPromotionCard::setVerticalCardPixmapRatio(qreal pixmapRatio)
{
  Q_D(NXPromotionCard);
  if (pixmapRatio > 0 && pixmapRatio <= 1) {
    Q_EMIT pVerticalCardPixmapRatioChanged();
    d->_pVerticalCardPixmapRatio = pixmapRatio;
  }
}

qreal NXPromotionCard::getVerticalCardPixmapRatio() const
{
  Q_D(const NXPromotionCard);
  return d->_pVerticalCardPixmapRatio;
}

bool NXPromotionCard::event(QEvent *event)
{
  Q_D(NXPromotionCard);
  switch (event->type()) {
  case QEvent::MouseButtonPress : {
    QMouseEvent *mouseEvent              = dynamic_cast<QMouseEvent *>(event);
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(d, "pPressOpacity");
    connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
      update();
    });
    connect(opacityAnimation, &QPropertyAnimation::finished, this, [=]() {
      d->_isPressAnimationFinished = true;
    });
    opacityAnimation->setDuration(300);
    opacityAnimation->setEasingCurve(QEasingCurve::InQuad);
    opacityAnimation->setStartValue(1);
    opacityAnimation->setEndValue(0);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *pressAnimation = new QPropertyAnimation(d, "pPressRadius");
    connect(pressAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
      d->_pressGradient->setRadius(value.toReal());
    });
    pressAnimation->setDuration(300);
    pressAnimation->setEasingCurve(QEasingCurve::InQuad);
    pressAnimation->setStartValue(30);
    pressAnimation->setEndValue(d->_getLongestDistance(mouseEvent->pos()) * 1.8);
    pressAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    d->_isPressAnimationFinished = false;
    d->_pressGradient->setFocalPoint(mouseEvent->pos());
    d->_pressGradient->setCenter(mouseEvent->pos());
    //点击后隐藏Hover效果
    d->_startHoverOpacityAnimation(false);
    break;
  }
  case QEvent::MouseButtonRelease : {
    Q_EMIT promotionCardClicked();
    break;
  }
  case QEvent::MouseMove : {
    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
    if (d->_pHoverOpacity < 1 && d->_isPressAnimationFinished) {
      d->_startHoverOpacityAnimation(true);
    }
    if (d->_isPressAnimationFinished) {
      d->_hoverGradient->setCenter(mouseEvent->pos());
      d->_hoverGradient->setFocalPoint(mouseEvent->pos());
    }
    update();
    break;
  }
  case QEvent::Enter : {
    d->_startHoverOpacityAnimation(true);
    break;
  }
  case QEvent::Leave : {
    d->_startHoverOpacityAnimation(false);
    break;
  }
  default : {
    break;
  }
  }
  return QWidget::event(event);
}

void NXPromotionCard::paintEvent(QPaintEvent *event)
{
  Q_D(NXPromotionCard);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(Qt::NoPen);
  //阴影绘制
  nxTheme->drawEffectShadow(&painter, rect(), d->_shadowBorderWidth, d->_pBorderRadius);
  QRect foregroundRect(d->_shadowBorderWidth, d->_shadowBorderWidth, width() - 2 * d->_shadowBorderWidth, height() - 2 * d->_shadowBorderWidth);
  QPainterPath path;
  path.addRoundedRect(foregroundRect, d->_pBorderRadius, d->_pBorderRadius);
  painter.setClipPath(path);
  //背景图片绘制
  if (!d->_pCardPixmap.isNull()) {
    //源区域计算
    QRect pixSourceRect    = d->_pCardPixmap.rect();
    qreal horizontalOffset = d->_pCardPixmap.width() * (1 - d->_pHorizontalCardPixmapRatio) / 2;
    qreal verticalOffset   = d->_pCardPixmap.height() * (1 - d->_pVerticalCardPixmapRatio) / 2;
    pixSourceRect.adjust(horizontalOffset, verticalOffset, -horizontalOffset, -verticalOffset);
    painter.drawPixmap(foregroundRect, d->_pCardPixmap, pixSourceRect);
  }
  //文字绘制
  painter.save();
  QFont font = painter.font();
  //卡片标题
  font.setWeight(QFont::Bold);
  font.setPixelSize(d->_pCardTitlePixelSize);
  painter.setFont(font);
  painter.setPen(d->_pCardTitleColor);
  painter.drawText(QRect(25, 25, foregroundRect.width() - 25, foregroundRect.height()), Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, d->_pCardTitle);
  //标题
  font.setWeight(QFont::Bold);
  font.setPixelSize(d->_pTitlePixelSize);
  painter.setFont(font);
  painter.setPen(d->_pTitleColor);
  int titleTextHeight = painter.fontMetrics().height();
  QRect titleRect(25, (height() - titleTextHeight) / 2, foregroundRect.width() - 25, titleTextHeight);
  painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignBottom | Qt::TextSingleLine, d->_pTitle);
  //推广标题
  if (!d->_pPromotionTitle.isEmpty()) {
    font.setWeight(QFont::Normal);
    font.setPixelSize(d->_pPromotionTitlePixelSize);
    painter.setFont(font);
    int promotionTitleTextWidth  = painter.fontMetrics().horizontalAdvance(d->_pPromotionTitle);
    int promotionTitleTextHeight = painter.fontMetrics().height();
    QRect promotionTitleTextRect(32, titleRect.top() - promotionTitleTextHeight - 5, foregroundRect.width() / 2 - 25, promotionTitleTextHeight);
    //背景绘制
    painter.setPen(Qt::NoPen);
    painter.setBrush(d->_pPromotionTitleBaseColor);
    painter.drawRoundedRect(
        QRect(25, promotionTitleTextRect.top() - 2, promotionTitleTextWidth + 14, promotionTitleTextHeight + 4), 8, 8);
    //文字绘制
    painter.setPen(d->_pPromotionTitleColor);
    painter.drawText(promotionTitleTextRect, Qt::AlignLeft | Qt::AlignBottom | Qt::TextSingleLine, d->_pPromotionTitle);
  }
  //副标题
  font.setWeight(QFont::Medium);
  font.setPixelSize(d->_pSubTitlePixelSize);
  painter.setFont(font);
  painter.setPen(d->_pSubTitleColor);
  painter.drawText(QRect(25, titleRect.bottom(), foregroundRect.width() / 2 - 25, height() / 2), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, d->_pSubTitle);

  painter.restore();
  //效果阴影绘制
  if (d->_isPressAnimationFinished) {
    //覆盖阴影绘制
    painter.setOpacity(d->_pHoverOpacity);
    painter.setBrush(*d->_hoverGradient);
    painter.drawEllipse(d->_hoverGradient->center(), d->_hoverGradient->radius(), d->_hoverGradient->radius());
  } else {
    //点击阴影绘制
    painter.setOpacity(d->_pPressOpacity);
    painter.setBrush(*d->_pressGradient);
    painter.drawEllipse(d->_pressGradient->center(), d->_pPressRadius, d->_pPressRadius / 1.1);
  }
  painter.restore();
}
