#include "NXPromotionView.h"

#include <QPainter>
#include <QResizeEvent>
#include <QTimer>
#include <QWheelEvent>

#include "NXPromotionCard.h"
#include "NXTheme.h"
#include "private/NXPromotionViewPrivate.h"

NXPromotionView::NXPromotionView(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXPromotionViewPrivate())
{
  Q_D(NXPromotionView);
  d->q_ptr                = this;
  d->_pCurrentIndex       = 0;
  d->_pCardExpandWidth    = 600;
  d->_pCardCollapseWidth  = 300;
  d->_pIsAutoScroll       = false;
  d->_pAutoScrollInterval = 5000;
  setMinimumHeight(200);
  resize(width(), 300);
  setObjectName("NXPromotionView");
  setStyleSheet(QStringLiteral("#NXPromotionView{background-color:transparent;}"));

  d->_autoScrollTimer = new QTimer(this);
  connect(d->_autoScrollTimer, &QTimer::timeout, this, [=]() {
    if (isVisible() && d->_promotionCardList.count() > 2) {
      d->onPromotionCardClicked(d->_promotionCardList[d->_getAdjacentIndex(Qt::LeftToRight, d->_pCurrentIndex)]);
    }
  });

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXPromotionView::~NXPromotionView()
{
}

void NXPromotionView::setCardExpandWidth(int width)
{
  Q_D(NXPromotionView);
  if (width <= 0) {
    return;
  }
  d->_pCardExpandWidth = width;
  d->_updatePromotionCardGeometry();
}

int NXPromotionView::getCardExpandWidth() const
{
  Q_D(const NXPromotionView);
  return d->_pCardExpandWidth;
}

void NXPromotionView::setCardCollapseWidth(int width)
{
  Q_D(NXPromotionView);
  if (width <= 0) {
    return;
  }
  d->_pCardCollapseWidth = width;
  d->_updatePromotionCardGeometry();
}

int NXPromotionView::getCardCollapseWidth() const
{
  Q_D(const NXPromotionView);
  return d->_pCardCollapseWidth;
}

void NXPromotionView::setCurrentIndex(int index)
{
  Q_D(NXPromotionView);
  if (index < 0 || index >= d->_promotionCardList.count()) {
    return;
  }
  d->onPromotionCardClicked(d->_promotionCardList[index]);
}

int NXPromotionView::getCurrentIndex() const
{
  Q_D(const NXPromotionView);
  return d->_pCurrentIndex;
}

void NXPromotionView::setIsAutoScroll(bool isAutoScroll)
{
  Q_D(NXPromotionView);
  if (isAutoScroll) {
    d->_autoScrollTimer->start(d->_pAutoScrollInterval);
  } else {
    d->_autoScrollTimer->stop();
  }
  d->_pIsAutoScroll = isAutoScroll;
  Q_EMIT pIsAutoScrollChanged();
}

bool NXPromotionView::getIsAutoScroll() const
{
  Q_D(const NXPromotionView);
  return d->_pIsAutoScroll;
}

void NXPromotionView::setAutoScrollInterval(int autoScrollInterval)
{
  Q_D(NXPromotionView);
  if (autoScrollInterval < 400) {
    return;
  }
  d->_pAutoScrollInterval = autoScrollInterval;
  Q_EMIT pAutoScrollIntervalChanged();
}

int NXPromotionView::getAutoScrollInterval() const
{
  Q_D(const NXPromotionView);
  return d->_pAutoScrollInterval;
}

void NXPromotionView::appendPromotionCard(NXPromotionCard *card)
{
  Q_D(NXPromotionView);
  if (!card || d->_promotionCardList.contains(card)) {
    return;
  }
  card->setMinimumSize(0, 0);
  card->setMaximumSize(10000, 10000);
  card->setParent(this);
  d->_promotionCardList.append(card);
  connect(card, &NXPromotionCard::promotionCardClicked, this, [=]() {
    d->onPromotionCardClicked(card);
  });
  d->_updatePromotionCardGeometry();
}

void NXPromotionView::wheelEvent(QWheelEvent *event)
{
  Q_D(NXPromotionView);
  if (d->_isAllowSwitch) {
    if (event->angleDelta().y() > 0) {
      //右滑
      d->_isAllowSwitch = false;
      QTimer::singleShot(400, this, [=] {
        d->_isAllowSwitch = true;
      });
      d->onPromotionCardClicked(d->_promotionCardList[d->_getAdjacentIndex(Qt::RightToLeft, d->_pCurrentIndex)]);
    } else {
      //左滑
      d->_isAllowSwitch = false;
      QTimer::singleShot(400, this, [=] {
        d->_isAllowSwitch = true;
      });
      d->onPromotionCardClicked(d->_promotionCardList[d->_getAdjacentIndex(Qt::LeftToRight, d->_pCurrentIndex)]);
    }
  }
  event->accept();
}

void NXPromotionView::resizeEvent(QResizeEvent *event)
{
  Q_D(NXPromotionView);
  if (d->_promotionCardList.count() > 0) {
    int cardCount = d->_promotionCardList.count();

    const int defaultExpandWidth   = 600;
    const int defaultCollapseWidth = 400;
    const int defaultHeight        = 300;

    qreal scale = 1.0;

    if (cardCount >= 3) {
      int visibleCollapseCount = cardCount - 2;
      int originalTotalWidth   = defaultExpandWidth + visibleCollapseCount * (defaultCollapseWidth + d->_cardSpacing);
      int availableWidth       = width() - d->_leftPadding - d->_cardSpacing;
      scale                    = static_cast<qreal>(availableWidth) / originalTotalWidth;
      d->_pCardExpandWidth     = static_cast<int>(defaultExpandWidth * scale);
      d->_pCardCollapseWidth   = static_cast<int>(defaultCollapseWidth * scale);
    } else if (cardCount == 2) {
      int originalTotalWidth = defaultExpandWidth + defaultCollapseWidth + d->_cardSpacing;
      int availableWidth     = width() - d->_leftPadding - d->_cardSpacing;
      scale                  = static_cast<qreal>(availableWidth) / originalTotalWidth;
      d->_pCardExpandWidth   = static_cast<int>(defaultExpandWidth * scale);
      d->_pCardCollapseWidth = static_cast<int>(defaultCollapseWidth * scale);
    } else {
      int totalWidth       = width() - d->_leftPadding - d->_cardSpacing;
      d->_pCardExpandWidth = qMax(300, totalWidth);
      scale                = static_cast<qreal>(d->_pCardExpandWidth) / defaultExpandWidth;
    }

    int newHeight = qMax(200, static_cast<int>(defaultHeight * scale));
    if (height() != newHeight) {
      setFixedHeight(newHeight);
    }

    d->_updatePromotionCardGeometry();
  }
  QWidget::resizeEvent(event);
}

void NXPromotionView::paintEvent(QPaintEvent *event)
{
  Q_D(NXPromotionView);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->_themeMode, BasicIndicator));
  //页标指示器绘制
  int promotionCardCount = d->_promotionCardList.count();
  bool isCountOdd        = promotionCardCount % 2;
  QPoint startPoint =
      isCountOdd ? QPoint(width() / 2 - promotionCardCount / 2 * d->_indicatorSpacing, height() - d->_bottomMargin / 2)
                 : QPoint(width() / 2 - promotionCardCount / 2 * d->_indicatorSpacing - d->_indicatorSpacing / 2, height() - d->_bottomMargin / 2);
  for (int i = 0; i < promotionCardCount; i++) {
    if (i == d->_pCurrentIndex) {
      painter.drawEllipse(QPointF(startPoint.x() + i * d->_indicatorSpacing, startPoint.y()), 3.5, 3.5);
    } else {
      painter.drawEllipse(QPointF(startPoint.x() + i * d->_indicatorSpacing, startPoint.y()), 2.5, 2.5);
    }
  }
  painter.restore();
}
