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

Q_PROPERTY_CREATE_CPP(NXTeachingTip, QS_SET_CREF(QString), Title)
Q_PROPERTY_CREATE_CPP(NXTeachingTip, QS_SET_CREF(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXTeachingTip, QS_SET_CREF(QString), Content)
Q_PROPERTY_CREATE_CPP(NXTeachingTip, QS_SET_CREF(QPixmap), HeroImage)
Q_PROPERTY_CREATE_CPP(NXTeachingTip, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXTeachingTip, NXIconType::IconName, TipIcon)
Q_PROPERTY_CREATE_CPP(NXTeachingTip, bool, IsLightDismiss)

NXTeachingTipPrivate::NXTeachingTipPrivate(QObject *parent)
    : QObject(parent)
{
}

NXTeachingTipPrivate::~NXTeachingTipPrivate()
{
}

QPoint NXTeachingTipPrivate::_calculatePosition() const
{
  Q_Q(const NXTeachingTip);
  if (!_target) {
    return QPoint(0, 0);
  }
  QPoint targetCenter             = _target->mapToGlobal(QPoint(_target->width() / 2, _target->height() / 2));
  NXTeachingTip::TailPosition pos = _tailPosition;
  if (pos == NXTeachingTip::Auto) {
    pos = _calculateAutoTailPosition();
  }

  int gap = _tailSize + 4;
  QPoint result;
  switch (pos) {
  case NXTeachingTip::Bottom :
    // Tip appears above target, tail points down
    result = QPoint(targetCenter.x() - q->width() / 2, _target->mapToGlobal(QPoint(0, 0)).y() - q->height() - gap);
    break;
  case NXTeachingTip::Top :
    // Tip appears below target, tail points up
    result = QPoint(targetCenter.x() - q->width() / 2, _target->mapToGlobal(QPoint(0, _target->height())).y() + gap);
    break;
  case NXTeachingTip::Right :
    // Tip appears left of target, tail points right
    result = QPoint(_target->mapToGlobal(QPoint(0, 0)).x() - q->width() - gap, targetCenter.y() - q->height() / 2);
    break;
  case NXTeachingTip::Left :
    // Tip appears right of target, tail points left
    result = QPoint(_target->mapToGlobal(QPoint(_target->width(), 0)).x() + gap, targetCenter.y() - q->height() / 2);
    break;
  default :
    result = QPoint(targetCenter.x() - q->width() / 2, _target->mapToGlobal(QPoint(0, 0)).y() - q->height() - gap);
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
  Q_Q(const NXTeachingTip);
  if (!_target) {
    return NXTeachingTip::Bottom;
  }
  QPoint targetGlobal = _target->mapToGlobal(QPoint(0, 0));
  QScreen *screen     = QApplication::screenAt(targetGlobal);
  if (!screen) {
    return NXTeachingTip::Bottom;
  }
  QRect screenGeo = screen->availableGeometry();
  int spaceAbove  = targetGlobal.y() - screenGeo.top();
  int spaceBelow  = screenGeo.bottom() - targetGlobal.y() - _target->height();

  // Prefer showing above (tail at bottom)
  if (spaceAbove >= q->sizeHint().height() + _tailSize + 4) {
    return NXTeachingTip::Bottom;
  }
  if (spaceBelow >= q->sizeHint().height() + _tailSize + 4) {
    return NXTeachingTip::Top;
  }
  return NXTeachingTip::Bottom;
}

void NXTeachingTipPrivate::_doShowAnimation()
{
  Q_Q(NXTeachingTip);
  QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(q);
  q->setGraphicsEffect(effect);
  _pOpacity = 0.0;
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
  d->_pBorderRadius   = 8;
  d->_pTitle          = QStringLiteral("");
  d->_pSubTitle       = QStringLiteral("");
  d->_pContent        = QStringLiteral("");
  d->_pTipIcon        = NXIconType::None;
  d->_pIsLightDismiss = true;
  d->_pOpacity        = 1.0;
  setObjectName("NXTeachingTip");
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  setFixedWidth(320);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
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
  d->_tailPosition = position;
}

NXTeachingTip::TailPosition NXTeachingTip::getTailPosition() const
{
  return d_ptr->_tailPosition;
}

void NXTeachingTip::setTarget(QWidget *target)
{
  Q_D(NXTeachingTip);
  if (d->_target) {
    d->_target->removeEventFilter(this);
  }
  d->_target = target;
  if (target) {
    target->installEventFilter(this);
  }
}

QWidget * NXTeachingTip::getTarget() const
{
  return d_ptr->_target;
}

void NXTeachingTip::setCloseButtonVisible(bool visible)
{
  Q_D(NXTeachingTip);
  d->_closeButtonVisible = visible;
  update();
}

void NXTeachingTip::addAction(const QString &text, const std::function<void()> &callback)
{
  Q_D(NXTeachingTip);
  d->_actions.append({ text, callback });
}

void NXTeachingTip::clearActions()
{
  Q_D(NXTeachingTip);
  d->_actions.clear();
}

void NXTeachingTip::showTip()
{
  Q_D(NXTeachingTip);
  if (d->_pIsLightDismiss) {
    qApp->installEventFilter(this);
  }
  // Calculate size based on content
  int contentHeight = d->_shadowBorderWidth * 2;
  if (!d->_pHeroImage.isNull()) {
    QPixmap scaled = d->_pHeroImage.scaledToWidth(width() - d->_shadowBorderWidth * 2, Qt::SmoothTransformation);
    contentHeight += scaled.height();
  }
  contentHeight += 12; // top padding
  if (!d->_pTitle.isEmpty()) {
    contentHeight += 24;
  }
  if (!d->_pSubTitle.isEmpty()) {
    contentHeight += 20;
  }
  if (!d->_pContent.isEmpty()) {
    QFont contentFont = font();
    contentFont.setPixelSize(13);
    QFontMetrics fm(contentFont);
    int textWidth      = width() - d->_shadowBorderWidth * 2 - 30;
    QRect boundingRect = fm.boundingRect(QRect(0, 0, textWidth, 0), Qt::TextWordWrap, d->_pContent);
    contentHeight += boundingRect.height() + 8;
  }
  if (!d->_actions.isEmpty()) {
    contentHeight += 44;
  }
  contentHeight += d->_shadowBorderWidth + 4; // bottom padding
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
  nxTheme->drawEffectShadow(&painter, rect(), d->_shadowBorderWidth, d->_pBorderRadius);
  // Main body
  QRect bodyRect =
      rect().adjusted(d->_shadowBorderWidth, d->_shadowBorderWidth, -d->_shadowBorderWidth, -d->_shadowBorderWidth);
  painter.setPen(QPen(NXThemeColor(d->_themeMode, PopupBorder), 1));
  painter.setBrush(NXThemeColor(d->_themeMode, PopupBase));
  painter.drawRoundedRect(bodyRect, d->_pBorderRadius, d->_pBorderRadius);

  int currentY     = d->_shadowBorderWidth;
  int contentLeft  = d->_shadowBorderWidth + 15;
  int contentWidth = bodyRect.width() - 30;

  // Hero image
  if (!d->_pHeroImage.isNull()) {
    QPixmap scaled = d->_pHeroImage.scaledToWidth(bodyRect.width(), Qt::SmoothTransformation);
    QRect imageRect(d->_shadowBorderWidth, currentY, bodyRect.width(), scaled.height());

    // Clip to rounded top corners
    QPainterPath clipPath;
    clipPath.addRoundedRect(
        QRectF(bodyRect.left(), bodyRect.top(), bodyRect.width(), scaled.height() + d->_pBorderRadius),
        d->_pBorderRadius, d->_pBorderRadius);
    painter.save();
    painter.setClipPath(clipPath);
    painter.drawPixmap(imageRect, scaled);
    painter.restore();

    currentY += scaled.height();
  }

  currentY += 12;

  // Close button
  if (d->_closeButtonVisible) {
    QFont closeFont = QFont(QStringLiteral("NXAwesome"));
    closeFont.setPixelSize(12);
    painter.setFont(closeFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicDetailsText));
    d->_closeButtonRect = QRect(bodyRect.right() - 28, d->_shadowBorderWidth + 8, 20, 20);
    painter.drawText(d->_closeButtonRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Xmark));
  }

  // Icon + Title
  if (!d->_pTitle.isEmpty()) {
    int titleX = contentLeft;
    if (d->_pTipIcon != NXIconType::None) {
      QFont iconFont = QFont(QStringLiteral("NXAwesome"));
      iconFont.setPixelSize(16);
      painter.setFont(iconFont);
      painter.setPen(NXThemeColor(d->_themeMode, PrimaryNormal));
      painter.drawText(QRect(contentLeft, currentY, 20, 22), Qt::AlignVCenter | Qt::AlignLeft, QChar((unsigned short) d->_pTipIcon));
      titleX += 26;
    }
    QFont titleFont = font();
    titleFont.setPixelSize(14);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicText));
    painter.drawText(QRect(titleX, currentY, contentWidth - (titleX - contentLeft) - 25, 22), Qt::AlignVCenter | Qt::AlignLeft, d->_pTitle);
    currentY += 24;
  }

  // SubTitle
  if (!d->_pSubTitle.isEmpty()) {
    QFont subFont = font();
    subFont.setPixelSize(12);
    painter.setFont(subFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicDetailsText));
    painter.drawText(QRect(contentLeft, currentY, contentWidth, 18), Qt::AlignVCenter | Qt::AlignLeft, d->_pSubTitle);
    currentY += 20;
  }

  // Content text (word wrap)
  if (!d->_pContent.isEmpty()) {
    currentY += 4;
    QFont contentFont = font();
    contentFont.setPixelSize(13);
    painter.setFont(contentFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicText));
    QRect textRect(contentLeft, currentY, contentWidth, height() - currentY - d->_shadowBorderWidth);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, d->_pContent);
    QFontMetrics fm(contentFont);
    QRect boundingRect = fm.boundingRect(textRect, Qt::TextWordWrap, d->_pContent);
    currentY += boundingRect.height() + 4;
  }

  // Action buttons area
  if (!d->_actions.isEmpty()) {
    currentY += 8;
    int buttonWidth   = 80;
    int buttonHeight  = 30;
    int buttonSpacing = 8;
    int totalWidth    = d->_actions.size() * buttonWidth + (d->_actions.size() - 1) * buttonSpacing;
    int startX        = bodyRect.right() - 15 - totalWidth;

    QFont buttonFont = font();
    buttonFont.setPixelSize(12);
    painter.setFont(buttonFont);

    for (int i = 0; i < d->_actions.size(); ++i) {
      QRect btnRect(startX + i * (buttonWidth + buttonSpacing), currentY, buttonWidth, buttonHeight);
      if (i == d->_actions.size() - 1) {
        // Primary button (last)
        painter.setPen(Qt::NoPen);
        painter.setBrush(NXThemeColor(d->_themeMode, PrimaryNormal));
        painter.drawRoundedRect(btnRect, 4, 4);
        painter.setPen(Qt::white);
      } else {
        painter.setPen(QPen(NXThemeColor(d->_themeMode, BasicBorder), 1));
        painter.setBrush(NXThemeColor(d->_themeMode, BasicBase));
        painter.drawRoundedRect(btnRect, 4, 4);
        painter.setPen(NXThemeColor(d->_themeMode, BasicText));
      }
      painter.drawText(btnRect, Qt::AlignCenter, d->_actions[i].text);
    }
  }

  // Tail/arrow
  if (d->_target) {
    NXTeachingTip::TailPosition tailPos = d->_tailPosition;
    if (tailPos == NXTeachingTip::Auto) {
      tailPos = d->_calculateAutoTailPosition();
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->_themeMode, PopupBase));

    QPolygonF triangle;
    int ts = d->_tailSize;
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
  if (watched == d->_target) {
    if (event->type() == QEvent::Move || event->type() == QEvent::Resize) {
      if (isVisible()) {
        move(d->_calculatePosition());
      }
    } else if (event->type() == QEvent::Hide || event->type() == QEvent::Destroy) {
      closeTip();
    }
  }
  // Light dismiss: click outside closes the tip
  if (d->_pIsLightDismiss && event->type() == QEvent::MouseButtonPress) {
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
  if (d->_closeButtonVisible && d->_closeButtonRect.contains(event->pos())) {
    Q_EMIT closeButtonClicked();
    closeTip();
    return;
  }
  // Action button hit test
  if (!d->_actions.isEmpty()) {
    QRect bodyRect =
        rect().adjusted(d->_shadowBorderWidth, d->_shadowBorderWidth, -d->_shadowBorderWidth, -d->_shadowBorderWidth);
    int buttonWidth   = 80;
    int buttonHeight  = 30;
    int buttonSpacing = 8;
    int totalWidth    = d->_actions.size() * buttonWidth + (d->_actions.size() - 1) * buttonSpacing;
    int startX        = bodyRect.right() - 15 - totalWidth;

    // Approximate button Y
    int buttonY = height() - d->_shadowBorderWidth - 12 - buttonHeight;

    for (int i = 0; i < d->_actions.size(); ++i) {
      QRect btnRect(startX + i * (buttonWidth + buttonSpacing), buttonY, buttonWidth, buttonHeight);
      if (btnRect.contains(event->pos())) {
        if (d->_actions[i].callback) {
          d->_actions[i].callback();
        }
        return;
      }
    }
  }
  QWidget::mousePressEvent(event);
}
