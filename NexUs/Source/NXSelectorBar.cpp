#include "NXSelectorBar.h"

#include <QEasingCurve>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVariantAnimation>

#include "NXTheme.h"
#include "private/NXSelectorBarPrivate.h"

Q_PROPERTY_CREATE_CPP(NXSelectorBar, QPS_PAEMIT(int), CurrentIndex)
Q_PROPERTY_CREATE_CPP(NXSelectorBar, QPS_PAEMIT(int), BorderRadius)

NXSelectorBar::NXSelectorBar(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXSelectorBarPrivate())
{
  Q_D(NXSelectorBar);
  d->q_ptr          = this;
  d->m_CurrentIndex = 0;
  d->m_BorderRadius = 4;
  setObjectName("NXSelectorBar");
  setMouseTracking(true);
  setFixedHeight(36);
  setMinimumWidth(120);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXSelectorBar::~NXSelectorBar()
{
}

void NXSelectorBar::addItem(QString const &text)
{
  addItem(NXIconType::None, text);
}

void NXSelectorBar::addItem(NXIconType::IconName icon, QString const &text)
{
  Q_D(NXSelectorBar);
  NXSelectorBarPrivate::SelectorItem item;
  item.icon = icon;
  item.text = text;
  d->m_Items.append(item);

  if (!d->m_Items.isEmpty()) {
    d->m_CurrentIndex  = qBound(0, d->m_CurrentIndex, d->m_Items.count() - 1);
    qreal itemWidth    = static_cast<qreal>(width()) / d->m_Items.count();
    d->m_IndicatorX     = d->m_CurrentIndex * itemWidth;
    d->m_IndicatorWidth = itemWidth;
  }
  updateGeometry();
  update();
}

void NXSelectorBar::clearItems()
{
  Q_D(NXSelectorBar);
  d->m_Items.clear();
  d->m_CurrentIndex  = 0;
  d->m_HoveredIndex   = -1;
  d->m_IndicatorX     = 0;
  d->m_IndicatorWidth = 0;
  updateGeometry();
  update();
}

int NXSelectorBar::getItemCount() const
{
  return d_ptr->m_Items.count();
}

QSize NXSelectorBar::sizeHint() const
{
  int count = d_ptr->m_Items.count();
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
  backgroundPath.addRoundedRect(QRectF(0.5, 0.5, width() - 1, height() - 1), d->m_BorderRadius, d->m_BorderRadius);
  painter.fillPath(backgroundPath, NXThemeColor(d->m_ThemeMode, BasicBase));
  painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
  painter.drawPath(backgroundPath);

  int itemCount = d->m_Items.count();
  if (itemCount == 0) {
    return;
  }

  qreal itemWidth = static_cast<qreal>(width()) / itemCount;

  // Draw items
  for (int i = 0; i < itemCount; ++i) {
    QRectF itemRect(i * itemWidth, 0, itemWidth, height());

    // Hovered background
    if (i == d->m_HoveredIndex && i != d->m_CurrentIndex) {
      QPainterPath hoverPath;
      hoverPath.addRoundedRect(itemRect.adjusted(2, 2, -2, -2), d->m_BorderRadius, d->m_BorderRadius);
      painter.fillPath(hoverPath, NXThemeColor(d->m_ThemeMode, BasicHover));
    }

    NXSelectorBarPrivate::SelectorItem const &item = d->m_Items.at(i);
    bool isSelected                                = (i == d->m_CurrentIndex);

    // Text and icon
    QFont textFont = font();
    textFont.setPixelSize(13);
    if (isSelected) {
      textFont.setBold(true);
    }
    painter.setFont(textFont);
    painter.setPen(isSelected ? NXThemeColor(d->m_ThemeMode, PrimaryNormal) : NXThemeColor(d->m_ThemeMode, BasicText));

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
  if (d->m_IndicatorWidth > 0) {
    qreal indicatorInset = itemWidth * 0.25;
    QRectF indicatorRect(d->m_IndicatorX + indicatorInset, height() - 3, d->m_IndicatorWidth - indicatorInset * 2, 3);
    QPainterPath indicatorPath;
    indicatorPath.addRoundedRect(indicatorRect, 1.5, 1.5);
    painter.fillPath(indicatorPath, NXThemeColor(d->m_ThemeMode, PrimaryNormal));
  }
}

void NXSelectorBar::resizeEvent(QResizeEvent *event)
{
  Q_D(NXSelectorBar);
  if (!d->m_Items.isEmpty()) {
    qreal const itemWidth = static_cast<qreal>(width()) / d->m_Items.count();
    d->m_IndicatorX        = d->m_CurrentIndex * itemWidth;
    d->m_IndicatorWidth    = itemWidth;
  }
  QWidget::resizeEvent(event);
}

void NXSelectorBar::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXSelectorBar);
  if (event->button() == Qt::LeftButton) {
    int index = d->_itemIndexAtPos(event->pos());
    if (index >= 0 && index != d->m_CurrentIndex) {
      d->m_CurrentIndex = index;
      qreal itemWidth   = static_cast<qreal>(width()) / d->m_Items.count();
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
  if (index != d->m_HoveredIndex) {
    d->m_HoveredIndex = index;
    update();
  }
  QWidget::mouseMoveEvent(event);
}

void NXSelectorBar::leaveEvent(QEvent *event)
{
  Q_D(NXSelectorBar);
  d->m_HoveredIndex = -1;
  update();
  QWidget::leaveEvent(event);
}

NXSelectorBarPrivate::NXSelectorBarPrivate(QObject *parent)
    : QObject { parent }
{
  m_IndicatorAnimation = new QVariantAnimation(this);
  m_IndicatorAnimation->setDuration(200);
  m_IndicatorAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

NXSelectorBarPrivate::~NXSelectorBarPrivate()
{
}

void NXSelectorBarPrivate::_startIndicatorAnimation(qreal targetX, qreal targetWidth)
{
  Q_Q(NXSelectorBar);
  m_IndicatorAnimation->stop();
  qreal startX = m_IndicatorX;
  m_IndicatorAnimation->setStartValue(startX);
  m_IndicatorAnimation->setEndValue(targetX);
  QObject::disconnect(m_IndicatorAnimation, nullptr, nullptr, nullptr);
  QObject::connect(m_IndicatorAnimation, &QVariantAnimation::valueChanged, q, [=](QVariant const &value) {
    m_IndicatorX     = value.toReal();
    m_IndicatorWidth = targetWidth;
    q->update();
  });
  m_IndicatorAnimation->start();
}

int NXSelectorBarPrivate::_itemIndexAtPos(QPoint pos) const
{
  Q_Q(NXSelectorBar const);
  int itemCount = m_Items.count();
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
