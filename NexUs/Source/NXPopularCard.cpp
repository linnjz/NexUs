#include "NXPopularCard.h"

#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QTimer>

#include "DeveloperComponents/NXPopularCardFloater.h"
#include "NXApplication.h"
#include "NXPushButton.h"
#include "NXTheme.h"
#include "private/NXPopularCardPrivate.h"
Q_PROPERTY_CREATE_CPP(NXPopularCard, QS_SET_CREF(QPixmap), CardPixmap)
Q_PROPERTY_CREATE_CPP(NXPopularCard, QS_SET_CREF(QPixmap), CardFloatPixmap)
Q_PROPERTY_CREATE_CPP(NXPopularCard, QS_SET_CREF(QString), Title)
Q_PROPERTY_CREATE_CPP(NXPopularCard, QS_SET_CREF(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXPopularCard, QS_SET_CREF(QString), InteractiveTips)
Q_PROPERTY_CREATE_CPP(NXPopularCard, QS_SET_CREF(QString), DetailedText)
Q_PROPERTY_CREATE_CPP(NXPopularCard, int, BorderRadius)

NXPopularCard::NXPopularCard(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXPopularCardPrivate())
{
  Q_D(NXPopularCard);
  setFixedSize(320, 120);
  d->q_ptr          = this;
  d->_pBorderRadius = 8;
  d->_pHoverYOffset = 0;
  d->_pHoverOpacity = 0;
  setObjectName("NXPopularCard");
  setStyleSheet(QStringLiteral("#NXPopularCard{background-color:transparent}"));
  setMouseTracking(true);

  d->_pCardFloatArea = parentWidget();
  d->_floater        = new NXPopularCardFloater(this, d, d->_pCardFloatArea);
  d->_floatTimer     = new QTimer(this);
  connect(d->_floatTimer, &QTimer::timeout, d, &NXPopularCardPrivate::_showFloater);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXPopularCard::~NXPopularCard()
{
}

void NXPopularCard::setCardButtonText(const QString &cardButtonText)
{
  Q_D(NXPopularCard);
  if (cardButtonText.isEmpty()) {
    return;
  }
  d->_pCardButtonText = cardButtonText;
  d->_floater->_overButton->setText(d->_pCardButtonText);
  Q_EMIT pCardButtonTextChanged();
}

QString NXPopularCard::getCardButtonText() const
{
  Q_D(const NXPopularCard);
  return d->_pCardButtonText;
}

void NXPopularCard::setCardFloatArea(QWidget *floatArea)
{
  Q_D(NXPopularCard);
  if (!floatArea || floatArea == this) {
    return;
  }
  d->_pCardFloatArea = floatArea;
  d->_floater->setParent(floatArea);
  Q_EMIT pCardFloatAreaChanged();
}

QWidget * NXPopularCard::getCardFloatArea() const
{
  Q_D(const NXPopularCard);
  return d->_pCardFloatArea;
}

bool NXPopularCard::event(QEvent *event)
{
  Q_D(NXPopularCard);
  switch (event->type()) {
  case QEvent::Enter : {
    d->_floatTimer->start(450);
    QPropertyAnimation *hoverAnimation = new QPropertyAnimation(d, "pHoverYOffset");
    connect(hoverAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      update();
    });
    hoverAnimation->setDuration(130);
    hoverAnimation->setStartValue(d->_pHoverYOffset);
    hoverAnimation->setEndValue(6);
    hoverAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(d, "pHoverOpacity");
    opacityAnimation->setDuration(130);
    opacityAnimation->setStartValue(d->_pHoverOpacity);
    opacityAnimation->setEndValue(1);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    break;
  }
  case QEvent::Leave : {
    d->_floatTimer->stop();
    QPropertyAnimation *hoverAnimation = new QPropertyAnimation(d, "pHoverYOffset");
    connect(hoverAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      update();
    });
    hoverAnimation->setDuration(130);
    hoverAnimation->setStartValue(d->_pHoverYOffset);
    hoverAnimation->setEndValue(0);
    hoverAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(d, "pHoverOpacity");
    opacityAnimation->setDuration(130);
    opacityAnimation->setStartValue(d->_pHoverOpacity);
    opacityAnimation->setEndValue(0);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    break;
  }
  case QEvent::MouseButtonRelease : {
    Q_EMIT popularCardClicked();
    break;
  }
  default : {
    break;
  }
  }
  return QWidget::event(event);
}

void NXPopularCard::paintEvent(QPaintEvent *event)
{
  int fontPixelSize = nxApp->getFontPixelSize();
  Q_D(NXPopularCard);
  if (d->_isFloating) {
    return;
  }
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  if (underMouse()) {
    // 阴影绘制
    painter.setOpacity(d->_pHoverOpacity);
    QRect shadowRect = rect().adjusted(0, -d->_pHoverYOffset, 0, -d->_pHoverYOffset);
    nxTheme->drawEffectShadow(&painter, shadowRect, d->_shadowBorderWidth, d->_pBorderRadius);
  }
  QRectF foregroundRect(d->_shadowBorderWidth, d->_shadowBorderWidth - d->_pHoverYOffset + 1, width() - 2 * d->_shadowBorderWidth, height() - 2 * d->_shadowBorderWidth);
  //背景绘制
  painter.setOpacity(1);
  painter.setPen(underMouse() ? NXThemeColor(d->_themeMode, PopupBorderHover) : NXThemeColor(d->_themeMode, BasicBorder));
  painter.setBrush(NXThemeColor(d->_themeMode, BasicBaseAlpha));
  painter.drawRoundedRect(foregroundRect, d->_pBorderRadius, d->_pBorderRadius);
  //图片绘制
  painter.save();
  QRectF pixRect(foregroundRect.x() + foregroundRect.height() * 0.15, foregroundRect.y() + foregroundRect.height() * 0.15, foregroundRect.height() * 0.7, foregroundRect.height() * 0.7);
  QPainterPath pixPath;
  pixPath.addRoundedRect(pixRect, 4, 4);
  painter.setClipPath(pixPath);
  painter.drawPixmap(pixRect, d->_pCardPixmap, d->_pCardPixmap.rect());
  painter.restore();

  //文字绘制
  //计算按钮最终矩形
  int buttonTargetWidth =
      d->_floater->_overButton->fontMetrics().horizontalAdvance(d->_floater->_overButton->text()) + 40;
  if (buttonTargetWidth > width() * 0.25) {
    buttonTargetWidth = width() * 0.25;
  }
  //处理NXPushButton的阴影偏移
  d->_buttonTargetRect = QRect(QPoint(width() + 2 * d->_floater->_floatGeometryOffset - d->_shadowBorderWidth + 3 - foregroundRect.height() * 0.15 - buttonTargetWidth, foregroundRect.height() * 0.15 - 3), QSize(buttonTargetWidth, 36));

  // Title
  painter.setPen(NXThemeColor(d->_themeMode, BasicText));
  QFont font = painter.font();
  font.setWeight(QFont::Bold);
  font.setPixelSize(fontPixelSize + 2);
  painter.setFont(font);
  int titleHeight = painter.fontMetrics().height();
  QRectF titleRect(pixRect.right() + d->_textHSpacing, pixRect.y(), d->_floater->_floatGeometryOffset * 2 + foregroundRect.width() - pixRect.width() - d->_textHSpacing * 2 - foregroundRect.height() * 0.15 - buttonTargetWidth, titleHeight);
  QString titleText = painter.fontMetrics().elidedText(d->_pTitle, Qt::ElideRight, titleRect.width());
  painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, titleText);

  // SubTitle
  font.setWeight(QFont::DemiBold);
  font.setPixelSize(fontPixelSize);
  painter.setFont(font);
  int subTitleHeight = painter.fontMetrics().height();
  QRectF subTitleRect(pixRect.right() + d->_textHSpacing, titleRect.bottom() + d->_textVSpacing, d->_floater->_floatGeometryOffset * 2 + foregroundRect.width() - pixRect.width() - d->_textHSpacing * 2 - foregroundRect.height() * 0.15 - buttonTargetWidth, subTitleHeight);
  QString subTitleText = painter.fontMetrics().elidedText(d->_pSubTitle, Qt::ElideRight, subTitleRect.width());
  painter.drawText(subTitleRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, subTitleText);

  // InteractiveTips
  int tipWidth  = painter.fontMetrics().horizontalAdvance(d->_pInteractiveTips);
  int tipHeight = painter.fontMetrics().height();
  if (!d->_pInteractiveTips.isEmpty()) {
    font.setWeight(QFont::DemiBold);
    font.setPixelSize(fontPixelSize - 1);
    painter.setFont(font);
    //覆盖背景绘制
    QRectF tipRect(foregroundRect.right() - d->_textHSpacing - tipWidth, foregroundRect.bottom() - d->_textHSpacing - tipHeight, foregroundRect.width() / 2 - d->_textHSpacing, tipHeight);
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->_themeMode, BasicBaseDeep));
    QRectF baseRect = tipRect;
    baseRect.setRight(tipRect.x() + tipWidth);
    baseRect.adjust(-7, -3, 4, 3);
    d->_interactiveTipsBaseRect = baseRect;
    painter.drawRoundedRect(baseRect, 6, 6);
    //文字绘制
    painter.setPen(NXThemeColor(d->_themeMode, BasicText));
    painter.drawText(tipRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, d->_pInteractiveTips);
  } else {
    QRectF tipRect(foregroundRect.right() - d->_textHSpacing - 50, foregroundRect.bottom() - d->_textHSpacing - tipHeight, foregroundRect.width() / 2 - d->_textHSpacing, tipHeight);
    tipRect.setRight(tipRect.x() + tipWidth);
    tipRect.adjust(-7, -3, 4, 3);
    d->_interactiveTipsBaseRect = tipRect;
  }
  painter.restore();
}
