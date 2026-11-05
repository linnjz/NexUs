#include "NXFloatButton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#include "NXMenu.h"
#include "NXTheme.h"
#include "private/NXFloatButtonPrivate.h"

Q_PROPERTY_CREATE_CPP(NXFloatButton, QPS_PAEMIT(int), ButtonSize)

void NXFloatButton::setMargin(int Margin)
{
  Q_D(NXFloatButton);
  d->m_Margin = Margin;
  d->updatePosition();
  Q_EMIT signalMarginChanged();
}

int NXFloatButton::getMargin() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  return d_ptr->m_Margin;
}

NXFloatButton::NXFloatButton(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXFloatButtonPrivate())
{
  Q_D(NXFloatButton);
  d->q_ptr        = this;
  d->m_ButtonSize = 56;
  d->m_Margin     = 30;
  d->m_HoverAlpha = 0;
  d->m_ThemeMode   = nxTheme->getThemeMode();

  setFixedSize(d->m_ButtonSize, d->m_ButtonSize);
  setCursor(Qt::PointingHandCursor);
  setMouseTracking(true);
  raise();

  if (parent) {
    parent->installEventFilter(this);
    d->updatePosition();
  }

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXFloatButton::NXFloatButton(NXIconType::IconName icon, QWidget *parent)
    : NXFloatButton(parent)
{
  Q_D(NXFloatButton);
  d->m_Icon = icon;
}

NXFloatButton::NXFloatButton(NXIconType::IconName icon, Position position, QWidget *parent)
    : NXFloatButton(parent)
{
  Q_D(NXFloatButton);
  d->m_Icon     = icon;
  d->m_Position = (int) position;
  d->updatePosition();
}

NXFloatButton::~NXFloatButton()
{
}

void NXFloatButton::setIcon(NXIconType::IconName icon)
{
  Q_D(NXFloatButton);
  d->m_Icon = icon;
  update();
}

NXIconType::IconName NXFloatButton::getIcon() const
{
  return d_ptr->m_Icon;
}

void NXFloatButton::setPosition(Position position)
{
  Q_D(NXFloatButton);
  d->m_Position = (int) position;
  d->updatePosition();
}

NXFloatButton::Position NXFloatButton::getPosition() const
{
  return (Position) d_ptr->m_Position;
}

void NXFloatButton::setMenu(NXMenu *menu)
{
  Q_D(NXFloatButton);
  d->m_Menu = menu;
}

NXMenu *NXFloatButton::getMenu() const
{
  return d_ptr->m_Menu;
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
  if (d->m_IsPressed) {
    painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryPress));
  } else if (d->m_IsAlphaAnimationFinished && underMouse()) {
    painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryHover));
  } else {
    QColor baseColor = NXThemeColor(d->m_ThemeMode, PrimaryNormal);
    if (d->m_HoverAlpha > 0) {
      QColor hoverColor = NXThemeColor(d->m_ThemeMode, PrimaryHover);
      qreal t           = d->m_HoverAlpha / 255.0;
      baseColor         = QColor(baseColor.red() + (hoverColor.red() - baseColor.red()) * t, baseColor.green() + (hoverColor.green() - baseColor.green()) * t, baseColor.blue() + (hoverColor.blue() - baseColor.blue()) * t);
    }
    painter.setBrush(baseColor);
  }
  painter.drawEllipse(foregroundRect);

  painter.setPen(Qt::white);
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(22);
  painter.setFont(iconFont);
  painter.drawText(foregroundRect, Qt::AlignCenter, QChar((unsigned short) d->m_Icon));
}

void NXFloatButton::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXFloatButton);
  if (event->button() == Qt::LeftButton) {
    d->m_IsPressed = true;
    update();
  }
  QWidget::mousePressEvent(event);
}

void NXFloatButton::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXFloatButton);
  if (d->m_IsPressed && event->button() == Qt::LeftButton) {
    d->m_IsPressed = false;
    update();
    if (rect().contains(event->pos())) {
      if (d->m_Menu) {
        QPoint menuPos;
        if (d->m_Position == NXFloatButton::TopLeft || d->m_Position == NXFloatButton::TopRight) {
          menuPos = mapToGlobal(QPoint(0, height() + 5));
        } else {
          menuPos = mapToGlobal(QPoint(0, -d->m_Menu->sizeHint().height() - 5));
        }
        d->m_Menu->popup(menuPos);
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
    d->m_IsAlphaAnimationFinished       = false;
    QPropertyAnimation *alphaAnimation = new QPropertyAnimation(d, "HoverAlpha");
    connect(alphaAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      update();
    });
    connect(alphaAnimation, &QPropertyAnimation::finished, this, [=]() {
      d->m_IsAlphaAnimationFinished = true;
    });
    alphaAnimation->setDuration(200);
    alphaAnimation->setStartValue(d->m_HoverAlpha);
    alphaAnimation->setEndValue(255);
    alphaAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    break;
  }
  case QEvent::Leave : {
    d->m_IsAlphaAnimationFinished       = false;
    QPropertyAnimation *alphaAnimation = new QPropertyAnimation(d, "HoverAlpha");
    connect(alphaAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      update();
    });
    connect(alphaAnimation, &QPropertyAnimation::finished, this, [=]() {
      d->m_IsAlphaAnimationFinished = true;
    });
    alphaAnimation->setDuration(200);
    alphaAnimation->setStartValue(d->m_HoverAlpha);
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
  switch (m_Position) {
  case NXFloatButton::BottomRight :
    x = parent->width() - q->width() - m_Margin;
    y = parent->height() - q->height() - m_Margin;
    break;
  case NXFloatButton::BottomLeft :
    x = m_Margin;
    y = parent->height() - q->height() - m_Margin;
    break;
  case NXFloatButton::TopRight :
    x = parent->width() - q->width() - m_Margin;
    y = m_Margin;
    break;
  case NXFloatButton::TopLeft :
    x = m_Margin;
    y = m_Margin;
    break;
  }
  q->move(x, y);
  q->raise();
}
