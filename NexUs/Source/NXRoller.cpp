#include "NXRoller.h"

#include <utility>

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QWheelEvent>
#include <QtMath>

#include "NXApplication.h"
#include "NXTheme.h"
#include "private/NXRollerPrivate.h"
Q_PROPERTY_CREATE_CPP(NXRoller, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXRoller, QPS_PAEMIT(bool), IsContainer)
Q_PROPERTY_CREATE_CPP(NXRoller, QPS_PAEMIT(bool), IsEnableLoop)

NXRoller::NXRoller(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXRollerPrivate())
{
  Q_D(NXRoller);
  d->q_ptr             = this;
  d->m_ScrollOffset    = 0;
  d->m_ItemHeight      = 35;
  d->m_BorderRadius    = 3;
  d->m_MaxVisibleItems = 5;
  d->m_CurrentIndex    = 0;
  d->m_IsEnableLoop    = true;
  d->m_IsContainer     = false;
  setFixedWidth(90);
  setFixedSize(90, 175);
  setMouseTracking(true);
  setObjectName("NXRoller");
  setStyleSheet(QStringLiteral("#NXRoller{background-color:transparent;}"));
  QFont font = this->font();
  font.setPixelSize(nxApp->getFontPixelSize() + 3);
  setFont(font);

  d->m_ScrollAnimation = new QPropertyAnimation(d, "ScrollOffset");
  connect(d->m_ScrollAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    update();
  });
  connect(d->m_ScrollAnimation, &QPropertyAnimation::finished, this, [=]() {
    while (d->m_ScrollOffset < -d->m_ItemList.size() * d->m_ItemHeight) {
      d->m_ScrollOffset += d->m_ItemList.size() * d->m_ItemHeight;
    }
    while (d->m_ScrollOffset >= d->m_ItemList.size() * d->m_ItemHeight) {
      d->m_ScrollOffset -= d->m_ItemList.size() * d->m_ItemHeight;
    }
    d->m_TargetScrollOffset = d->m_ScrollOffset;
    int currentIndex       = 0;
    if (d->m_ScrollOffset >= 0) {
      currentIndex = d->m_ScrollOffset / d->m_ItemHeight;
    } else if (d->m_ScrollOffset <= -d->m_ItemHeight) {
      currentIndex = d->m_ItemList.count() + d->m_ScrollOffset / d->m_ItemHeight;
    }
    if (currentIndex != d->m_CurrentIndex) {
      d->m_CurrentIndex = currentIndex;
      Q_EMIT signalCurrentIndexChanged();
      Q_EMIT currentDataChanged(getCurrentData());
    }
    update();
  });

  d->m_ScrollAnimation->setDuration(300);
  d->m_ScrollAnimation->setEasingCurve(QEasingCurve::OutCubic);

  d->m_PressSustainTimer = new QTimer(this);
  d->m_PressSustainTimer->setInterval(300);
  connect(d->m_PressSustainTimer, &QTimer::timeout, this, [=]() {
    d->m_RepeatScrollTimer->start();
  });

  d->m_RepeatScrollTimer = new QTimer(this);
  d->m_RepeatScrollTimer->setInterval(50);
  connect(d->m_RepeatScrollTimer, &QTimer::timeout, this, [=]() {
    if (d->m_IsUpArrowPress) {
      d->_scroll(120);
    }
    if (d->m_IsDownArrowPress) {
      d->_scroll(-120);
    }
  });

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXRoller::~NXRoller()
{
}

void NXRoller::setCurrentData(QString const &data)
{
  Q_D(NXRoller);
  if (d->m_ItemList.contains(data)) {
    setCurrentIndex(d->m_ItemList.indexOf(data));
  }
}

QString NXRoller::getCurrentData() const
{
  Q_CONST_D(NXRoller);
  if (d->m_CurrentIndex >= d->m_ItemList.count()) {
    return {};
  }
  return d->m_ItemList[d->m_CurrentIndex];
}

void NXRoller::setItemList(QStringList const &itemList)
{
  Q_D(NXRoller);
  d->m_ItemList = std::move(itemList);
  update();
  Q_EMIT signalItemListChanged();
}

QStringList NXRoller::getItemList() const noexcept(std::is_nothrow_copy_constructible_v<QStringList>)
{
  Q_CONST_D(NXRoller);
  return d->m_ItemList;
}

void NXRoller::setItemHeight(int itemHeight)
{
  Q_D(NXRoller);
  d->m_ItemHeight = itemHeight;
  setFixedHeight(d->m_MaxVisibleItems * d->m_ItemHeight);
  update();
  Q_EMIT signalItemHeightChanged();
}

int NXRoller::getItemHeight() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXRoller);
  return d->m_ItemHeight;
}

void NXRoller::setMaxVisibleItems(int maxVisibleItems)
{
  Q_D(NXRoller);
  d->m_MaxVisibleItems = maxVisibleItems;
  setFixedHeight(d->m_MaxVisibleItems * d->m_ItemHeight);
  update();
  Q_EMIT signalMaxVisibleItemsChanged();
}

int NXRoller::getMaxVisibleItems() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXRoller);
  return d->m_MaxVisibleItems;
}

void NXRoller::setCurrentIndex(int currentIndex)
{
  Q_D(NXRoller);
  if (currentIndex >= d->m_ItemList.count()) {
    return;
  }
  d->m_CurrentIndex      = currentIndex;
  d->m_ScrollOffset      = d->m_ItemHeight * currentIndex;
  d->m_TargetScrollOffset = d->m_ScrollOffset;
  update();
}

int NXRoller::getCurrentIndex() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXRoller);
  return d->m_CurrentIndex;
}

void NXRoller::wheelEvent(QWheelEvent *event)
{
  Q_D(NXRoller);
  d->_scroll(event->angleDelta().y());
  event->accept();
}

void NXRoller::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXRoller);
  if (d->m_IsContainer && d->m_MaxVisibleItems >= 5) {
    auto currentPos = event->pos();
    if (d->m_UpArrowRect.contains(currentPos)) {
      d->m_IsUpArrowPress = true;
      d->_scroll(120);
      d->m_PressSustainTimer->start();
    } else if (d->m_DownArrowRect.contains(currentPos)) {
      d->m_IsDownArrowPress = true;
      d->_scroll(-120);
      d->m_PressSustainTimer->start();
    }
  }
  QWidget::mousePressEvent(event);
}

void NXRoller::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXRoller);
  int centerIndex = d->m_MaxVisibleItems / 2;
  int clickIndex  = event->pos().y() / d->m_ItemHeight;
  int jumpCount   = abs(d->m_MaxVisibleItems / 2 - clickIndex);
  if (d->m_IsUpArrowPress || d->m_IsDownArrowPress) {
    d->m_PressSustainTimer->stop();
    d->m_RepeatScrollTimer->stop();
    d->m_IsUpArrowPress   = false;
    d->m_IsDownArrowPress = false;
    update();
  } else {
    if (clickIndex > centerIndex) {
      for (int i = 0; i < jumpCount; i++) {
        d->_scroll(-120);
      }
    } else if (clickIndex < centerIndex) {
      for (int i = 0; i < jumpCount; i++) {
        d->_scroll(120);
      }
    }
  }
  update();
}

void NXRoller::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXRoller);
  d->m_MousePoint = event->pos();
  update();
  QWidget::mouseMoveEvent(event);
}

void NXRoller::leaveEvent(QEvent *event)
{
  Q_D(NXRoller);
  d->m_MousePoint = QPoint();
  update();
  QWidget::leaveEvent(event);
}

void NXRoller::paintEvent(QPaintEvent *event)
{
  Q_D(NXRoller);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  // 背景绘制
  QRect foregroundRect(d->m_PenBorderWidth, d->m_PenBorderWidth, width() - 2 * d->m_PenBorderWidth, height() - 2 * d->m_PenBorderWidth - 1);
  if (!d->m_IsContainer) {
    painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), d->m_PenBorderWidth));
    painter.setBrush(isEnabled() ? NXThemeColor(d->m_ThemeMode, BasicBase) : NXThemeColor(d->m_ThemeMode, BasicDisable));
    painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  }
  painter.setClipRect(foregroundRect);
  // 中心指示区域绘制
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
  bool isEven = d->m_MaxVisibleItems % 2 == 0;
  if (isEven) {
    painter.drawRoundedRect(QRect(d->m_IndicatorMargin, d->m_ItemHeight * d->m_MaxVisibleItems / 2 + d->m_IndicatorMargin, width() - 2 * d->m_IndicatorMargin, d->m_ItemHeight - 2 * d->m_IndicatorMargin), d->m_BorderRadius, d->m_BorderRadius);
  } else {
    painter.drawRoundedRect(QRect(d->m_IndicatorMargin, (height() - d->m_ItemHeight) / 2 + d->m_IndicatorMargin, width() - 2 * d->m_IndicatorMargin, d->m_ItemHeight - 2 * d->m_IndicatorMargin), d->m_BorderRadius, d->m_BorderRadius);
  }

  int yStart           = -d->m_ItemHeight;
  int yEnd             = height();
  int centerYStart     = isEven ? d->m_ItemHeight * d->m_MaxVisibleItems / 2 : height() / 2 - d->m_ItemHeight;
  int centerYEnd       = isEven ? d->m_ItemHeight * d->m_MaxVisibleItems / 2 + d->m_ItemHeight / 2 : height() / 2;
  int startIndexOffset = d->m_MaxVisibleItems / 2;
  for (int i = 0; i < d->m_ItemList.size(); i++) {
    qreal y = (i + startIndexOffset) * d->m_ItemHeight - d->m_ScrollOffset;
    // 范围控制
    if (d->m_IsEnableLoop) {
      while (y <= yStart) {
        y += d->m_ItemList.size() * d->m_ItemHeight;
      }
      while (y >= yEnd) {
        y -= d->m_ItemList.size() * d->m_ItemHeight;
      }
    }
    // 可见区域绘制
    if (y >= yStart && y <= yEnd) {
      painter.save();
      painter.translate(0, y);
      // 文字绘制
      if (y >= centerYStart && y <= centerYEnd) {
        painter.setPen(NXThemeColor(d->m_ThemeMode, BasicTextInvert));
      } else {
        // 覆盖效果绘制
        if (!d->m_MousePoint.isNull() && d->m_ScrollAnimation->state() == QPropertyAnimation::State::Stopped) {
          QRectF itemRect = QRectF(0, y, width(), d->m_ItemHeight);
          if (itemRect.contains(d->m_MousePoint)) {
            painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicHover));
            painter.drawRoundedRect(QRect(d->m_IndicatorMargin, d->m_IndicatorMargin, width() - 2 * d->m_IndicatorMargin, d->m_ItemHeight - 2 * d->m_IndicatorMargin), d->m_BorderRadius, d->m_BorderRadius);
          }
        }
        painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
      }
      painter.drawText(QRect(0, 0, width(), d->m_ItemHeight), Qt::AlignCenter | Qt::TextSingleLine, d->m_ItemList[i]);
      painter.restore();
    }
  }
  // Container模式指示器绘制
  if (d->m_IsContainer && d->m_MaxVisibleItems >= 5 && underMouse()) {
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->m_ThemeMode, PopupBase));
    // 覆盖文字
    d->m_UpArrowRect   = QRect(foregroundRect.x(), foregroundRect.y(), foregroundRect.width(), d->m_ItemHeight);
    d->m_DownArrowRect = QRect(foregroundRect.x(), foregroundRect.bottom() - d->m_ItemHeight + 2, foregroundRect.width(), d->m_ItemHeight);
    painter.drawRect(d->m_UpArrowRect);
    painter.drawRect(d->m_DownArrowRect);
    painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicText));
    // 上箭头
    int upArrowWidth     = d->m_IsUpArrowPress ? 8 : 10; // 边长
    qreal upAdjacentEdge = upArrowWidth * qCos(30 * M_PI / 180.0);
    QPainterPath upArrowPath;
    upArrowPath.moveTo(d->m_UpArrowRect.center().x(), d->m_UpArrowRect.center().y() - upAdjacentEdge / 2);
    upArrowPath.lineTo(d->m_UpArrowRect.center().x() + upArrowWidth / 2, d->m_UpArrowRect.center().y() + upAdjacentEdge / 2);
    upArrowPath.lineTo(d->m_UpArrowRect.center().x() - upArrowWidth / 2, d->m_UpArrowRect.center().y() + upAdjacentEdge / 2);
    upArrowPath.closeSubpath();
    painter.drawPath(upArrowPath);
    // 下箭头
    int downArrowWidth     = d->m_IsDownArrowPress ? 8 : 10;
    qreal downAdjacentEdge = downArrowWidth * qCos(30 * M_PI / 180.0);
    QPainterPath downArrowPath;
    downArrowPath.moveTo(d->m_DownArrowRect.center().x(), d->m_DownArrowRect.center().y() + downAdjacentEdge / 2);
    downArrowPath.lineTo(d->m_DownArrowRect.center().x() + downArrowWidth / 2, d->m_DownArrowRect.center().y() - downAdjacentEdge / 2);
    downArrowPath.lineTo(d->m_DownArrowRect.center().x() - downArrowWidth / 2, d->m_DownArrowRect.center().y() - downAdjacentEdge / 2);
    downArrowPath.closeSubpath();
    painter.drawPath(downArrowPath);
  }
  painter.restore();
}
