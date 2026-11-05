#include "NXFlowLayoutPrivate.h"

#include <QDebug>
#include <QPropertyAnimation>
#include <QWidget>

#include "NXFlowLayout.h"

NXFlowLayoutPrivate::NXFlowLayoutPrivate(QObject *parent)
    : QObject { parent }
{
}

NXFlowLayoutPrivate::~NXFlowLayoutPrivate()
{
}

int NXFlowLayoutPrivate::_doLayout(QRect rect, bool testOnly) const
{
  Q_Q(NXFlowLayout const);
  int left, top, right, bottom;
  q->getContentsMargins(&left, &top, &right, &bottom);
  QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
  int x               = effectiveRect.x();
  int y               = effectiveRect.y();
  int lineHeight      = 0;
  for (QLayoutItem *item : m_ItemList) {
    QWidget const *wid = item->widget();
    int spaceX         = q->horizontalSpacing();
    if (spaceX == -1) {
      spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
    }
    int spaceY = q->verticalSpacing();
    if (spaceY == -1) {
      spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
    }

    int nextX = x + item->sizeHint().width() + spaceX;
    if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
      x          = effectiveRect.x();
      y          = y + lineHeight + spaceY;
      nextX      = x + item->sizeHint().width() + spaceX;
      lineHeight = 0;
    }
    if (!m_LastGeometryMap.contains(item)) {
      m_LastGeometryMap.insert(item, QPoint(x, y));
    }
    if (!testOnly) {
      if ((item->geometry().x() == 0 && item->geometry().y() == 0)) {
        item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
      } else if (item->geometry().x() != x || item->geometry().y() != y) {
        if (m_IsAnimation) {
          // 阻止多重动画
          if (m_LastGeometryMap[item] == QPoint(x, y)) {
            x          = nextX;
            lineHeight = qMax(lineHeight, item->sizeHint().height());
            continue;
          }
          QPropertyAnimation *geometryAnimation = new QPropertyAnimation(item->widget(), "geometry");
          geometryAnimation->setStartValue(item->widget()->geometry());
          geometryAnimation->setEndValue(QRect(QPoint(x, y), item->sizeHint()));
          geometryAnimation->setDuration(400);
          geometryAnimation->setEasingCurve(QEasingCurve::OutCubic);
          geometryAnimation->start(QAbstractAnimation::DeleteWhenStopped);
          m_LastGeometryMap[item] = QPoint(x, y);
        } else {
          item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        }
      }
    }
    x          = nextX;
    lineHeight = qMax(lineHeight, item->sizeHint().height());
  }
  return y + lineHeight - rect.y() + bottom;
}

int NXFlowLayoutPrivate::_smartSpacing(QStyle::PixelMetric pm) const
{
  Q_Q(NXFlowLayout const);
  QObject *parent = q->parent();
  if (!parent) {
    return -1;
  } else if (parent->isWidgetType()) {
    QWidget *pw = static_cast<QWidget *>(parent);
    return pw->style()->pixelMetric(pm, nullptr, pw);
  } else {
    return static_cast<QLayout *>(parent)->spacing();
  }
}
