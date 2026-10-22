#include "NXFloatButton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#include "NXMenu.h"
#include "NXTheme.h"
#include "private/NXFloatButtonPrivate.h"

Q_PROPERTY_CREATE_CPP(NXFloatButton, int, ButtonSize)

void NXFloatButton::setMargin(int Margin)
{
  Q_D(NXFloatButton);
  d->_pMargin = Margin;
  d->updatePosition();
  Q_EMIT pMarginChanged();
}

int NXFloatButton::getMargin() const
{
  return d_ptr->_pMargin;
}

NXFloatButton::NXFloatButton(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXFloatButtonPrivate())
{
  Q_D(NXFloatButton);
  d->q_ptr        = this;
  d->_pButtonSize = 56;
  d->_pMargin     = 30;
  d->_pHoverAlpha = 0;
  d->_themeMode   = nxTheme->getThemeMode();

  setFixedSize(d->_pButtonSize, d->_pButtonSize);
  setCursor(Qt::PointingHandCursor);
  setMouseTracking(true);
  raise();

  if (parent) {
    parent->installEventFilter(this);
    d->updatePosition();
  }

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXFloatButton::NXFloatButton(NXIconType::IconName icon, QWidget *parent)
    : NXFloatButton(parent)
{
  Q_D(NXFloatButton);
  d->_icon = icon;
}

NXFloatButton::NXFloatButton(NXIconType::IconName icon, Position position, QWidget *parent)
    : NXFloatButton(parent)
{
  Q_D(NXFloatButton);
  d->_icon     = icon;
  d->_position = (int) position;
  d->updatePosition();
}

NXFloatButton::~NXFloatButton()
{
}

void NXFloatButton::setIcon(NXIconType::IconName icon)
{
  Q_D(NXFloatButton);
  d->_icon = icon;
  update();
}

NXIconType::IconName NXFloatButton::getIcon() const
{
  return d_ptr->_icon;
}

void NXFloatButton::setPosition(Position position)
{
  Q_D(NXFloatButton);
  d->_position = (int) position;
  d->updatePosition();
}

NXFloatButton::Position NXFloatButton::getPosition() const
{
  return (Position) d_ptr->_position;
}

void NXFloatButton::setMenu(NXMenu *menu)
{
  Q_D(NXFloatButton);
  d->_menu = menu;
}

NXMenu * NXFloatButton::getMenu() const
{
  return d_ptr->_menu;
}

void NXFloatButton::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXFloatButton);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  nxTheme->drawEffectShadow(&painter, rect(), 6, width() / 2);

  QRect foregroundRect = rect().adjusted(6, 6, -6, -6);
  int radius           = foregroundRect.width() / 2;

  painter.setPen(Qt::NoPen);
  if (d->_isPressed) {
    painter.setBrush(NXThemeColor(d->_themeMode, PrimaryPress));
  } else if (d->_isAlphaAnimationFinished && underMouse()) {
    painter.setBrush(NXThemeColor(d->_themeMode, PrimaryHover));
  } else {
    QColor baseColor = NXThemeColor(d->_themeMode, PrimaryNormal);
    if (d->_pHoverAlpha > 0) {
      QColor hoverColor = NXThemeColor(d->_themeMode, PrimaryHover);
      qreal t           = d->_pHoverAlpha / 255.0;
      baseColor         = QColor(baseColor.red() + (hoverColor.red() - baseColor.red()) * t, baseColor.green() + (hoverColor.green() - baseColor.green()) * t, baseColor.blue() + (hoverColor.blue() - baseColor.blue()) * t);
    }
    painter.setBrush(baseColor);
  }
  painter.drawEllipse(foregroundRect);

  painter.setPen(Qt::white);
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(22);
  painter.setFont(iconFont);
  painter.drawText(foregroundRect, Qt::AlignCenter, QChar((unsigned short) d->_icon));
}

void NXFloatButton::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXFloatButton);
  if (event->button() == Qt::LeftButton) {
    d->_isPressed = true;
    update();
  }
  QWidget::mousePressEvent(event);
}

void NXFloatButton::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXFloatButton);
  if (d->_isPressed && event->button() == Qt::LeftButton) {
    d->_isPressed = false;
    update();
    if (rect().contains(event->pos())) {
      if (d->_menu) {
        QPoint menuPos;
        if (d->_position == NXFloatButton::TopLeft || d->_position == NXFloatButton::TopRight) {
          menuPos = mapToGlobal(QPoint(0, height() + 5));
        } else {
          menuPos = mapToGlobal(QPoint(0, -d->_menu->sizeHint().height() - 5));
        }
        d->_menu->popup(menuPos);
      }
      Q_EMIT clicked();
    }
  }
  QWidget::mouseReleaseEvent(event);
}

bool NXFloatButton::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXFloatButton);
  if (watched == parentWidget() && event->type() == QEvent::Resize) {
    d->updatePosition();
  }
  return QWidget::eventFilter(watched, event);
}

bool NXFloatButton::event(QEvent *event)
{
  Q_D(NXFloatButton);
  switch (event->type()) {
  case QEvent::Enter : {
    d->_isAlphaAnimationFinished       = false;
    QPropertyAnimation *alphaAnimation = new QPropertyAnimation(d, "pHoverAlpha");
    connect(alphaAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      update();
    });
    connect(alphaAnimation, &QPropertyAnimation::finished, this, [=]() {
      d->_isAlphaAnimationFinished = true;
    });
    alphaAnimation->setDuration(200);
    alphaAnimation->setStartValue(d->_pHoverAlpha);
    alphaAnimation->setEndValue(255);
    alphaAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    break;
  }
  case QEvent::Leave : {
    d->_isAlphaAnimationFinished       = false;
    QPropertyAnimation *alphaAnimation = new QPropertyAnimation(d, "pHoverAlpha");
    connect(alphaAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      update();
    });
    connect(alphaAnimation, &QPropertyAnimation::finished, this, [=]() {
      d->_isAlphaAnimationFinished = true;
    });
    alphaAnimation->setDuration(200);
    alphaAnimation->setStartValue(d->_pHoverAlpha);
    alphaAnimation->setEndValue(0);
    alphaAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    break;
  }
  default : break;
  }
  return QWidget::event(event);
}

NXFloatButtonPrivate::NXFloatButtonPrivate(QObject *parent)
    : QObject { parent }
{
}

NXFloatButtonPrivate::~NXFloatButtonPrivate()
{
}

void NXFloatButtonPrivate::updatePosition()
{
  Q_Q(NXFloatButton);
  QWidget *parent = q->parentWidget();
  if (!parent) {
    return;
  }
  int x = 0;
  int y = 0;
  switch (_position) {
  case NXFloatButton::BottomRight :
    x = parent->width() - q->width() - _pMargin;
    y = parent->height() - q->height() - _pMargin;
    break;
  case NXFloatButton::BottomLeft :
    x = _pMargin;
    y = parent->height() - q->height() - _pMargin;
    break;
  case NXFloatButton::TopRight :
    x = parent->width() - q->width() - _pMargin;
    y = _pMargin;
    break;
  case NXFloatButton::TopLeft :
    x = _pMargin;
    y = _pMargin;
    break;
  }
  q->move(x, y);
  q->raise();
}
