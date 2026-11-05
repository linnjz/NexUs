#include "NXFlyout.h"

#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScreen>
#include <QVBoxLayout>

#include "NXTheme.h"
#include "private/NXFlyoutPrivate.h"
Q_PROPERTY_CREATE_CPP(NXFlyout, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXFlyout, QPS_SCREF_PAEMIT(QString), Content)
Q_PROPERTY_CREATE_CPP(NXFlyout, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXFlyout, QPS_PAEMIT(bool), IsLightDismiss)

NXFlyout::NXFlyout(QWidget *parent)
    : QWidget { nullptr }
    , d_ptr(new NXFlyoutPrivate())
{
  Q_D(NXFlyout);
  d->q_ptr            = this;
  d->m_BorderRadius   = 8;
  d->m_IsLightDismiss = true;
  setObjectName("NXFlyout");
  setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  d->m_MainLayout = new QVBoxLayout(this);
  int margin     = d->m_ShadowBorderWidth + 8;
  d->m_MainLayout->setContentsMargins(margin, margin, margin, margin);

  d->m_OpacityEffect = new QGraphicsOpacityEffect(this);
  d->m_OpacityEffect->setOpacity(0);
  setGraphicsEffect(d->m_OpacityEffect);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
  hide();
}

NXFlyout::~NXFlyout()
{
}

void NXFlyout::setContentWidget(QWidget *widget)
{
  Q_D(NXFlyout);
  if (!widget || widget == this) {
    return;
  }
  if (d->m_ContentWidget) {
    d->m_MainLayout->removeWidget(d->m_ContentWidget);
    d->m_ContentWidget->deleteLater();
  }
  d->m_ContentWidget = widget;
  d->m_MainLayout->addWidget(widget);
}

void NXFlyout::showFlyout(QWidget *target)
{
  Q_D(NXFlyout);
  if (!target) {
    return;
  }
  // Calculate size based on content
  if (!d->m_ContentWidget) {
    setFixedWidth(280);
    int contentHeight = d->m_ShadowBorderWidth * 2 + 12;
    int textWidth     = 280 - d->m_ShadowBorderWidth * 2 - 30;
    if (!d->m_Title.isEmpty()) {
      QFont titleFont = font();
      titleFont.setPixelSize(14);
      titleFont.setBold(true);
      QFontMetrics tfm(titleFont);
      contentHeight += tfm.height() + 6;
    }
    if (!d->m_Content.isEmpty()) {
      QFont contentFont = font();
      contentFont.setPixelSize(13);
      QFontMetrics cfm(contentFont);
      QRect br = cfm.boundingRect(QRect(0, 0, textWidth, 0), Qt::TextWordWrap, d->m_Content);
      contentHeight += br.height() + 8;
    }
    contentHeight += 12;
    setFixedHeight(contentHeight);
  } else {
    setMinimumWidth(0);
    setMaximumWidth(QWIDGETSIZE_MAX);
    setMinimumHeight(0);
    setMaximumHeight(QWIDGETSIZE_MAX);
    adjustSize();
  }
  QPoint targetGlobal = target->mapToGlobal(QPoint(0, target->height()));
  int x               = targetGlobal.x() + (target->width() - width()) / 2;
  int y               = targetGlobal.y() + 5;

  QScreen *screen = QApplication::screenAt(targetGlobal);
  if (screen) {
    QRect screenRect = screen->availableGeometry();
    if (x < screenRect.left()) {
      x = screenRect.left();
    }
    if (x + width() > screenRect.right()) {
      x = screenRect.right() - width();
    }
    if (y + height() > screenRect.bottom()) {
      y = targetGlobal.y() - target->height() - height() - 5;
    }
    if (y < screenRect.top()) {
      y = screenRect.top();
    }
  }
  move(x, y);
  show();
  raise();
  if (d->m_IsLightDismiss) {
    qApp->installEventFilter(d);
  }
  d->_doShowAnimation();
}

void NXFlyout::closeFlyout()
{
  Q_D(NXFlyout);
  qApp->removeEventFilter(d);
  hide();
  Q_EMIT closed();
}

void NXFlyout::paintEvent(QPaintEvent *event)
{
  Q_D(NXFlyout);
  QPainter painter(this);
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing);
  // Shadow
  nxTheme->drawEffectShadow(&painter, rect(), d->m_ShadowBorderWidth, d->m_BorderRadius);
  QRect foregroundRect = rect();
  foregroundRect.adjust(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, -d->m_ShadowBorderWidth, -d->m_ShadowBorderWidth);
  painter.setPen(NXThemeColor(d->m_ThemeMode, PopupBorder));
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PopupBase));
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);

  if (!d->m_ContentWidget) {
    int textX     = foregroundRect.x() + 12;
    int textY     = foregroundRect.y() + 12;
    int textWidth = foregroundRect.width() - 24;

    // Title
    if (!d->m_Title.isEmpty()) {
      QFont titleFont = painter.font();
      titleFont.setPixelSize(14);
      titleFont.setBold(true);
      painter.setFont(titleFont);
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
      QRect titleRect(textX, textY, textWidth, painter.fontMetrics().height());
      painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, d->m_Title);
      textY += titleRect.height() + 6;
    }

    // Content
    if (!d->m_Content.isEmpty()) {
      QFont contentFont = painter.font();
      contentFont.setPixelSize(13);
      contentFont.setBold(false);
      painter.setFont(contentFont);
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
      QRect contentRect(textX, textY, textWidth, foregroundRect.bottom() - textY - 12);
      painter.drawText(contentRect, Qt::AlignLeft | Qt::TextWordWrap, d->m_Content);
    }
  }
  painter.restore();
}

// NXFlyoutPrivate

NXFlyoutPrivate::NXFlyoutPrivate(QObject *parent)
    : QObject(parent)
{
}

NXFlyoutPrivate::~NXFlyoutPrivate()
{
}

bool NXFlyoutPrivate::eventFilter(QObject *watched, QEvent *event)
{
  Q_Q(NXFlyout);
  if (event->type() == QEvent::MouseButtonPress) {
    QWidget *clickedWidget = qobject_cast<QWidget *>(watched);
    if (clickedWidget && !q->isAncestorOf(clickedWidget) && clickedWidget != q) {
      q->closeFlyout();
      return false;
    }
  }
  return QObject::eventFilter(watched, event);
}

void NXFlyoutPrivate::_doShowAnimation()
{
  QPropertyAnimation *animation = new QPropertyAnimation(m_OpacityEffect, "opacity");
  animation->setDuration(250);
  animation->setStartValue(0.0);
  animation->setEndValue(1.0);
  animation->setEasingCurve(QEasingCurve::InOutCubic);
  animation->start(QAbstractAnimation::DeleteWhenStopped);
}
