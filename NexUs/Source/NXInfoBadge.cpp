#include "NXInfoBadge.h"

#include <QEvent>
#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXInfoBadgePrivate.h"

Q_PROPERTY_CREATE_CPP(NXInfoBadge, int, Value)
Q_PROPERTY_CREATE_CPP(NXInfoBadge, NXIconType::IconName, NXIcon)

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
  if (!_target) {
    return;
  }
  // Position at inner top-right corner of target
  int x = _target->width() - q->width() - 2;
  int y = 2;
  q->move(x, y);
}

QColor NXInfoBadgePrivate::_getSeverityColor() const
{
  switch (_severity) {
  case NXInfoBadge::Attention     : return NXThemeColor(_themeMode, StatusDanger);
  case NXInfoBadge::Informational : return NXThemeColor(_themeMode, PrimaryNormal);
  case NXInfoBadge::Success       : return QColor(0x0F, 0x7B, 0x0F);
  case NXInfoBadge::Caution       : return QColor(0x9D, 0x5D, 0x00);
  case NXInfoBadge::Critical      : return NXThemeColor(_themeMode, StatusDanger);
  }
  return NXThemeColor(_themeMode, StatusDanger);
}

NXInfoBadge::NXInfoBadge(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXInfoBadgePrivate())
{
  Q_D(NXInfoBadge);
  d->q_ptr    = this;
  d->_pValue  = 0;
  d->_pNXIcon = NXIconType::None;
  setObjectName("NXInfoBadge");
  setAttribute(Qt::WA_TransparentForMouseEvents);
  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
  connect(this, &NXInfoBadge::pValueChanged, this, [=]() {
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
  d->_badgeMode = Value_;
  d->_pValue    = value;
}

NXInfoBadge::NXInfoBadge(NXIconType::IconName icon, QWidget *parent)
    : NXInfoBadge(parent)
{
  Q_D(NXInfoBadge);
  d->_badgeMode = Icon;
  d->_pNXIcon   = icon;
}

NXInfoBadge::~NXInfoBadge()
{
}

void NXInfoBadge::setBadgeMode(BadgeMode mode)
{
  Q_D(NXInfoBadge);
  d->_badgeMode = mode;
  updateGeometry();
  adjustSize();
  d->_updatePosition();
  update();
}

NXInfoBadge::BadgeMode NXInfoBadge::getBadgeMode() const
{
  return d_ptr->_badgeMode;
}

void NXInfoBadge::setMaxValue(int maxValue)
{
  Q_D(NXInfoBadge);
  d->_maxValue = maxValue;
  update();
}

int NXInfoBadge::getMaxValue() const
{
  return d_ptr->_maxValue;
}

void NXInfoBadge::setSeverity(Severity severity)
{
  Q_D(NXInfoBadge);
  d->_severity = severity;
  update();
}

NXInfoBadge::Severity NXInfoBadge::getSeverity() const
{
  return d_ptr->_severity;
}

void NXInfoBadge::attachTo(QWidget *target)
{
  Q_D(NXInfoBadge);
  if (d->_target) {
    d->_target->removeEventFilter(this);
  }
  d->_target = target;
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
  if (watched == d->_target) {
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

  switch (d->_badgeMode) {
  case Dot : {
    QRectF dotRect(1, 1, width() - 2, height() - 2);
    painter.drawEllipse(dotRect);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(dotRect.adjusted(1, 1, -1, -1));
    break;
  }
  case Value_ : {
    QString text;
    if (d->_pValue > d->_maxValue) {
      text = QString::number(d->_maxValue) + QStringLiteral("+");
    } else {
      text = QString::number(d->_pValue);
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
    painter.drawText(rect(), Qt::AlignCenter, QChar((unsigned short) d->_pNXIcon));
    break;
  }
  }
}

QSize NXInfoBadge::sizeHint() const
{
  Q_D(const NXInfoBadge);
  switch (d->_badgeMode) {
  case Dot    : return QSize(10, 10);
  case Value_ : {
    QString text;
    if (d->_pValue > d->_maxValue) {
      text = QString::number(d->_maxValue) + QStringLiteral("+");
    } else {
      text = QString::number(d->_pValue);
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
