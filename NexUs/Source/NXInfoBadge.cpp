#include "NXInfoBadge.h"

#include <QEvent>
#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXInfoBadgePrivate.h"

Q_PROPERTY_CREATE_CPP(NXInfoBadge, QPS_PAEMIT(int), Value)
Q_PROPERTY_CREATE_CPP(NXInfoBadge, QPS_PAEMIT(NXIconType::IconName), NXIcon)

NXInfoBadgePrivate::NXInfoBadgePrivate(QObject *parent)
    : QObject(parent)
{
}

NXInfoBadgePrivate::~NXInfoBadgePrivate()
{
}

void NXInfoBadgePrivate::_updatePosition()
{
  Q_Q(NXInfoBadge);
  if (!m_Target) {
    return;
  }
  // Position at inner top-right corner of target
  int x = m_Target->width() - q->width() - 2;
  int y = 2;
  q->move(x, y);
}

QColor NXInfoBadgePrivate::_getSeverityColor() const
{
  switch (m_Severity) {
  case NXInfoBadge::Attention     : return NXThemeColor(m_ThemeMode, StatusDanger);
  case NXInfoBadge::Informational : return NXThemeColor(m_ThemeMode, PrimaryNormal);
  case NXInfoBadge::Success       : return QColor(0x0F, 0x7B, 0x0F);
  case NXInfoBadge::Caution       : return QColor(0x9D, 0x5D, 0x00);
  case NXInfoBadge::Critical      : return NXThemeColor(m_ThemeMode, StatusDanger);
  }
  return NXThemeColor(m_ThemeMode, StatusDanger);
}

NXInfoBadge::NXInfoBadge(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXInfoBadgePrivate())
{
  Q_D(NXInfoBadge);
  d->q_ptr    = this;
  d->m_Value  = 0;
  d->m_NXIcon = NXIconType::None;
  setObjectName("NXInfoBadge");
  setAttribute(Qt::WA_TransparentForMouseEvents);
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
  connect(this, &NXInfoBadge::signalValueChanged, this, [=]() {
    updateGeometry();
    adjustSize();
    d->_updatePosition();
    update();
  });
}

NXInfoBadge::NXInfoBadge(int value, QWidget *parent)
    : NXInfoBadge(parent)
{
  Q_D(NXInfoBadge);
  d->m_BadgeMode = Value_;
  d->m_Value    = value;
}

NXInfoBadge::NXInfoBadge(NXIconType::IconName icon, QWidget *parent)
    : NXInfoBadge(parent)
{
  Q_D(NXInfoBadge);
  d->m_BadgeMode = Icon;
  d->m_NXIcon   = icon;
}

NXInfoBadge::~NXInfoBadge()
{
}

void NXInfoBadge::setBadgeMode(BadgeMode mode)
{
  Q_D(NXInfoBadge);
  d->m_BadgeMode = mode;
  updateGeometry();
  adjustSize();
  d->_updatePosition();
  update();
}

NXInfoBadge::BadgeMode NXInfoBadge::getBadgeMode() const
{
  return d_ptr->m_BadgeMode;
}

void NXInfoBadge::setMaxValue(int maxValue)
{
  Q_D(NXInfoBadge);
  d->m_MaxValue = maxValue;
  update();
}

int NXInfoBadge::getMaxValue() const
{
  return d_ptr->m_MaxValue;
}

void NXInfoBadge::setSeverity(Severity severity)
{
  Q_D(NXInfoBadge);
  d->m_Severity = severity;
  update();
}

NXInfoBadge::Severity NXInfoBadge::getSeverity() const
{
  return d_ptr->m_Severity;
}

void NXInfoBadge::attachTo(QWidget *target)
{
  Q_D(NXInfoBadge);
  if (d->m_Target) {
    d->m_Target->removeEventFilter(this);
  }
  d->m_Target = target;
  if (target) {
    setParent(target);
    target->installEventFilter(this);
    raise();
    adjustSize();
    d->_updatePosition();
    show();
  }
}

bool NXInfoBadge::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXInfoBadge);
  if (watched == d->m_Target) {
    if (event->type() == QEvent::Resize || event->type() == QEvent::Move) {
      d->_updatePosition();
      raise();
    }
  }
  return QWidget::eventFilter(watched, event);
}

void NXInfoBadge::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXInfoBadge);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  QColor badgeColor = d->_getSeverityColor();
  painter.setBrush(badgeColor);

  // White outline for contrast
  painter.setPen(QPen(Qt::white, 2));

  switch (d->m_BadgeMode) {
  case Dot : {
    QRectF dotRect(1, 1, width() - 2, height() - 2);
    painter.drawEllipse(dotRect);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(dotRect.adjusted(1, 1, -1, -1));
    break;
  }
  case Value_ : {
    QString text;
    if (d->m_Value > d->m_MaxValue) {
      text = QString::number(d->m_MaxValue) + QStringLiteral("+");
    } else {
      text = QString::number(d->m_Value);
    }
    QRectF badgeRect(1, 1, width() - 2, height() - 2);
    painter.drawRoundedRect(badgeRect, badgeRect.height() / 2.0, badgeRect.height() / 2.0);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(badgeRect, badgeRect.height() / 2.0, badgeRect.height() / 2.0);
    painter.setPen(Qt::white);
    QFont font = this->font();
    font.setPixelSize(10);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, text);
    break;
  }
  case Icon : {
    QRectF iconRect(1, 1, width() - 2, height() - 2);
    painter.drawEllipse(iconRect);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(iconRect);
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(height() * 0.55);
    painter.setFont(iconFont);
    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignCenter, QChar((unsigned short) d->m_NXIcon));
    break;
  }
  }
}

QSize NXInfoBadge::sizeHint() const
{
  Q_CONST_D(NXInfoBadge);
  switch (d->m_BadgeMode) {
  case Dot    : return QSize(10, 10);
  case Value_ : {
    QString text;
    if (d->m_Value > d->m_MaxValue) {
      text = QString::number(d->m_MaxValue) + QStringLiteral("+");
    } else {
      text = QString::number(d->m_Value);
    }
    QFont font = this->font();
    font.setPixelSize(10);
    font.setBold(true);
    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(text);
    int w         = qMax(16, textWidth + 8);
    return QSize(w, 16);
  }
  case Icon : return QSize(16, 16);
  }
  return QSize(10, 10);
}
