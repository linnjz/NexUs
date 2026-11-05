#include "NXPromotionViewPrivate.h"

#include <QPropertyAnimation>

#include "NXPromotionCard.h"
#include "NXPromotionView.h"

NXPromotionViewPrivate::NXPromotionViewPrivate(QObject *parent)
    : QObject { parent }
{
}

NXPromotionViewPrivate::~NXPromotionViewPrivate()
{
}

void NXPromotionViewPrivate::onPromotionCardClicked(NXPromotionCard *clickedCard)
{
  Q_Q(NXPromotionView);
  int newCurrentIndex = m_PromotionCardList.indexOf(clickedCard);
  int oldCurrentIndex = m_CurrentIndex;
  if (newCurrentIndex == oldCurrentIndex) {
    return;
  }
  //卡片移动动画
  bool isRightToLeft = m_PromotionCardList[oldCurrentIndex]->x() < m_PromotionCardList[newCurrentIndex]->x();
  int originIndex    = _getAdjacentIndex(Qt::RightToLeft, newCurrentIndex);
  for (int i = 0; i < m_PromotionCardList.count(); i++) {
    NXPromotionCard *card                 = m_PromotionCardList[originIndex];
    QPropertyAnimation *geometryAnimation = new QPropertyAnimation(card, "geometry");
    QRect geometry                        = card->geometry();
    geometryAnimation->setEasingCurve(QEasingCurve::OutCubic);
    geometryAnimation->setDuration(650);
    geometryAnimation->setStartValue(geometry);
    if (i == 0) {
      //最左侧卡片
      QRect targetGeometry(-m_CardCollapseWidth + m_LeftPadding, 0, m_CardCollapseWidth, q->height() - m_BottomMargin);
      if (m_PromotionCardList.count() > 2) {
        if (!isRightToLeft) {
          geometryAnimation->setKeyValueAt(0.7, QRect(geometry.x() + m_CardCollapseWidth * 0.7 + m_CardSpacing, 0, m_CardCollapseWidth, q->height() - m_BottomMargin));
          geometryAnimation->setKeyValueAt(
              0.71, QRect(-m_CardCollapseWidth, 0, m_CardCollapseWidth, q->height() - m_BottomMargin));
        }
        geometryAnimation->setEndValue(targetGeometry);
      } else {
        if (isRightToLeft) {
          geometryAnimation->setEndValue(targetGeometry);
        } else {
          geometryAnimation->setEndValue(QRect(m_LeftPadding + m_CardExpandWidth + m_CardSpacing * 2, 0, m_CardCollapseWidth, q->height() - m_BottomMargin));
        }
      }
      _startHorizontalCardPixmapRatioAnimation(card, card->getHorizontalCardPixmapRatio(), 0.5);
    } else if (i == 1) {
      //展开的卡片
      geometryAnimation->setEndValue(
          QRect(m_LeftPadding + m_CardSpacing, 0, m_CardExpandWidth, q->height() - m_BottomMargin));
      _startHorizontalCardPixmapRatioAnimation(card, card->getHorizontalCardPixmapRatio(), 1);
    } else {
      QRect targetGeometry(m_LeftPadding + m_CardExpandWidth + m_CardSpacing * i + m_CardCollapseWidth * (i - 2), 0, m_CardCollapseWidth, q->height() - m_BottomMargin);
      if (isRightToLeft && targetGeometry.x() > card->x()) {
        connect(geometryAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
          if (card->geometry().right() <= 0) {
            geometryAnimation->pause();
            qreal timeRatio = geometryAnimation->currentTime() / 650.0;
            geometryAnimation->setKeyValueAt(
                timeRatio,
                QRect(m_PromotionCardList[_getAdjacentIndex(Qt::RightToLeft, originIndex)]->geometry().right() + m_CardCollapseWidth * (1 - timeRatio), 0, m_CardCollapseWidth, q->height() - m_BottomMargin));
            geometryAnimation->setEndValue(targetGeometry);
            geometryAnimation->resume();
          }
        });
        geometryAnimation->setEndValue(
            QRect(-m_CardCollapseWidth * 2 + m_LeftPadding, 0, m_CardCollapseWidth, q->height() - m_BottomMargin));
      } else {
        geometryAnimation->setEndValue(targetGeometry);
      }
      card->setHorizontalCardPixmapRatio(0.5);
    }
    originIndex = _getAdjacentIndex(Qt::LeftToRight, originIndex);
    geometryAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }
  m_CurrentIndex = newCurrentIndex;
  q->update();
  Q_EMIT q->signalCurrentIndexChanged();
}

void NXPromotionViewPrivate::_startCardGeometryAnimation(NXPromotionCard *card, QRect start, QRect end)
{
  QPropertyAnimation *geometryAnimation = new QPropertyAnimation(card, "geometry");
  geometryAnimation->setEasingCurve(QEasingCurve::OutCubic);
  geometryAnimation->setDuration(650);
  geometryAnimation->setStartValue(start);
  geometryAnimation->setEndValue(end);
  geometryAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXPromotionViewPrivate::_startHorizontalCardPixmapRatioAnimation(NXPromotionCard *card, qreal start, qreal end)
{
  QPropertyAnimation *ratioAnimation = new QPropertyAnimation(card, "HorizontalCardPixmapRatio");
  ratioAnimation->setEasingCurve(QEasingCurve::OutCubic);
  ratioAnimation->setDuration(650);
  ratioAnimation->setStartValue(start);
  ratioAnimation->setEndValue(end);
  ratioAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXPromotionViewPrivate::_updatePromotionCardGeometry()
{
  Q_Q(NXPromotionView);
  if (m_PromotionCardList.count() < 3) {
    if (m_PromotionCardList.count() == 1) {
      m_PromotionCardList[0]->setGeometry(m_LeftPadding + m_CardSpacing, 0, m_CardExpandWidth, q->height() - m_BottomMargin);
    } else {
      m_PromotionCardList[0]->setGeometry(m_LeftPadding + m_CardSpacing, 0, m_CardExpandWidth, q->height() - m_BottomMargin);
      QRect lastGeometry = m_PromotionCardList[0]->geometry();
      m_PromotionCardList[1]->setGeometry(
          QRect(lastGeometry.right() + m_CardSpacing, lastGeometry.y(), m_CardCollapseWidth, lastGeometry.height()));
      m_PromotionCardList[1]->setHorizontalCardPixmapRatio(0.5);
    }
  } else {
    for (int i = 0; i < m_PromotionCardList.count(); i++) {
      if (i == 0) {
        //展开的卡片
        m_PromotionCardList[i]->setGeometry(m_LeftPadding + m_CardSpacing, 0, m_CardExpandWidth, q->height() - m_BottomMargin);
        m_PromotionCardList[i]->setHorizontalCardPixmapRatio(1);
      } else if (i == m_PromotionCardList.count() - 1) {
        //最左侧卡片
        m_PromotionCardList[i]->setGeometry(-m_CardCollapseWidth + m_LeftPadding, 0, m_CardCollapseWidth, q->height() - m_BottomMargin);
        m_PromotionCardList[i]->setHorizontalCardPixmapRatio(0.5);
      } else {
        QRect lastGeometry = m_PromotionCardList[i - 1]->geometry();
        m_PromotionCardList[i]->setGeometry(
            QRect(lastGeometry.right() + m_CardSpacing, lastGeometry.y(), m_CardCollapseWidth, lastGeometry.height()));
        m_PromotionCardList[i]->setHorizontalCardPixmapRatio(0.5);
      }
    }
  }
}

int NXPromotionViewPrivate::_getAdjacentIndex(Qt::LayoutDirection direction, int index)
{
  if (direction == Qt::LeftToRight) {
    if (index == m_PromotionCardList.count() - 1) {
      return 0;
    }
    return index + 1;
  } else {
    if (index > 0) {
      return index - 1;
    }
    return m_PromotionCardList.count() - 1;
  }
}

int NXPromotionViewPrivate::_getRightLimitX()
{
  int count = m_PromotionCardList.count();
  return m_LeftPadding + m_CardExpandWidth + m_CardSpacing * (count - 1) + m_CardCollapseWidth * (count - 3);
}
