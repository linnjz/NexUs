#include "NXSelectorBar.h"

#include <QEasingCurve>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVariantAnimation>

#include "NXTheme.h"
#include "private/NXSelectorBarPrivate.h"

Q_PROPERTY_CREATE_CPP(NXSelectorBar, int, CurrentIndex)
Q_PROPERTY_CREATE_CPP(NXSelectorBar, int, BorderRadius)

NXSelectorBar::NXSelectorBar(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXSelectorBarPrivate())
{
  Q_D(NXSelectorBar);
  d->q_ptr          = this;
  d->_pCurrentIndex = 0;
  d->_pBorderRadius = 4;
  setObjectName("NXSelectorBar");
  setMouseTracking(true);
  setFixedHeight(36);
  setMinimumWidth(120);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXSelectorBar::~NXSelectorBar()
{
}

void NXSelectorBar::addItem(const QString &text)
{
  addItem(NXIconType::None, text);
}

void NXSelectorBar::addItem(NXIconType::IconName icon, const QString &text)
{
  Q_D(NXSelectorBar);
  NXSelectorBarPrivate::SelectorItem item;
  item.icon = icon;
  item.text = text;
  d->_items.append(item);

  if (!d->_items.isEmpty()) {
    d->_pCurrentIndex  = qBound(0, d->_pCurrentIndex, d->_items.count() - 1);
    qreal itemWidth    = static_cast<qreal>(width()) / d->_items.count();
    d->_indicatorX     = d->_pCurrentIndex * itemWidth;
    d->_indicatorWidth = itemWidth;
  }
  updateGeometry();
  update();
}

void NXSelectorBar::clearItems()
{
  Q_D(NXSelectorBar);
  d->_items.clear();
  d->_pCurrentIndex  = 0;
  d->_hoveredIndex   = -1;
  d->_indicatorX     = 0;
  d->_indicatorWidth = 0;
  updateGeometry();
  update();
}

int NXSelectorBar::getItemCount() const
{
  return d_ptr->_items.count();
}

QSize NXSelectorBar::sizeHint() const
{
  int count = d_ptr->_items.count();
  int w     = qMax(count * 80, 120);
  return QSize(w, 36);
}

void NXSelectorBar::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  Q_D(NXSelectorBar);

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  // Background
  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(QRectF(0.5, 0.5, width() - 1, height() - 1), d->_pBorderRadius, d->_pBorderRadius);
  painter.fillPath(backgroundPath, NXThemeColor(d->_themeMode, BasicBase));
  painter.setPen(QPen(NXThemeColor(d->_themeMode, BasicBorder), 1));
  painter.drawPath(backgroundPath);

  int itemCount = d->_items.count();
  if (itemCount == 0) {
    return;
  }

  qreal itemWidth = static_cast<qreal>(width()) / itemCount;

  // Draw items
  for (int i = 0; i < itemCount; ++i) {
    QRectF itemRect(i * itemWidth, 0, itemWidth, height());

    // Hovered background
    if (i == d->_hoveredIndex && i != d->_pCurrentIndex) {
      QPainterPath hoverPath;
      hoverPath.addRoundedRect(itemRect.adjusted(2, 2, -2, -2), d->_pBorderRadius, d->_pBorderRadius);
      painter.fillPath(hoverPath, NXThemeColor(d->_themeMode, BasicHover));
    }

    const NXSelectorBarPrivate::SelectorItem &item = d->_items.at(i);
    bool isSelected                                = (i == d->_pCurrentIndex);

    // Text and icon
    QFont textFont = font();
    textFont.setPixelSize(13);
    if (isSelected) {
      textFont.setBold(true);
    }
    painter.setFont(textFont);
    painter.setPen(isSelected ? NXThemeColor(d->_themeMode, PrimaryNormal) : NXThemeColor(d->_themeMode, BasicText));

    if (item.icon != NXIconType::None) {
      QFont iconFont(QStringLiteral("NXAwesome"));
      iconFont.setPixelSize(14);
      QFontMetrics iconFm(iconFont);
      QFontMetrics textFm(textFont);
      int iconWidth  = iconFm.horizontalAdvance(QChar(static_cast<int>(item.icon)));
      int textWidth  = textFm.horizontalAdvance(item.text);
      int totalWidth = iconWidth + 4 + textWidth;
      qreal startX   = itemRect.center().x() - totalWidth / 2.0;

      painter.setFont(iconFont);
      painter.drawText(QPointF(startX, itemRect.center().y() + iconFm.ascent() / 2.0 - 1), QString(QChar(static_cast<int>(item.icon))));

      painter.setFont(textFont);
      painter.drawText(QPointF(startX + iconWidth + 4, itemRect.center().y() + textFm.ascent() / 2.0 - 1), item.text);
    } else {
      painter.drawText(itemRect, Qt::AlignCenter, item.text);
    }
  }

  // Bottom indicator
  if (d->_indicatorWidth > 0) {
    qreal indicatorInset = itemWidth * 0.25;
    QRectF indicatorRect(d->_indicatorX + indicatorInset, height() - 3, d->_indicatorWidth - indicatorInset * 2, 3);
    QPainterPath indicatorPath;
    indicatorPath.addRoundedRect(indicatorRect, 1.5, 1.5);
    painter.fillPath(indicatorPath, NXThemeColor(d->_themeMode, PrimaryNormal));
  }
}

void NXSelectorBar::resizeEvent(QResizeEvent *event)
{
  Q_D(NXSelectorBar);
  if (!d->_items.isEmpty()) {
    const qreal itemWidth = static_cast<qreal>(width()) / d->_items.count();
    d->_indicatorX        = d->_pCurrentIndex * itemWidth;
    d->_indicatorWidth    = itemWidth;
  }
  QWidget::resizeEvent(event);
}

void NXSelectorBar::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXSelectorBar);
  if (event->button() == Qt::LeftButton) {
    int index = d->_itemIndexAtPos(event->pos());
    if (index >= 0 && index != d->_pCurrentIndex) {
      d->_pCurrentIndex = index;
      qreal itemWidth   = static_cast<qreal>(width()) / d->_items.count();
      d->_startIndicatorAnimation(index * itemWidth, itemWidth);
      Q_EMIT currentIndexChanged(index);
      update();
    }
  }
  QWidget::mousePressEvent(event);
}

void NXSelectorBar::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXSelectorBar);
  int index = d->_itemIndexAtPos(event->pos());
  if (index != d->_hoveredIndex) {
    d->_hoveredIndex = index;
    update();
  }
  QWidget::mouseMoveEvent(event);
}

void NXSelectorBar::leaveEvent(QEvent *event)
{
  Q_D(NXSelectorBar);
  d->_hoveredIndex = -1;
  update();
  QWidget::leaveEvent(event);
}

NXSelectorBarPrivate::NXSelectorBarPrivate(QObject *parent)
    : QObject { parent }
{
  _indicatorAnimation = new QVariantAnimation(this);
  _indicatorAnimation->setDuration(200);
  _indicatorAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

NXSelectorBarPrivate::~NXSelectorBarPrivate()
{
}

void NXSelectorBarPrivate::_startIndicatorAnimation(qreal targetX, qreal targetWidth)
{
  Q_Q(NXSelectorBar);
  _indicatorAnimation->stop();
  qreal startX = _indicatorX;
  _indicatorAnimation->setStartValue(startX);
  _indicatorAnimation->setEndValue(targetX);
  QObject::disconnect(_indicatorAnimation, nullptr, nullptr, nullptr);
  QObject::connect(_indicatorAnimation, &QVariantAnimation::valueChanged, q, [=](const QVariant &value) {
    _indicatorX     = value.toReal();
    _indicatorWidth = targetWidth;
    q->update();
  });
  _indicatorAnimation->start();
}

int NXSelectorBarPrivate::_itemIndexAtPos(QPoint pos) const
{
  Q_Q(const NXSelectorBar);
  int itemCount = _items.count();
  if (itemCount == 0) {
    return -1;
  }
  qreal itemWidth = static_cast<qreal>(q->width()) / itemCount;
  int index       = static_cast<int>(pos.x() / itemWidth);
  if (index < 0 || index >= itemCount) {
    return -1;
  }
  return index;
}
