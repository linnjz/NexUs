#include "NXPromotionCard.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QRadialGradient>
#include <QtMath>

#include "NXTheme.h"
#include "private/NXPromotionCardPrivate.h"
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_SCREF_PAEMIT(QPixmap), CardPixmap)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_SCREF_PAEMIT(QString), CardTitle)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_SCREF_PAEMIT(QString), PromotionTitle)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_SCREF_PAEMIT(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_SCREF_PAEMIT(QColor), CardTitleColor)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_SCREF_PAEMIT(QColor), PromotionTitleColor)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_SCREF_PAEMIT(QColor), PromotionTitleBaseColor)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_SCREF_PAEMIT(QColor), TitleColor)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_SCREF_PAEMIT(QColor), SubTitleColor)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_PAEMIT(int), CardTitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_PAEMIT(int), PromotionTitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_PAEMIT(int), TitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXPromotionCard, QPS_PAEMIT(int), SubTitlePixelSize)

NXPromotionCard::NXPromotionCard(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXPromotionCardPrivate())
{
  Q_D(NXPromotionCard);
  d->q_ptr                       = this;
  d->m_BorderRadius              = 5;
  d->m_PressRadius               = 0;
  d->m_HoverOpacity              = 0;
  d->m_HorizontalCardPixmapRatio = 1;
  d->m_VerticalCardPixmapRatio   = 1;
  d->m_CardTitle                 = QStringLiteral("");
  d->m_PromotionTitle            = QStringLiteral("");
  d->m_Title                     = QStringLiteral("");
  d->m_SubTitle                  = QStringLiteral("");
  d->m_CardTitleColor            = Qt::white;
  d->m_PromotionTitleColor       = Qt::white;
  d->m_TitleColor                = Qt::white;
  d->m_SubTitleColor             = Qt::white;
  d->m_PromotionTitleBaseColor   = QColor(0, 0, 0, 120);
  d->m_CardTitlePixelSize        = 22;
  d->m_PromotionTitlePixelSize   = 12;
  d->m_TitlePixelSize            = 25;
  d->m_SubTitlePixelSize         = 16;
  setMouseTracking(true);
  setObjectName("NXPromotionCard");
  setStyleSheet(QStringLiteral("#NXPromotionCard{background-color:transparent;}"));
  d->m_HoverGradient = new QRadialGradient();
  d->m_HoverGradient->setRadius(170);
  d->m_HoverGradient->setColorAt(0, QColor(0xFF, 0xFF, 0xFF, 40));
  d->m_HoverGradient->setColorAt(1, QColor(0xFF, 0xFF, 0xFF, 0));

  d->m_PressGradient = new QRadialGradient();
  d->m_PressGradient->setRadius(170);
  d->m_PressGradient->setColorAt(0, QColor(0xFF, 0xFF, 0xFF, 0));
  d->m_PressGradient->setColorAt(1, QColor(0xFF, 0xFF, 0xFF, 40));
}

NXPromotionCard::~NXPromotionCard()
{
  Q_CONST_D(NXPromotionCard);
  delete d->m_HoverGradient;
  delete d->m_PressGradient;
}

void NXPromotionCard::setHorizontalCardPixmapRatio(qreal pixmapRatio)
{
  Q_D(NXPromotionCard);
  if (pixmapRatio > 0 && pixmapRatio <= 1) {
    Q_EMIT signalHorizontalCardPixmapRatioChanged();
    d->m_HorizontalCardPixmapRatio = pixmapRatio;
  }
}

qreal NXPromotionCard::getHorizontalCardPixmapRatio() const noexcept(std::is_nothrow_copy_constructible_v<qreal>)
{
  Q_CONST_D(NXPromotionCard);
  return d->m_HorizontalCardPixmapRatio;
}

void NXPromotionCard::setVerticalCardPixmapRatio(qreal pixmapRatio)
{
  Q_D(NXPromotionCard);
  if (pixmapRatio > 0 && pixmapRatio <= 1) {
    Q_EMIT signalVerticalCardPixmapRatioChanged();
    d->m_VerticalCardPixmapRatio = pixmapRatio;
  }
}

qreal NXPromotionCard::getVerticalCardPixmapRatio() const noexcept(std::is_nothrow_copy_constructible_v<qreal>)
{
  Q_CONST_D(NXPromotionCard);
  return d->m_VerticalCardPixmapRatio;
}

bool NXPromotionCard::event(QEvent *event)
{
  Q_D(NXPromotionCard);
  switch (event->type()) {
  case QEvent::MouseButtonPress : {
    QMouseEvent *mouseEvent              = dynamic_cast<QMouseEvent *>(event);
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(d, "PressOpacity");
    connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      update();
    });
    connect(opacityAnimation, &QPropertyAnimation::finished, this, [=]() {
      d->m_IsPressAnimationFinished = true;
    });
    opacityAnimation->setDuration(300);
    opacityAnimation->setEasingCurve(QEasingCurve::InQuad);
    opacityAnimation->setStartValue(1);
    opacityAnimation->setEndValue(0);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *pressAnimation = new QPropertyAnimation(d, "PressRadius");
    connect(pressAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      d->m_PressGradient->setRadius(value.toReal());
    });
    pressAnimation->setDuration(300);
    pressAnimation->setEasingCurve(QEasingCurve::InQuad);
    pressAnimation->setStartValue(30);
    pressAnimation->setEndValue(d->_getLongestDistance(mouseEvent->pos()) * 1.8);
    pressAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    d->m_IsPressAnimationFinished = false;
    d->m_PressGradient->setFocalPoint(mouseEvent->pos());
    d->m_PressGradient->setCenter(mouseEvent->pos());
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
    if (d->m_HoverOpacity < 1 && d->m_IsPressAnimationFinished) {
      d->_startHoverOpacityAnimation(true);
    }
    if (d->m_IsPressAnimationFinished) {
      d->m_HoverGradient->setCenter(mouseEvent->pos());
      d->m_HoverGradient->setFocalPoint(mouseEvent->pos());
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
  nxTheme->drawEffectShadow(&painter, rect(), d->m_ShadowBorderWidth, d->m_BorderRadius);
  QRect foregroundRect(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, width() - 2 * d->m_ShadowBorderWidth, height() - 2 * d->m_ShadowBorderWidth);
  QPainterPath path;
  path.addRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  painter.setClipPath(path);
  //背景图片绘制
  if (!d->m_CardPixmap.isNull()) {
    //源区域计算
    QRect pixSourceRect    = d->m_CardPixmap.rect();
    qreal horizontalOffset = d->m_CardPixmap.width() * (1 - d->m_HorizontalCardPixmapRatio) / 2;
    qreal verticalOffset   = d->m_CardPixmap.height() * (1 - d->m_VerticalCardPixmapRatio) / 2;
    pixSourceRect.adjust(horizontalOffset, verticalOffset, -horizontalOffset, -verticalOffset);
    painter.drawPixmap(foregroundRect, d->m_CardPixmap, pixSourceRect);
  }
  //文字绘制
  painter.save();
  QFont font = painter.font();
  //卡片标题
  font.setWeight(QFont::Bold);
  font.setPixelSize(d->m_CardTitlePixelSize);
  painter.setFont(font);
  painter.setPen(d->m_CardTitleColor);
  painter.drawText(QRect(25, 25, foregroundRect.width() - 25, foregroundRect.height()), Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, d->m_CardTitle);
  //标题
  font.setWeight(QFont::Bold);
  font.setPixelSize(d->m_TitlePixelSize);
  painter.setFont(font);
  painter.setPen(d->m_TitleColor);
  int titleTextHeight = painter.fontMetrics().height();
  QRect titleRect(25, (height() - titleTextHeight) / 2, foregroundRect.width() - 25, titleTextHeight);
  painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignBottom | Qt::TextSingleLine, d->m_Title);
  //推广标题
  if (!d->m_PromotionTitle.isEmpty()) {
    font.setWeight(QFont::Normal);
    font.setPixelSize(d->m_PromotionTitlePixelSize);
    painter.setFont(font);
    int promotionTitleTextWidth  = painter.fontMetrics().horizontalAdvance(d->m_PromotionTitle);
    int promotionTitleTextHeight = painter.fontMetrics().height();
    QRect promotionTitleTextRect(32, titleRect.top() - promotionTitleTextHeight - 5, foregroundRect.width() / 2 - 25, promotionTitleTextHeight);
    //背景绘制
    painter.setPen(Qt::NoPen);
    painter.setBrush(d->m_PromotionTitleBaseColor);
    painter.drawRoundedRect(
        QRect(25, promotionTitleTextRect.top() - 2, promotionTitleTextWidth + 14, promotionTitleTextHeight + 4), 8, 8);
    //文字绘制
    painter.setPen(d->m_PromotionTitleColor);
    painter.drawText(promotionTitleTextRect, Qt::AlignLeft | Qt::AlignBottom | Qt::TextSingleLine, d->m_PromotionTitle);
  }
  //副标题
  font.setWeight(QFont::Medium);
  font.setPixelSize(d->m_SubTitlePixelSize);
  painter.setFont(font);
  painter.setPen(d->m_SubTitleColor);
  painter.drawText(QRect(25, titleRect.bottom(), foregroundRect.width() / 2 - 25, height() / 2), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, d->m_SubTitle);

  painter.restore();
  //效果阴影绘制
  if (d->m_IsPressAnimationFinished) {
    //覆盖阴影绘制
    painter.setOpacity(d->m_HoverOpacity);
    painter.setBrush(*d->m_HoverGradient);
    painter.drawEllipse(d->m_HoverGradient->center(), d->m_HoverGradient->radius(), d->m_HoverGradient->radius());
  } else {
    //点击阴影绘制
    painter.setOpacity(d->m_PressOpacity);
    painter.setBrush(*d->m_PressGradient);
    painter.drawEllipse(d->m_PressGradient->center(), d->m_PressRadius, d->m_PressRadius / 1.1);
  }
  painter.restore();
}
