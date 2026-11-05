#include "NXNotificationCenter.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QWheelEvent>

#include "NXTheme.h"
#include "private/NXNotificationCenterPrivate.h"
Q_PROPERTY_CREATE_CPP(NXNotificationCenter, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXNotificationCenter, QPS_PAEMIT(int), PanelWidth)

NXNotificationCenter::NXNotificationCenter(QWidget *parent)
    : QWidget { nullptr }
    , d_ptr(new NXNotificationCenterPrivate())
{
  Q_D(NXNotificationCenter);
  d->q_ptr          = this;
  d->m_BorderRadius = 8;
  d->m_PanelWidth   = 360;
  setObjectName("NXNotificationCenter");
  setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setFixedWidth(d->m_PanelWidth);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
  hide();
}

NXNotificationCenter::~NXNotificationCenter()
{
}

void NXNotificationCenter::addNotification(NotificationItem const &item)
{
  Q_D(NXNotificationCenter);
  d->m_Items.prepend(item);
  if (d->m_IsVisible) {
    update();
  }
}

void NXNotificationCenter::clearAll()
{
  Q_D(NXNotificationCenter);
  d->m_Items.clear();
  d->m_ScrollOffset = 0;
  if (d->m_IsVisible) {
    update();
  }
}

int NXNotificationCenter::getNotificationCount() const
{
  return d_ptr->m_Items.count();
}

void NXNotificationCenter::showPanel(QWidget *anchor)
{
  Q_D(NXNotificationCenter);
  if (!anchor || d->m_IsVisible) {
    return;
  }
  d->m_AnchorWidget = anchor;
  d->m_ScrollOffset = 0;
  setFixedWidth(d->m_PanelWidth);

  QPoint anchorGlobal = anchor->mapToGlobal(QPoint(0, 0));
  int anchorRight     = anchorGlobal.x() + anchor->width();
  int anchorHeight    = anchor->height();
  setFixedHeight(anchorHeight);

  // Start position: off-screen to the right
  int startX = anchorRight;
  int endX   = anchorRight - d->m_PanelWidth;
  int posY   = anchorGlobal.y();

  move(startX, posY);
  show();
  raise();

  if (d->m_SlideAnimation) {
    d->m_SlideAnimation->stop();
    delete d->m_SlideAnimation;
  }
  d->m_SlideAnimation = new QPropertyAnimation(this, "pos");
  d->m_SlideAnimation->setDuration(250);
  d->m_SlideAnimation->setEasingCurve(QEasingCurve::OutCubic);
  d->m_SlideAnimation->setStartValue(QPoint(startX, posY));
  d->m_SlideAnimation->setEndValue(QPoint(endX, posY));
  d->m_SlideAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  connect(d->m_SlideAnimation, &QPropertyAnimation::finished, this, [d]() {
    d->m_SlideAnimation = nullptr;
  });

  d->m_IsVisible = true;
  qApp->installEventFilter(d);
  Q_EMIT panelVisibilityChanged(true);
}

void NXNotificationCenter::hidePanel()
{
  Q_D(NXNotificationCenter);
  if (!d->m_IsVisible) {
    return;
  }
  qApp->removeEventFilter(d);
  d->m_IsVisible = false;

  if (d->m_AnchorWidget) {
    QPoint anchorGlobal = d->m_AnchorWidget->mapToGlobal(QPoint(0, 0));
    int anchorRight     = anchorGlobal.x() + d->m_AnchorWidget->width();
    int posY            = anchorGlobal.y();

    if (d->m_SlideAnimation) {
      d->m_SlideAnimation->stop();
      delete d->m_SlideAnimation;
    }
    d->m_SlideAnimation = new QPropertyAnimation(this, "pos");
    d->m_SlideAnimation->setDuration(250);
    d->m_SlideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    d->m_SlideAnimation->setStartValue(pos());
    d->m_SlideAnimation->setEndValue(QPoint(anchorRight, posY));
    connect(d->m_SlideAnimation, &QPropertyAnimation::finished, this, [this, d]() {
      hide();
      d->m_SlideAnimation = nullptr;
    });
    d->m_SlideAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  } else {
    hide();
  }
  Q_EMIT panelVisibilityChanged(false);
}

bool NXNotificationCenter::isPanelVisible() const
{
  return d_ptr->m_IsVisible;
}

void NXNotificationCenter::paintEvent(QPaintEvent *event)
{
  Q_D(NXNotificationCenter);
  Q_UNUSED(event);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  // Shadow on left edge
  nxTheme->drawEffectShadow(&painter, rect(), d->m_ShadowBorderWidth, d->m_BorderRadius);

  // Background
  QRect foregroundRect = rect();
  foregroundRect.adjust(d->m_ShadowBorderWidth, 0, 0, 0);
  painter.setPen(NXThemeColor(d->m_ThemeMode, PopupBorder));
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PopupBase));
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);

  int contentLeft  = foregroundRect.x() + 12;
  int contentRight = foregroundRect.right() - 12;
  int contentWidth = contentRight - contentLeft;

  // Header
  int headerY      = foregroundRect.y() + 12;
  QFont headerFont = painter.font();
  headerFont.setPixelSize(16);
  headerFont.setBold(true);
  painter.setFont(headerFont);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  QRect headerTitleRect(contentLeft, headerY, contentWidth / 2, 24);
  painter.drawText(headerTitleRect, Qt::AlignLeft | Qt::AlignVCenter, QString::fromUtf8("\xe9\x80\x9a\xe7\x9f\xa5\xe4\xb8\xad\xe5\xbf\x83"));

  // "清除全部" button
  QFont clearFont = painter.font();
  clearFont.setPixelSize(13);
  clearFont.setBold(false);
  painter.setFont(clearFont);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
  QRect clearButtonRect(contentRight - 80, headerY, 80, 24);
  painter.drawText(clearButtonRect, Qt::AlignRight | Qt::AlignVCenter, QString::fromUtf8("\xe6\xb8\x85\xe9\x99\xa4\xe5\x85\xa8\xe9\x83\xa8"));

  // Separator line
  int separatorY = headerY + 30;
  painter.setPen(NXThemeColor(d->m_ThemeMode, PopupBorder));
  painter.drawLine(contentLeft, separatorY, contentRight, separatorY);

  int cardAreaTop    = separatorY + 8;
  int cardAreaBottom = foregroundRect.bottom() - 8;

  if (d->m_Items.isEmpty()) {
    // No notifications
    QFont emptyFont = painter.font();
    emptyFont.setPixelSize(14);
    emptyFont.setBold(false);
    painter.setFont(emptyFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
    QRect emptyRect(contentLeft, cardAreaTop, contentWidth, cardAreaBottom - cardAreaTop);
    painter.drawText(emptyRect, Qt::AlignCenter, QString::fromUtf8("\xe6\x9a\x82\xe6\x97\xa0\xe9\x80\x9a\xe7\x9f\xa5"));
  } else {
    // Clip to card area
    painter.setClipRect(QRect(foregroundRect.x(), cardAreaTop, foregroundRect.width(), cardAreaBottom - cardAreaTop));

    int cardHeight = 70;
    int cardMargin = 8;
    int currentY   = cardAreaTop - d->m_ScrollOffset;

    for (int i = 0; i < d->m_Items.count(); ++i) {
      NotificationItem const &item = d->m_Items[i];
      QRect cardRect(contentLeft, currentY, contentWidth, cardHeight);

      if (cardRect.bottom() >= cardAreaTop && cardRect.top() <= cardAreaBottom) {
        // Card background
        painter.setPen(Qt::NoPen);
        painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBase));
        QPainterPath cardPath;
        cardPath.addRoundedRect(cardRect, d->m_BorderRadius, d->m_BorderRadius);
        painter.drawPath(cardPath);

        int iconAreaWidth = 30;
        int textLeft      = cardRect.x() + 8;

        // Icon
        if (item.icon != NXIconType::None) {
          QFont iconFont = QFont(QStringLiteral("NXAwesome"));
          iconFont.setPixelSize(18);
          painter.setFont(iconFont);
          painter.setPen(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
          QRect iconRect(textLeft, cardRect.y() + 8, iconAreaWidth, 22);
          painter.drawText(iconRect, Qt::AlignVCenter | Qt::AlignLeft, QChar((unsigned short) item.icon));
          textLeft += iconAreaWidth + 4;
        } else {
          textLeft += 4;
        }

        int textWidth = cardRect.right() - textLeft - 8;

        // Title
        QFont titleFont = this->font();
        titleFont.setPixelSize(14);
        titleFont.setBold(true);
        painter.setFont(titleFont);
        painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
        QRect titleRect(textLeft, cardRect.y() + 6, textWidth, 20);
        painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, painter.fontMetrics().elidedText(item.title, Qt::ElideRight, textWidth));

        // Content
        QFont contentFont = this->font();
        contentFont.setPixelSize(13);
        painter.setFont(contentFont);
        painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
        QRect contentRect(textLeft, cardRect.y() + 26, textWidth, 20);
        painter.drawText(contentRect, Qt::AlignLeft | Qt::AlignVCenter, painter.fontMetrics().elidedText(item.content, Qt::ElideRight, textWidth));

        // Timestamp
        QFont tsFont = this->font();
        tsFont.setPixelSize(12);
        painter.setFont(tsFont);
        painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
        QRect tsRect(textLeft, cardRect.y() + 48, textWidth, 16);
        painter.drawText(tsRect, Qt::AlignRight | Qt::AlignVCenter, item.timestamp);
      }
      currentY += cardHeight + cardMargin;
    }
    painter.setClipping(false);
  }
  painter.restore();
}

void NXNotificationCenter::wheelEvent(QWheelEvent *event)
{
  Q_D(NXNotificationCenter);
  if (d->m_Items.isEmpty()) {
    return;
  }
  int cardHeight  = 70;
  int cardMargin  = 8;
  int totalHeight = d->m_Items.count() * (cardHeight + cardMargin) - cardMargin;

  // Header area height
  int headerHeight  = 50;
  int visibleHeight = height() - headerHeight - 16;

  int maxScroll = qMax(0, totalHeight - visibleHeight);
  d->m_ScrollOffset -= event->angleDelta().y() / 3;
  d->m_ScrollOffset = qBound(0, d->m_ScrollOffset, maxScroll);
  update();
  event->accept();
}

void NXNotificationCenter::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXNotificationCenter);
  if (event->button() != Qt::LeftButton) {
    QWidget::mousePressEvent(event);
    return;
  }

  QPoint pos = event->pos();

  // Check clear all button
  if (d->_hitTestClearAll(pos)) {
    clearAll();
    return;
  }

  // Check notification cards
  int index = d->_hitTestCard(pos);
  if (index >= 0) {
    Q_EMIT notificationClicked(index);
    return;
  }
  QWidget::mousePressEvent(event);
}

// NXNotificationCenterPrivate

NXNotificationCenterPrivate::NXNotificationCenterPrivate(QObject *parent)
    : QObject(parent)
{
}

NXNotificationCenterPrivate::~NXNotificationCenterPrivate()
{
}

bool NXNotificationCenterPrivate::eventFilter(QObject *watched, QEvent *event)
{
  Q_Q(NXNotificationCenter);
  if (event->type() == QEvent::MouseButtonPress) {
    QWidget *clickedWidget = qobject_cast<QWidget *>(watched);
    if (clickedWidget && !q->isAncestorOf(clickedWidget) && clickedWidget != q) {
      q->hidePanel();
      return false;
    }
  }
  return QObject::eventFilter(watched, event);
}

int NXNotificationCenterPrivate::_hitTestCard(QPoint pos) const
{
  Q_Q(NXNotificationCenter const);
  int foregroundLeft = m_ShadowBorderWidth;
  int contentLeft    = foregroundLeft + 12;
  int contentRight   = q->width() - 12;
  int contentWidth   = contentRight - contentLeft;

  int headerY     = 12;
  int separatorY  = headerY + 30;
  int cardAreaTop = separatorY + 8;

  int cardHeight = 70;
  int cardMargin = 8;

  for (int i = 0; i < m_Items.count(); ++i) {
    int currentY = cardAreaTop - m_ScrollOffset + i * (cardHeight + cardMargin);
    QRect cardRect(contentLeft, currentY, contentWidth, cardHeight);
    if (cardRect.contains(pos) && currentY + cardHeight > cardAreaTop) {
      return i;
    }
  }
  return -1;
}

QRect NXNotificationCenterPrivate::_clearAllButtonRect() const
{
  Q_Q(NXNotificationCenter const);
  int foregroundLeft = m_ShadowBorderWidth;
  int contentRight   = q->width() - 12;
  int headerY        = 12;
  return QRect(contentRight - 80, headerY, 80, 24);
}

bool NXNotificationCenterPrivate::_hitTestClearAll(QPoint pos) const
{
  return _clearAllButtonRect().contains(pos);
}
