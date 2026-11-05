#include "NXProgressBarStyle.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>

#include "NXTheme.h"

NXProgressBarStyle::NXProgressBarStyle(QStyle *style)
    : m_ThemeMode { nxTheme->getThemeMode() }
{
  setProperty("busyStartValue", 0);
  setProperty("busyEndValue", 0);
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXProgressBarStyle::~NXProgressBarStyle()
{
}

void NXProgressBarStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::CE_ProgressBarLabel : {
    if (QStyleOptionProgressBar const *popt = qstyleoption_cast<QStyleOptionProgressBar const *>(option)) {
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
      painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
      painter->drawText(popt->rect, Qt::AlignCenter, popt->text);
      painter->restore();
    }
    return;
  }
  case QStyle::CE_ProgressBarGroove : {
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(m_ThemeMode, BasicChute));
    painter->drawRoundedRect(option->rect, 3, 3);
    painter->restore();
    return;
  }
  case QStyle::CE_ProgressBarContents : {
    QStyleOptionProgressBar const *popt = qstyleoption_cast<QStyleOptionProgressBar const *>(option);
    if (!popt) {
      break;
    }
    QRect contentRect = popt->rect;
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
    bool const inverted = popt->invertedAppearance;
    bool reverse        = popt->direction == Qt::RightToLeft;
    if (inverted) {
      reverse = !reverse;
    }
    if (popt->minimum == 0 && popt->maximum == 0) {
      int startValue = this->property("busyStartValue").toInt();
      if (startValue < 0) {
        startValue = 0;
      }
      int endValue = this->property("busyEndValue").toInt();
      if (popt->state & QStyle::State_Horizontal) {
        if (endValue > contentRect.width()) {
          endValue = contentRect.width();
        }
        int busyWidth = endValue - startValue;
        if (reverse) {
          painter->translate((contentRect.x() + contentRect.width()) / 2, contentRect.y());
          painter->rotate(180);
          painter->translate(-(contentRect.x() + contentRect.width()) / 2, -contentRect.y() - contentRect.height() * 0.90);
        }
        painter->drawRoundedRect(QRectF(startValue, contentRect.y(), busyWidth, contentRect.height()), 3, 3);
      } else {
        if (endValue > contentRect.height()) {
          endValue = contentRect.height();
        }
        int busyHeight = endValue - startValue;
        painter->drawRoundedRect(
            QRectF(contentRect.x(), contentRect.height() - endValue, contentRect.width(), busyHeight), 3, 3);
      }
    } else {
      qreal ratio = popt->progress / (qreal) (popt->maximum - popt->minimum);
      if (popt->state & QStyle::State_Horizontal) {
        if (reverse) {
          painter->translate((contentRect.x() + contentRect.width()) / 2, contentRect.y());
          painter->rotate(180);
          painter->translate(-(contentRect.x() + contentRect.width()) / 2, -contentRect.y() - contentRect.height() * 0.90);
        }
        painter->drawRoundedRect(
            QRectF(contentRect.x(), contentRect.y(), contentRect.width() * ratio, contentRect.height()), 3, 3);
      } else {
        painter->drawRoundedRect(QRectF(contentRect.x(), contentRect.y() + contentRect.height() * (1 - ratio), contentRect.width(), contentRect.height() * ratio), 3, 3);
      }
    }
    painter->restore();
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawControl(element, option, painter, widget);
}

QRect NXProgressBarStyle::subElementRect(SubElement element, QStyleOption const *option, QWidget const *widget) const
{
  switch (element) {
  case QStyle::SE_ProgressBarLabel : {
    QStyleOptionProgressBar const *popt = qstyleoption_cast<QStyleOptionProgressBar const *>(option);
    if (!popt) {
      break;
    }
    if (popt->minimum == 0 && popt->maximum == 0) {
      return QRect();
    }
    QRect contentRect = popt->rect;
    if (popt->state & QStyle::State_Horizontal) {
      int textWidth = popt->fontMetrics.horizontalAdvance(QStringLiteral(" 100% "));
      return QRect(contentRect.right() - textWidth + 1, contentRect.top(), textWidth, contentRect.height());
    }
    return contentRect;
  }
  case QStyle::SE_ProgressBarGroove :
  case QStyle::SE_ProgressBarContents : {
    QRect textRect                      = subElementRect(SE_ProgressBarLabel, option, widget);
    QStyleOptionProgressBar const *popt = qstyleoption_cast<QStyleOptionProgressBar const *>(option);
    if (!popt) {
      break;
    }
    QRect contentRect = popt->rect;
    int width         = contentRect.width();
    int height        = contentRect.height();
    if (popt->state & QStyle::State_Horizontal) {
      contentRect.setTop(contentRect.top() + height * 0.45);
      contentRect.setBottom(contentRect.bottom() - height * 0.30);
      if (!(popt->minimum == 0 && popt->maximum == 0)) {
        contentRect.setWidth(contentRect.width() - textRect.width());
      }
    } else {
      contentRect.setLeft(contentRect.left() + width * 0.375);
      contentRect.setRight(contentRect.right() - width * 0.375);
    }
    return contentRect;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::subElementRect(element, option, widget);
}
