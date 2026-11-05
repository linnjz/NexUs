#include "NXSnackbar.h"

#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QScreen>
#include <QTimer>

#include "NXTheme.h"
#include "private/NXSnackbarPrivate.h"

static int s_maxSnackbars  = 5;
static int const s_spacing = 10;
static QList<NXSnackbar *> s_activeSnackbars;
static int s_baseBottomY = 0;
static int s_centerX     = 0;

static int
s_calcTargetY(int index)
{
  int offset = 0;
  for (int i = s_activeSnackbars.size() - 1; i > index; --i) {
    offset += s_activeSnackbars[i]->height() + s_spacing;
  }
  return s_baseBottomY - s_activeSnackbars[index]->height() - offset;
}

void NXSnackbar::_reflowAll()
{
  for (int i = 0; i < s_activeSnackbars.size(); ++i) {
    NXSnackbar *snackbar = s_activeSnackbars[i];
    int targetY          = s_calcTargetY(i);
    if (snackbar->y() != targetY) {
      QPropertyAnimation *moveAnim = new QPropertyAnimation(snackbar, "pos", snackbar);
      moveAnim->setDuration(200);
      moveAnim->setStartValue(snackbar->pos());
      moveAnim->setEndValue(QPoint(s_centerX, targetY));
      moveAnim->setEasingCurve(QEasingCurve::OutCubic);
      moveAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }
  }
}

Q_PROPERTY_CREATE_CPP(NXSnackbar, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSnackbar, QPS_PAEMIT(int), DisplayMsec)

NXSnackbar::NXSnackbar(
    SnackbarType type, QString const &text, QString const &actionText, int displayMsec, QWidget *parent)
    : QWidget { nullptr }
    , d_ptr(new NXSnackbarPrivate())
{
  Q_D(NXSnackbar);
  d->q_ptr          = this;
  d->m_BorderRadius = 8;
  d->m_DisplayMsec  = displayMsec;
  d->m_SnackbarType  = type;
  d->m_Text          = text;
  d->m_ActionText    = actionText;

  setObjectName("NXSnackbar");
  setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_DeleteOnClose);
  setMouseTracking(true);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });

  QFont textFont = font();
  textFont.setPixelSize(14);
  QFontMetrics fm(textFont);
  int textWidth   = fm.horizontalAdvance(text);
  int actionWidth = 0;
  if (!actionText.isEmpty()) {
    QFont actionFont = textFont;
    actionFont.setBold(true);
    QFontMetrics afm(actionFont);
    actionWidth = afm.horizontalAdvance(actionText) + 24;
  }
  int totalWidth = textWidth + 80 + actionWidth;
  totalWidth     = qBound(280, totalWidth, 560);
  setFixedHeight(48 + d->m_ShadowBorderWidth * 2);
  setFixedWidth(totalWidth + d->m_ShadowBorderWidth * 2);

  while (s_activeSnackbars.size() >= s_maxSnackbars) {
    s_activeSnackbars.first()->dismiss();
  }

  if (parent) {
    QRect parentRect = parent->geometry();
    s_centerX        = parentRect.x() + (parentRect.width() - width()) / 2;
    s_baseBottomY    = parentRect.y() + parentRect.height() - 30;
  } else {
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
      QRect screenRect = screen->availableGeometry();
      s_centerX        = screenRect.x() + (screenRect.width() - width()) / 2;
      s_baseBottomY    = screenRect.y() + screenRect.height() - 30;
    }
  }

  s_activeSnackbars.append(this);
  int targetY = s_calcTargetY(s_activeSnackbars.size() - 1);

  for (int i = 0; i < s_activeSnackbars.size() - 1; ++i) {
    NXSnackbar *snackbar = s_activeSnackbars[i];
    int newY             = s_calcTargetY(i);
    if (snackbar->y() != newY) {
      QPropertyAnimation *moveAnim = new QPropertyAnimation(snackbar, "pos", snackbar);
      moveAnim->setDuration(200);
      moveAnim->setStartValue(snackbar->pos());
      moveAnim->setEndValue(QPoint(s_centerX, newY));
      moveAnim->setEasingCurve(QEasingCurve::OutCubic);
      moveAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }
  }

  move(s_centerX, targetY + 20);
  show();

  QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
  opacityEffect->setOpacity(0);
  setGraphicsEffect(opacityEffect);

  QPropertyAnimation *fadeIn = new QPropertyAnimation(opacityEffect, "opacity", this);
  fadeIn->setDuration(250);
  fadeIn->setStartValue(0.0);
  fadeIn->setEndValue(1.0);
  fadeIn->setEasingCurve(QEasingCurve::OutCubic);

  QPropertyAnimation *slideIn = new QPropertyAnimation(this, "pos", this);
  slideIn->setDuration(250);
  slideIn->setStartValue(QPoint(s_centerX, targetY + 20));
  slideIn->setEndValue(QPoint(s_centerX, targetY));
  slideIn->setEasingCurve(QEasingCurve::OutCubic);

  d->m_DoClose = [=]() {
    if (d->m_IsClosing) {
      return;
    }
    d->m_IsClosing = true;
    s_activeSnackbars.removeOne(this);
    _reflowAll();

    QPropertyAnimation *fadeOut = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeOut->setDuration(300);
    fadeOut->setStartValue(opacityEffect->opacity());
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::InCubic);
    connect(fadeOut, &QPropertyAnimation::finished, this, [=]() {
      Q_EMIT closed();
      close();
      deleteLater();
    });
    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
  };

  connect(fadeIn, &QPropertyAnimation::finished, this, [=]() {
    QTimer::singleShot(displayMsec, this, d->m_DoClose);
  });

  fadeIn->start();
  slideIn->start();
}

NXSnackbar::~NXSnackbar()
{
}

void NXSnackbar::setMaxCount(int count)
{
  s_maxSnackbars = qBound(1, count, 12);
}

int NXSnackbar::getMaxCount()
{
  return s_maxSnackbars;
}

void NXSnackbar::dismiss()
{
  Q_D(NXSnackbar);
  if (d->m_DoClose) {
    d->m_DoClose();
  }
}

NXSnackbar *NXSnackbar::success(QString const &text, QString const &actionText, int displayMsec, QWidget *parent)
{
  return new NXSnackbar(Success, text, actionText, displayMsec, parent);
}

NXSnackbar *NXSnackbar::info(QString const &text, QString const &actionText, int displayMsec, QWidget *parent)
{
  return new NXSnackbar(Info, text, actionText, displayMsec, parent);
}

NXSnackbar *NXSnackbar::warning(QString const &text, QString const &actionText, int displayMsec, QWidget *parent)
{
  return new NXSnackbar(Warning, text, actionText, displayMsec, parent);
}

NXSnackbar *NXSnackbar::error(QString const &text, QString const &actionText, int displayMsec, QWidget *parent)
{
  return new NXSnackbar(Error, text, actionText, displayMsec, parent);
}

void NXSnackbar::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXSnackbar);
  QPainter painter(this);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);

  nxTheme->drawEffectShadow(&painter, rect(), d->m_ShadowBorderWidth, d->m_BorderRadius);

  QRect foregroundRect(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, width() - 2 * d->m_ShadowBorderWidth, height() - 2 * d->m_ShadowBorderWidth);

  painter.save();
  painter.setPen(NXThemeColor(d->m_ThemeMode, PopupBorder));
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PopupBase));
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  painter.restore();

  QColor indicatorColor;
  NXIconType::IconName iconEnum;
  QColor iconColor;
  switch (d->m_SnackbarType) {
  case Success :
    indicatorColor = QColor(0x0F, 0x7B, 0x0F);
    iconEnum       = NXIconType::Check;
    iconColor      = QColor(0x0F, 0x7B, 0x0F);
    break;
  case Info :
    indicatorColor = NXThemeColor(d->m_ThemeMode, PrimaryNormal);
    iconEnum       = NXIconType::CircleInfo;
    iconColor      = NXThemeColor(d->m_ThemeMode, PrimaryNormal);
    break;
  case Warning :
    indicatorColor = QColor(0xF7, 0x93, 0x0E);
    iconEnum       = NXIconType::CircleExclamation;
    iconColor      = QColor(0xF7, 0x93, 0x0E);
    break;
  case Error :
    indicatorColor = NXThemeColor(d->m_ThemeMode, StatusDanger);
    iconEnum       = NXIconType::CircleXmark;
    iconColor      = NXThemeColor(d->m_ThemeMode, StatusDanger);
    break;
  }

  painter.save();
  painter.setPen(Qt::NoPen);
  painter.setBrush(indicatorColor);
  QPainterPath clipPath;
  clipPath.addRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  painter.setClipPath(clipPath);
  painter.drawRect(QRect(foregroundRect.x(), foregroundRect.y(), 4, foregroundRect.height()));
  painter.restore();

  painter.save();
  painter.setPen(iconColor);
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(16);
  painter.setFont(iconFont);
  QRect iconRect(foregroundRect.x() + 14, foregroundRect.y(), 20, foregroundRect.height());
  painter.drawText(iconRect, Qt::AlignCenter, QChar((unsigned short) iconEnum));
  painter.restore();

  int textLeft  = foregroundRect.x() + 42;
  int textRight = foregroundRect.right() - 10;

  if (!d->m_ActionText.isEmpty()) {
    QFont actionFont = font();
    actionFont.setPixelSize(14);
    actionFont.setBold(true);
    QFontMetrics afm(actionFont);
    int actionWidth  = afm.horizontalAdvance(d->m_ActionText) + 16;
    int actionHeight = 28;
    int actionX      = foregroundRect.right() - actionWidth - 8;
    int actionY      = foregroundRect.y() + (foregroundRect.height() - actionHeight) / 2;
    d->m_ActionRect   = QRect(actionX, actionY, actionWidth, actionHeight);
    textRight        = actionX - 8;

    if (d->m_IsActionPressed) {
      painter.save();
      painter.setPen(Qt::NoPen);
      painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicPress));
      painter.drawRoundedRect(d->m_ActionRect, 4, 4);
      painter.restore();
    } else if (d->m_IsActionHover) {
      painter.save();
      painter.setPen(Qt::NoPen);
      painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicHover));
      painter.drawRoundedRect(d->m_ActionRect, 4, 4);
      painter.restore();
    }

    painter.save();
    painter.setPen(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
    painter.setFont(actionFont);
    painter.drawText(d->m_ActionRect, Qt::AlignCenter, d->m_ActionText);
    painter.restore();
  }

  painter.save();
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  QFont textFont = font();
  textFont.setPixelSize(14);
  painter.setFont(textFont);
  QRect textRect(textLeft, foregroundRect.y(), textRight - textLeft, foregroundRect.height());
  painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, painter.fontMetrics().elidedText(d->m_Text, Qt::ElideRight, textRect.width()));
  painter.restore();
}

void NXSnackbar::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXSnackbar);
  if (!d->m_ActionText.isEmpty() && d->m_ActionRect.contains(event->pos())) {
    d->m_IsActionPressed = true;
    update();
  }
  QWidget::mousePressEvent(event);
}

void NXSnackbar::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXSnackbar);
  if (d->m_IsActionPressed && d->m_ActionRect.contains(event->pos())) {
    Q_EMIT actionClicked();
  }
  d->m_IsActionPressed = false;
  update();
  QWidget::mouseReleaseEvent(event);
}

void NXSnackbar::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXSnackbar);
  bool hover = !d->m_ActionText.isEmpty() && d->m_ActionRect.contains(event->pos());
  if (hover != d->m_IsActionHover) {
    d->m_IsActionHover = hover;
    setCursor(hover ? Qt::PointingHandCursor : Qt::ArrowCursor);
    update();
  }
  QWidget::mouseMoveEvent(event);
}

NXSnackbarPrivate::NXSnackbarPrivate(QObject *parent)
    : QObject { parent }
{
}

NXSnackbarPrivate::~NXSnackbarPrivate()
{
}
