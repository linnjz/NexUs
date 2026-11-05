#include "NXTeachingTip.h"

#include <QApplication>
#include <QEvent>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScreen>
#include <QVBoxLayout>

#include "NXTheme.h"
#include "private/NXTeachingTipPrivate.h"

Q_PROPERTY_CREATE_CPP(NXTeachingTip, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXTeachingTip, QPS_SCREF_PAEMIT(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXTeachingTip, QPS_SCREF_PAEMIT(QString), Content)
Q_PROPERTY_CREATE_CPP(NXTeachingTip, QPS_SCREF_PAEMIT(QPixmap), HeroImage)
Q_PROPERTY_CREATE_CPP(NXTeachingTip, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXTeachingTip, QPS_PAEMIT(NXIconType::IconName), TipIcon)
Q_PROPERTY_CREATE_CPP(NXTeachingTip, QPS_PAEMIT(bool), IsLightDismiss)

NXTeachingTipPrivate::NXTeachingTipPrivate(QObject *parent)
    : QObject(parent)
{
}

NXTeachingTipPrivate::~NXTeachingTipPrivate()
{
}

QPoint NXTeachingTipPrivate::_calculatePosition() const
{
  Q_Q(NXTeachingTip const);
  if (!m_Target) {
    return QPoint(0, 0);
  }
  QPoint targetCenter             = m_Target->mapToGlobal(QPoint(m_Target->width() / 2, m_Target->height() / 2));
  NXTeachingTip::TailPosition pos = m_TailPosition;
  if (pos == NXTeachingTip::Auto) {
    pos = _calculateAutoTailPosition();
  }

  int gap = m_TailSize + 4;
  QPoint result;
  switch (pos) {
  case NXTeachingTip::Bottom :
    // Tip appears above target, tail points down
    result = QPoint(targetCenter.x() - q->width() / 2, m_Target->mapToGlobal(QPoint(0, 0)).y() - q->height() - gap);
    break;
  case NXTeachingTip::Top :
    // Tip appears below target, tail points up
    result = QPoint(targetCenter.x() - q->width() / 2, m_Target->mapToGlobal(QPoint(0, m_Target->height())).y() + gap);
    break;
  case NXTeachingTip::Right :
    // Tip appears left of target, tail points right
    result = QPoint(m_Target->mapToGlobal(QPoint(0, 0)).x() - q->width() - gap, targetCenter.y() - q->height() / 2);
    break;
  case NXTeachingTip::Left :
    // Tip appears right of target, tail points left
    result = QPoint(m_Target->mapToGlobal(QPoint(m_Target->width(), 0)).x() + gap, targetCenter.y() - q->height() / 2);
    break;
  default :
    result = QPoint(targetCenter.x() - q->width() / 2, m_Target->mapToGlobal(QPoint(0, 0)).y() - q->height() - gap);
    break;
  }

  // Screen boundary clamping
  QScreen *screen = QApplication::screenAt(targetCenter);
  if (screen) {
    QRect screenGeo = screen->availableGeometry();
    result.setX(qBound(screenGeo.left() + 4, result.x(), screenGeo.right() - q->width() - 4));
    result.setY(qBound(screenGeo.top() + 4, result.y(), screenGeo.bottom() - q->height() - 4));
  }
  return result;
}

NXTeachingTip::TailPosition NXTeachingTipPrivate::_calculateAutoTailPosition() const
{
  Q_Q(NXTeachingTip const);
  if (!m_Target) {
    return NXTeachingTip::Bottom;
  }
  QPoint targetGlobal = m_Target->mapToGlobal(QPoint(0, 0));
  QScreen *screen     = QApplication::screenAt(targetGlobal);
  if (!screen) {
    return NXTeachingTip::Bottom;
  }
  QRect screenGeo = screen->availableGeometry();
  int spaceAbove  = targetGlobal.y() - screenGeo.top();
  int spaceBelow  = screenGeo.bottom() - targetGlobal.y() - m_Target->height();

  // Prefer showing above (tail at bottom)
  if (spaceAbove >= q->sizeHint().height() + m_TailSize + 4) {
    return NXTeachingTip::Bottom;
  }
  if (spaceBelow >= q->sizeHint().height() + m_TailSize + 4) {
    return NXTeachingTip::Top;
  }
  return NXTeachingTip::Bottom;
}

void NXTeachingTipPrivate::_doShowAnimation()
{
  Q_Q(NXTeachingTip);
  QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(q);
  q->setGraphicsEffect(effect);
  m_Opacity = 0.0;
  effect->setOpacity(0);

  QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity", q);
  animation->setDuration(200);
  animation->setStartValue(0.0);
  animation->setEndValue(1.0);
  animation->setEasingCurve(QEasingCurve::OutCubic);
  QObject::connect(animation, &QPropertyAnimation::finished, q, [=]() {
    q->setGraphicsEffect(nullptr);
  });
  animation->start(QAbstractAnimation::DeleteWhenStopped);
}

NXTeachingTip::NXTeachingTip(QWidget *parent)
    : QWidget { nullptr }
    , d_ptr(new NXTeachingTipPrivate())
{
  Q_UNUSED(parent);
  Q_D(NXTeachingTip);
  d->q_ptr            = this;
  d->m_BorderRadius   = 8;
  d->m_Title          = QStringLiteral("");
  d->m_SubTitle       = QStringLiteral("");
  d->m_Content        = QStringLiteral("");
  d->m_TipIcon        = NXIconType::None;
  d->m_IsLightDismiss = true;
  d->m_Opacity        = 1.0;
  setObjectName("NXTeachingTip");
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  setFixedWidth(320);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
  hide();
}

NXTeachingTip::~NXTeachingTip()
{
}

void NXTeachingTip::setTailPosition(TailPosition position)
{
  Q_D(NXTeachingTip);
  d->m_TailPosition = position;
}

NXTeachingTip::TailPosition NXTeachingTip::getTailPosition() const
{
  return d_ptr->m_TailPosition;
}

void NXTeachingTip::setTarget(QWidget *target)
{
  Q_D(NXTeachingTip);
  if (d->m_Target) {
    d->m_Target->removeEventFilter(this);
  }
  d->m_Target = target;
  if (target) {
    target->installEventFilter(this);
  }
}

QWidget *NXTeachingTip::getTarget() const
{
  return d_ptr->m_Target;
}

void NXTeachingTip::setCloseButtonVisible(bool visible)
{
  Q_D(NXTeachingTip);
  d->m_CloseButtonVisible = visible;
  update();
}

void NXTeachingTip::addAction(QString const &text, std::function<void()> const &callback)
{
  Q_D(NXTeachingTip);
  d->m_Actions.append({ text, callback });
}

void NXTeachingTip::clearActions()
{
  Q_D(NXTeachingTip);
  d->m_Actions.clear();
}

void NXTeachingTip::showTip()
{
  Q_D(NXTeachingTip);
  if (d->m_IsLightDismiss) {
    qApp->installEventFilter(this);
  }
  // Calculate size based on content
  int contentHeight = d->m_ShadowBorderWidth * 2;
  if (!d->m_HeroImage.isNull()) {
    QPixmap scaled = d->m_HeroImage.scaledToWidth(width() - d->m_ShadowBorderWidth * 2, Qt::SmoothTransformation);
    contentHeight += scaled.height();
  }
  contentHeight += 12; // top padding
  if (!d->m_Title.isEmpty()) {
    contentHeight += 24;
  }
  if (!d->m_SubTitle.isEmpty()) {
    contentHeight += 20;
  }
  if (!d->m_Content.isEmpty()) {
    QFont contentFont = font();
    contentFont.setPixelSize(13);
    QFontMetrics fm(contentFont);
    int textWidth      = width() - d->m_ShadowBorderWidth * 2 - 30;
    QRect boundingRect = fm.boundingRect(QRect(0, 0, textWidth, 0), Qt::TextWordWrap, d->m_Content);
    contentHeight += boundingRect.height() + 8;
  }
  if (!d->m_Actions.isEmpty()) {
    contentHeight += 44;
  }
  contentHeight += d->m_ShadowBorderWidth + 4; // bottom padding
  setFixedHeight(contentHeight);

  QPoint pos = d->_calculatePosition();
  move(pos);
  show();
  raise();
  d->_doShowAnimation();
}

void NXTeachingTip::closeTip()
{
  Q_D(NXTeachingTip);
  qApp->removeEventFilter(this);
  hide();
  Q_EMIT closed();
}

void NXTeachingTip::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXTeachingTip);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

  // Shadow
  nxTheme->drawEffectShadow(&painter, rect(), d->m_ShadowBorderWidth, d->m_BorderRadius);
  // Main body
  QRect bodyRect =
      rect().adjusted(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, -d->m_ShadowBorderWidth, -d->m_ShadowBorderWidth);
  painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, PopupBorder), 1));
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PopupBase));
  painter.drawRoundedRect(bodyRect, d->m_BorderRadius, d->m_BorderRadius);

  int currentY     = d->m_ShadowBorderWidth;
  int contentLeft  = d->m_ShadowBorderWidth + 15;
  int contentWidth = bodyRect.width() - 30;

  // Hero image
  if (!d->m_HeroImage.isNull()) {
    QPixmap scaled = d->m_HeroImage.scaledToWidth(bodyRect.width(), Qt::SmoothTransformation);
    QRect imageRect(d->m_ShadowBorderWidth, currentY, bodyRect.width(), scaled.height());

    // Clip to rounded top corners
    QPainterPath clipPath;
    clipPath.addRoundedRect(
        QRectF(bodyRect.left(), bodyRect.top(), bodyRect.width(), scaled.height() + d->m_BorderRadius),
        d->m_BorderRadius, d->m_BorderRadius);
    painter.save();
    painter.setClipPath(clipPath);
    painter.drawPixmap(imageRect, scaled);
    painter.restore();

    currentY += scaled.height();
  }

  currentY += 12;

  // Close button
  if (d->m_CloseButtonVisible) {
    QFont closeFont = QFont(QStringLiteral("NXAwesome"));
    closeFont.setPixelSize(12);
    painter.setFont(closeFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
    d->m_CloseButtonRect = QRect(bodyRect.right() - 28, d->m_ShadowBorderWidth + 8, 20, 20);
    painter.drawText(d->m_CloseButtonRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Xmark));
  }

  // Icon + Title
  if (!d->m_Title.isEmpty()) {
    int titleX = contentLeft;
    if (d->m_TipIcon != NXIconType::None) {
      QFont iconFont = QFont(QStringLiteral("NXAwesome"));
      iconFont.setPixelSize(16);
      painter.setFont(iconFont);
      painter.setPen(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
      painter.drawText(QRect(contentLeft, currentY, 20, 22), Qt::AlignVCenter | Qt::AlignLeft, QChar((unsigned short) d->m_TipIcon));
      titleX += 26;
    }
    QFont titleFont = font();
    titleFont.setPixelSize(14);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    painter.drawText(QRect(titleX, currentY, contentWidth - (titleX - contentLeft) - 25, 22), Qt::AlignVCenter | Qt::AlignLeft, d->m_Title);
    currentY += 24;
  }

  // SubTitle
  if (!d->m_SubTitle.isEmpty()) {
    QFont subFont = font();
    subFont.setPixelSize(12);
    painter.setFont(subFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
    painter.drawText(QRect(contentLeft, currentY, contentWidth, 18), Qt::AlignVCenter | Qt::AlignLeft, d->m_SubTitle);
    currentY += 20;
  }

  // Content text (word wrap)
  if (!d->m_Content.isEmpty()) {
    currentY += 4;
    QFont contentFont = font();
    contentFont.setPixelSize(13);
    painter.setFont(contentFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    QRect textRect(contentLeft, currentY, contentWidth, height() - currentY - d->m_ShadowBorderWidth);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, d->m_Content);
    QFontMetrics fm(contentFont);
    QRect boundingRect = fm.boundingRect(textRect, Qt::TextWordWrap, d->m_Content);
    currentY += boundingRect.height() + 4;
  }

  // Action buttons area
  if (!d->m_Actions.isEmpty()) {
    currentY += 8;
    int buttonWidth   = 80;
    int buttonHeight  = 30;
    int buttonSpacing = 8;
    int totalWidth    = d->m_Actions.size() * buttonWidth + (d->m_Actions.size() - 1) * buttonSpacing;
    int startX        = bodyRect.right() - 15 - totalWidth;

    QFont buttonFont = font();
    buttonFont.setPixelSize(12);
    painter.setFont(buttonFont);

    for (int i = 0; i < d->m_Actions.size(); ++i) {
      QRect btnRect(startX + i * (buttonWidth + buttonSpacing), currentY, buttonWidth, buttonHeight);
      if (i == d->m_Actions.size() - 1) {
        // Primary button (last)
        painter.setPen(Qt::NoPen);
        painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
        painter.drawRoundedRect(btnRect, 4, 4);
        painter.setPen(Qt::white);
      } else {
        painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
        painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBase));
        painter.drawRoundedRect(btnRect, 4, 4);
        painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
      }
      painter.drawText(btnRect, Qt::AlignCenter, d->m_Actions[i].text);
    }
  }

  // Tail/arrow
  if (d->m_Target) {
    NXTeachingTip::TailPosition tailPos = d->m_TailPosition;
    if (tailPos == NXTeachingTip::Auto) {
      tailPos = d->_calculateAutoTailPosition();
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->m_ThemeMode, PopupBase));

    QPolygonF triangle;
    int ts = d->m_TailSize;
    switch (tailPos) {
    case NXTeachingTip::Bottom : {
      // Tail at bottom center
      int cx = width() / 2;
      int by = bodyRect.bottom();
      triangle << QPointF(cx - ts, by) << QPointF(cx + ts, by) << QPointF(cx, by + ts);
      break;
    }
    case NXTeachingTip::Top : {
      int cx = width() / 2;
      int ty = bodyRect.top();
      triangle << QPointF(cx - ts, ty) << QPointF(cx + ts, ty) << QPointF(cx, ty - ts);
      break;
    }
    case NXTeachingTip::Left : {
      int cy = height() / 2;
      int lx = bodyRect.left();
      triangle << QPointF(lx, cy - ts) << QPointF(lx, cy + ts) << QPointF(lx - ts, cy);
      break;
    }
    case NXTeachingTip::Right : {
      int cy = height() / 2;
      int rx = bodyRect.right();
      triangle << QPointF(rx, cy - ts) << QPointF(rx, cy + ts) << QPointF(rx + ts, cy);
      break;
    }
    default : break;
    }
    if (!triangle.isEmpty()) {
      QPainterPath tailPath;
      tailPath.addPolygon(triangle);
      tailPath.closeSubpath();
      painter.drawPath(tailPath);
    }
  }
}

bool NXTeachingTip::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXTeachingTip);
  if (watched == d->m_Target) {
    if (event->type() == QEvent::Move || event->type() == QEvent::Resize) {
      if (isVisible()) {
        move(d->_calculatePosition());
      }
    } else if (event->type() == QEvent::Hide || event->type() == QEvent::Destroy) {
      closeTip();
    }
  }
  // Light dismiss: click outside closes the tip
  if (d->m_IsLightDismiss && event->type() == QEvent::MouseButtonPress) {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPoint point = mouseEvent->globalPosition().toPoint();
#else
    QPoint point = mouseEvent->globalPos();
#endif
    if (watched != this && !this->geometry().contains(this->mapFromGlobal(point))) {
      closeTip();
    }
  }
  return QWidget::eventFilter(watched, event);
}

void NXTeachingTip::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);
}

void NXTeachingTip::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXTeachingTip);
  // Close button hit test
  if (d->m_CloseButtonVisible && d->m_CloseButtonRect.contains(event->pos())) {
    Q_EMIT closeButtonClicked();
    closeTip();
    return;
  }
  // Action button hit test
  if (!d->m_Actions.isEmpty()) {
    QRect bodyRect =
        rect().adjusted(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, -d->m_ShadowBorderWidth, -d->m_ShadowBorderWidth);
    int buttonWidth   = 80;
    int buttonHeight  = 30;
    int buttonSpacing = 8;
    int totalWidth    = d->m_Actions.size() * buttonWidth + (d->m_Actions.size() - 1) * buttonSpacing;
    int startX        = bodyRect.right() - 15 - totalWidth;

    // Approximate button Y
    int buttonY = height() - d->m_ShadowBorderWidth - 12 - buttonHeight;

    for (int i = 0; i < d->m_Actions.size(); ++i) {
      QRect btnRect(startX + i * (buttonWidth + buttonSpacing), buttonY, buttonWidth, buttonHeight);
      if (btnRect.contains(event->pos())) {
        if (d->m_Actions[i].callback) {
          d->m_Actions[i].callback();
        }
        return;
      }
    }
  }
  QWidget::mousePressEvent(event);
}
