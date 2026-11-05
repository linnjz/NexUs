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
Q_PROPERTY_CREATE_CPP(NXPopularCard, QPS_SCREF_PAEMIT(QPixmap), CardPixmap)
Q_PROPERTY_CREATE_CPP(NXPopularCard, QPS_SCREF_PAEMIT(QPixmap), CardFloatPixmap)
Q_PROPERTY_CREATE_CPP(NXPopularCard, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXPopularCard, QPS_SCREF_PAEMIT(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXPopularCard, QPS_SCREF_PAEMIT(QString), InteractiveTips)
Q_PROPERTY_CREATE_CPP(NXPopularCard, QPS_SCREF_PAEMIT(QString), DetailedText)
Q_PROPERTY_CREATE_CPP(NXPopularCard, QPS_PAEMIT(int), BorderRadius)

NXPopularCard::NXPopularCard(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXPopularCardPrivate())
{
  Q_D(NXPopularCard);
  setFixedSize(320, 120);
  d->q_ptr          = this;
  d->m_BorderRadius = 8;
  d->m_HoverYOffset = 0;
  d->m_HoverOpacity = 0;
  setObjectName("NXPopularCard");
  setStyleSheet(QStringLiteral("#NXPopularCard{background-color:transparent}"));
  setMouseTracking(true);

  d->m_CardFloatArea = parentWidget();
  d->m_Floater        = new NXPopularCardFloater(this, d, d->m_CardFloatArea);
  d->m_FloatTimer     = new QTimer(this);
  connect(d->m_FloatTimer, &QTimer::timeout, d, &NXPopularCardPrivate::_showFloater);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXPopularCard::~NXPopularCard()
{
}

void NXPopularCard::setCardButtonText(QString const &cardButtonText)
{
  Q_D(NXPopularCard);
  if (cardButtonText.isEmpty()) {
    return;
  }
  d->m_CardButtonText = cardButtonText;
  d->m_Floater->m_OverButton->setText(d->m_CardButtonText);
  Q_EMIT signalCardButtonTextChanged();
}

QString NXPopularCard::getCardButtonText() const noexcept(std::is_nothrow_copy_constructible_v<QString>)
{
  Q_CONST_D(NXPopularCard);
  return d->m_CardButtonText;
}

void NXPopularCard::setCardFloatArea(QWidget *floatArea)
{
  Q_D(NXPopularCard);
  if (!floatArea || floatArea == this) {
    return;
  }
  d->m_CardFloatArea = floatArea;
  d->m_Floater->setParent(floatArea);
  Q_EMIT signalCardFloatAreaChanged();
}

QWidget *NXPopularCard::getCardFloatArea() const noexcept(std::is_nothrow_copy_constructible_v<QWidget *>)
{
  Q_CONST_D(NXPopularCard);
  return d->m_CardFloatArea;
}

bool NXPopularCard::event(QEvent *event)
{
  Q_D(NXPopularCard);
  switch (event->type()) {
  case QEvent::Enter : {
    d->m_FloatTimer->start(450);
    QPropertyAnimation *hoverAnimation = new QPropertyAnimation(d, "HoverYOffset");
    connect(hoverAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      update();
    });
    hoverAnimation->setDuration(130);
    hoverAnimation->setStartValue(d->m_HoverYOffset);
    hoverAnimation->setEndValue(6);
    hoverAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(d, "HoverOpacity");
    opacityAnimation->setDuration(130);
    opacityAnimation->setStartValue(d->m_HoverOpacity);
    opacityAnimation->setEndValue(1);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    break;
  }
  case QEvent::Leave : {
    d->m_FloatTimer->stop();
    QPropertyAnimation *hoverAnimation = new QPropertyAnimation(d, "HoverYOffset");
    connect(hoverAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      update();
    });
    hoverAnimation->setDuration(130);
    hoverAnimation->setStartValue(d->m_HoverYOffset);
    hoverAnimation->setEndValue(0);
    hoverAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(d, "HoverOpacity");
    opacityAnimation->setDuration(130);
    opacityAnimation->setStartValue(d->m_HoverOpacity);
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
  if (d->m_IsFloating) {
    return;
  }
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  if (underMouse()) {
    // 阴影绘制
    painter.setOpacity(d->m_HoverOpacity);
    QRect shadowRect = rect().adjusted(0, -d->m_HoverYOffset, 0, -d->m_HoverYOffset);
    nxTheme->drawEffectShadow(&painter, shadowRect, d->m_ShadowBorderWidth, d->m_BorderRadius);
  }
  QRectF foregroundRect(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth - d->m_HoverYOffset + 1, width() - 2 * d->m_ShadowBorderWidth, height() - 2 * d->m_ShadowBorderWidth);
  //背景绘制
  painter.setOpacity(1);
  painter.setPen(underMouse() ? NXThemeColor(d->m_ThemeMode, PopupBorderHover) : NXThemeColor(d->m_ThemeMode, BasicBorder));
  painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBaseAlpha));
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  //图片绘制
  painter.save();
  QRectF pixRect(foregroundRect.x() + foregroundRect.height() * 0.15, foregroundRect.y() + foregroundRect.height() * 0.15, foregroundRect.height() * 0.7, foregroundRect.height() * 0.7);
  QPainterPath pixPath;
  pixPath.addRoundedRect(pixRect, 4, 4);
  painter.setClipPath(pixPath);
  painter.drawPixmap(pixRect, d->m_CardPixmap, d->m_CardPixmap.rect());
  painter.restore();

  //文字绘制
  //计算按钮最终矩形
  int buttonTargetWidth =
      d->m_Floater->m_OverButton->fontMetrics().horizontalAdvance(d->m_Floater->m_OverButton->text()) + 40;
  if (buttonTargetWidth > width() * 0.25) {
    buttonTargetWidth = width() * 0.25;
  }
  //处理NXPushButton的阴影偏移
  d->m_ButtonTargetRect = QRect(QPoint(width() + 2 * d->m_Floater->m_FloatGeometryOffset - d->m_ShadowBorderWidth + 3 - foregroundRect.height() * 0.15 - buttonTargetWidth, foregroundRect.height() * 0.15 - 3), QSize(buttonTargetWidth, 36));

  // Title
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  QFont font = painter.font();
  font.setWeight(QFont::Bold);
  font.setPixelSize(fontPixelSize + 2);
  painter.setFont(font);
  int titleHeight = painter.fontMetrics().height();
  QRectF titleRect(pixRect.right() + d->m_TextHSpacing, pixRect.y(), d->m_Floater->m_FloatGeometryOffset * 2 + foregroundRect.width() - pixRect.width() - d->m_TextHSpacing * 2 - foregroundRect.height() * 0.15 - buttonTargetWidth, titleHeight);
  QString titleText = painter.fontMetrics().elidedText(d->m_Title, Qt::ElideRight, titleRect.width());
  painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, titleText);

  // SubTitle
  font.setWeight(QFont::DemiBold);
  font.setPixelSize(fontPixelSize);
  painter.setFont(font);
  int subTitleHeight = painter.fontMetrics().height();
  QRectF subTitleRect(pixRect.right() + d->m_TextHSpacing, titleRect.bottom() + d->m_TextVSpacing, d->m_Floater->m_FloatGeometryOffset * 2 + foregroundRect.width() - pixRect.width() - d->m_TextHSpacing * 2 - foregroundRect.height() * 0.15 - buttonTargetWidth, subTitleHeight);
  QString subTitleText = painter.fontMetrics().elidedText(d->m_SubTitle, Qt::ElideRight, subTitleRect.width());
  painter.drawText(subTitleRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, subTitleText);

  // InteractiveTips
  int tipWidth  = painter.fontMetrics().horizontalAdvance(d->m_InteractiveTips);
  int tipHeight = painter.fontMetrics().height();
  if (!d->m_InteractiveTips.isEmpty()) {
    font.setWeight(QFont::DemiBold);
    font.setPixelSize(fontPixelSize - 1);
    painter.setFont(font);
    //覆盖背景绘制
    QRectF tipRect(foregroundRect.right() - d->m_TextHSpacing - tipWidth, foregroundRect.bottom() - d->m_TextHSpacing - tipHeight, foregroundRect.width() / 2 - d->m_TextHSpacing, tipHeight);
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBaseDeep));
    QRectF baseRect = tipRect;
    baseRect.setRight(tipRect.x() + tipWidth);
    baseRect.adjust(-7, -3, 4, 3);
    d->m_InteractiveTipsBaseRect = baseRect;
    painter.drawRoundedRect(baseRect, 6, 6);
    //文字绘制
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    painter.drawText(tipRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, d->m_InteractiveTips);
  } else {
    QRectF tipRect(foregroundRect.right() - d->m_TextHSpacing - 50, foregroundRect.bottom() - d->m_TextHSpacing - tipHeight, foregroundRect.width() / 2 - d->m_TextHSpacing, tipHeight);
    tipRect.setRight(tipRect.x() + tipWidth);
    tipRect.adjust(-7, -3, 4, 3);
    d->m_InteractiveTipsBaseRect = tipRect;
  }
  painter.restore();
}
