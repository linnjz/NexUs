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
  d->m_CurrentIndex       = 0;
  d->m_CardExpandWidth    = 600;
  d->m_CardCollapseWidth  = 300;
  d->m_IsAutoScroll       = false;
  d->m_AutoScrollInterval = 5000;
  setMinimumHeight(200);
  resize(width(), 300);
  setObjectName("NXPromotionView");
  setStyleSheet(QStringLiteral("#NXPromotionView{background-color:transparent;}"));

  d->m_AutoScrollTimer = new QTimer(this);
  connect(d->m_AutoScrollTimer, &QTimer::timeout, this, [=]() {
    if (isVisible() && d->m_PromotionCardList.count() > 2) {
      d->onPromotionCardClicked(d->m_PromotionCardList[d->_getAdjacentIndex(Qt::LeftToRight, d->m_CurrentIndex)]);
    }
  });

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
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
  d->m_CardExpandWidth = width;
  d->_updatePromotionCardGeometry();
}

int NXPromotionView::getCardExpandWidth() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXPromotionView);
  return d->m_CardExpandWidth;
}

void NXPromotionView::setCardCollapseWidth(int width)
{
  Q_D(NXPromotionView);
  if (width <= 0) {
    return;
  }
  d->m_CardCollapseWidth = width;
  d->_updatePromotionCardGeometry();
}

int NXPromotionView::getCardCollapseWidth() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXPromotionView);
  return d->m_CardCollapseWidth;
}

void NXPromotionView::setCurrentIndex(int index)
{
  Q_D(NXPromotionView);
  if (index < 0 || index >= d->m_PromotionCardList.count()) {
    return;
  }
  d->onPromotionCardClicked(d->m_PromotionCardList[index]);
}

int NXPromotionView::getCurrentIndex() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXPromotionView);
  return d->m_CurrentIndex;
}

void NXPromotionView::setIsAutoScroll(bool isAutoScroll)
{
  Q_D(NXPromotionView);
  if (isAutoScroll) {
    d->m_AutoScrollTimer->start(d->m_AutoScrollInterval);
  } else {
    d->m_AutoScrollTimer->stop();
  }
  d->m_IsAutoScroll = isAutoScroll;
  Q_EMIT signalIsAutoScrollChanged();
}

bool NXPromotionView::getIsAutoScroll() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXPromotionView);
  return d->m_IsAutoScroll;
}

void NXPromotionView::setAutoScrollInterval(int autoScrollInterval)
{
  Q_D(NXPromotionView);
  if (autoScrollInterval < 400) {
    return;
  }
  d->m_AutoScrollInterval = autoScrollInterval;
  Q_EMIT signalAutoScrollIntervalChanged();
}

int NXPromotionView::getAutoScrollInterval() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXPromotionView);
  return d->m_AutoScrollInterval;
}

void NXPromotionView::appendPromotionCard(NXPromotionCard *card)
{
  Q_D(NXPromotionView);
  if (!card || d->m_PromotionCardList.contains(card)) {
    return;
  }
  card->setMinimumSize(0, 0);
  card->setMaximumSize(10000, 10000);
  card->setParent(this);
  d->m_PromotionCardList.append(card);
  connect(card, &NXPromotionCard::promotionCardClicked, this, [=]() {
    d->onPromotionCardClicked(card);
  });
  d->_updatePromotionCardGeometry();
}

void NXPromotionView::wheelEvent(QWheelEvent *event)
{
  Q_D(NXPromotionView);
  if (d->m_IsAllowSwitch) {
    if (event->angleDelta().y() > 0) {
      //右滑
      d->m_IsAllowSwitch = false;
      QTimer::singleShot(400, this, [=] {
        d->m_IsAllowSwitch = true;
      });
      d->onPromotionCardClicked(d->m_PromotionCardList[d->_getAdjacentIndex(Qt::RightToLeft, d->m_CurrentIndex)]);
    } else {
      //左滑
      d->m_IsAllowSwitch = false;
      QTimer::singleShot(400, this, [=] {
        d->m_IsAllowSwitch = true;
      });
      d->onPromotionCardClicked(d->m_PromotionCardList[d->_getAdjacentIndex(Qt::LeftToRight, d->m_CurrentIndex)]);
    }
  }
  event->accept();
}

void NXPromotionView::resizeEvent(QResizeEvent *event)
{
  Q_D(NXPromotionView);
  if (d->m_PromotionCardList.count() > 0) {
    int cardCount = d->m_PromotionCardList.count();

    int const defaultExpandWidth   = 600;
    int const defaultCollapseWidth = 400;
    int const defaultHeight        = 300;

    qreal scale = 1.0;

    if (cardCount >= 3) {
      int visibleCollapseCount = cardCount - 2;
      int originalTotalWidth   = defaultExpandWidth + visibleCollapseCount * (defaultCollapseWidth + d->m_CardSpacing);
      int availableWidth       = width() - d->m_LeftPadding - d->m_CardSpacing;
      scale                    = static_cast<qreal>(availableWidth) / originalTotalWidth;
      d->m_CardExpandWidth     = static_cast<int>(defaultExpandWidth * scale);
      d->m_CardCollapseWidth   = static_cast<int>(defaultCollapseWidth * scale);
    } else if (cardCount == 2) {
      int originalTotalWidth = defaultExpandWidth + defaultCollapseWidth + d->m_CardSpacing;
      int availableWidth     = width() - d->m_LeftPadding - d->m_CardSpacing;
      scale                  = static_cast<qreal>(availableWidth) / originalTotalWidth;
      d->m_CardExpandWidth   = static_cast<int>(defaultExpandWidth * scale);
      d->m_CardCollapseWidth = static_cast<int>(defaultCollapseWidth * scale);
    } else {
      int totalWidth       = width() - d->m_LeftPadding - d->m_CardSpacing;
      d->m_CardExpandWidth = qMax(300, totalWidth);
      scale                = static_cast<qreal>(d->m_CardExpandWidth) / defaultExpandWidth;
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
  painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicIndicator));
  //页标指示器绘制
  int promotionCardCount = d->m_PromotionCardList.count();
  bool isCountOdd        = promotionCardCount % 2;
  QPoint startPoint =
      isCountOdd ? QPoint(width() / 2 - promotionCardCount / 2 * d->m_IndicatorSpacing, height() - d->m_BottomMargin / 2)
                 : QPoint(width() / 2 - promotionCardCount / 2 * d->m_IndicatorSpacing - d->m_IndicatorSpacing / 2, height() - d->m_BottomMargin / 2);
  for (int i = 0; i < promotionCardCount; i++) {
    if (i == d->m_CurrentIndex) {
      painter.drawEllipse(QPointF(startPoint.x() + i * d->m_IndicatorSpacing, startPoint.y()), 3.5, 3.5);
    } else {
      painter.drawEllipse(QPointF(startPoint.x() + i * d->m_IndicatorSpacing, startPoint.y()), 2.5, 2.5);
    }
  }
  painter.restore();
}
