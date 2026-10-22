#include "NXInfoBarPrivate.h"

#include <QPropertyAnimation>

#include "NXInfoBar.h"
#include "NXTheme.h"

NXInfoBarPrivate::NXInfoBarPrivate(QObject *parent)
    : QObject(parent)
{
}

NXInfoBarPrivate::~NXInfoBarPrivate()
{
}

QColor NXInfoBarPrivate::_getSeverityColor() const
{
  switch (_pSeverity) {
  case NXInfoBarType::Success       : return QColor(0x0F, 0x7B, 0x0F);
  case NXInfoBarType::Warning       : return QColor(0x9D, 0x5D, 0x00);
  case NXInfoBarType::Error         : return QColor(0xC4, 0x2B, 0x1C);
  case NXInfoBarType::Informational :
  default                           : return NXThemeColor(_themeMode, PrimaryNormal);
  }
}

NXIconType::IconName NXInfoBarPrivate::_getSeverityIcon() const
{
  if (_pInfoBarIcon != NXIconType::None) {
    return _pInfoBarIcon;
  }
  switch (_pSeverity) {
  case NXInfoBarType::Success       : return NXIconType::CircleCheck;
  case NXInfoBarType::Warning       : return NXIconType::TriangleExclamation;
  case NXInfoBarType::Error         : return NXIconType::CircleXmark;
  case NXInfoBarType::Informational :
  default                           : return NXIconType::CircleInfo;
  }
}

void NXInfoBarPrivate::_doCloseAnimation()
{
  Q_Q(NXInfoBar);
  if (_isClosing) {
    return;
  }
  _isClosing = true;

  QPropertyAnimation *heightAnimation = new QPropertyAnimation(q, "maximumHeight", q);
  heightAnimation->setDuration(250);
  heightAnimation->setStartValue(q->height());
  heightAnimation->setEndValue(0);
  heightAnimation->setEasingCurve(QEasingCurve::InCubic);

  QObject::connect(heightAnimation, &QPropertyAnimation::finished, q, [=]() {
    q->hide();
    q->setMaximumHeight(QWIDGETSIZE_MAX);
    Q_EMIT q->closed();
  });
  heightAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}
