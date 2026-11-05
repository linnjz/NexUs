#include "NXDropDownButton.h"

#include <QEvent>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "NXMenu.h"
#include "NXTheme.h"
#include "private/NXDropDownButtonPrivate.h"

Q_PROPERTY_CREATE_CPP(NXDropDownButton, QPS_SCREF_PAEMIT(QString), Text)
Q_PROPERTY_CREATE_CPP(NXDropDownButton, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXDropDownButton, QPS_PAEMIT(NXIconType::IconName), NXIcon)

NXDropDownButton::NXDropDownButton(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXDropDownButtonPrivate())
{
  Q_D(NXDropDownButton);
  d->q_ptr          = this;
  d->m_BorderRadius = 6;
  d->m_NXIcon       = NXIconType::None;
  setObjectName("NXDropDownButton");
  setFixedHeight(35);
  setMouseTracking(true);
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXDropDownButton::~NXDropDownButton()
{
}

void NXDropDownButton::setMenu(NXMenu *menu)
{
  Q_D(NXDropDownButton);
  d->m_Menu = menu;
}

NXMenu *NXDropDownButton::getMenu() const
{
  return d_ptr->m_Menu;
}

bool NXDropDownButton::event(QEvent *event)
{
  Q_D(NXDropDownButton);
  switch (event->type()) {
  case QEvent::HoverEnter : {
    d->m_IsHover = true;
    update();
    break;
  }
  case QEvent::HoverLeave : {
    d->m_IsHover = false;
    update();
    break;
  }
  default : {
    break;
  }
  }
  return QWidget::event(event);
}

void NXDropDownButton::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXDropDownButton);
  if (event->button() == Qt::LeftButton) {
    d->m_IsPressed = true;
    update();
  }
  QWidget::mousePressEvent(event);
}

void NXDropDownButton::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXDropDownButton);
  if (event->button() == Qt::LeftButton) {
    d->m_IsPressed = false;
    update();
    if (d->m_Menu && rect().contains(event->pos())) {
      d->m_Menu->popup(mapToGlobal(QPoint(0, height())));
    }
  }
  QWidget::mouseReleaseEvent(event);
}

void NXDropDownButton::leaveEvent(QEvent *event)
{
  Q_D(NXDropDownButton);
  d->m_IsHover   = false;
  d->m_IsPressed = false;
  update();
  QWidget::leaveEvent(event);
}

void NXDropDownButton::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  Q_D(NXDropDownButton);

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  // Background
  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(QRectF(0.5, 0.5, width() - 1, height() - 1), d->m_BorderRadius, d->m_BorderRadius);
  if (d->m_IsPressed) {
    painter.fillPath(backgroundPath, NXThemeColor(d->m_ThemeMode, BasicPress));
  } else if (d->m_IsHover) {
    painter.fillPath(backgroundPath, NXThemeColor(d->m_ThemeMode, BasicHover));
  } else {
    painter.fillPath(backgroundPath, NXThemeColor(d->m_ThemeMode, BasicBase));
  }

  // Border (light mode only)
  if (d->m_ThemeMode == NXThemeType::Light) {
    painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
    painter.drawPath(backgroundPath);
  }

  // Arrow area
  int arrowAreaWidth   = 20;
  int arrowRightMargin = 8;

  // Content area (excluding arrow)
  int contentLeft  = 10;
  int contentRight = width() - arrowAreaWidth;
  int contentWidth = contentRight - contentLeft;

  // Calculate icon + text total width for centering
  int iconWidth   = 0;
  int iconTextGap = 6;
  int textWidth   = 0;

  if (d->m_NXIcon != NXIconType::None) {
    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(16);
    iconWidth = QFontMetrics(iconFont).horizontalAdvance(QChar(static_cast<int>(d->m_NXIcon)));
  }

  if (!d->m_Text.isEmpty()) {
    QFont textFont = font();
    textFont.setPixelSize(14);
    textWidth = QFontMetrics(textFont).horizontalAdvance(d->m_Text);
  }

  int totalContentWidth = iconWidth;
  if (iconWidth > 0 && textWidth > 0) {
    totalContentWidth += iconTextGap;
  }
  totalContentWidth += textWidth;

  int drawX = contentLeft + (contentWidth - totalContentWidth) / 2;

  // Draw icon
  if (d->m_NXIcon != NXIconType::None) {
    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(16);
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    QRect iconRect(drawX, 0, iconWidth, height());
    painter.drawText(iconRect, Qt::AlignCenter, QString(QChar(static_cast<int>(d->m_NXIcon))));
    drawX += iconWidth + iconTextGap;
  }

  // Draw text
  if (!d->m_Text.isEmpty()) {
    QFont textFont = font();
    textFont.setPixelSize(14);
    painter.setFont(textFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    QRect textRect(drawX, 0, textWidth, height());
    painter.drawText(textRect, Qt::AlignCenter, d->m_Text);
  }

  // Draw arrow (AngleDown)
  QFont arrowFont(QStringLiteral("NXAwesome"));
  arrowFont.setPixelSize(16);
  painter.setFont(arrowFont);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  QRect arrowRect(width() - arrowAreaWidth - arrowRightMargin, 0, arrowAreaWidth, height());
  painter.drawText(arrowRect, Qt::AlignCenter, QString(QChar(static_cast<int>(NXIconType::AngleDown))));
}

QSize NXDropDownButton::sizeHint() const
{
  Q_CONST_D(NXDropDownButton);

  int iconWidth        = 0;
  int iconTextGap      = 6;
  int textWidth        = 0;
  int arrowAreaWidth   = 20;
  int arrowRightMargin = 8;
  int leftPadding      = 10;
  int rightPadding     = 10;

  if (d->m_NXIcon != NXIconType::None) {
    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(16);
    iconWidth = QFontMetrics(iconFont).horizontalAdvance(QChar(static_cast<int>(d->m_NXIcon)));
  }

  if (!d->m_Text.isEmpty()) {
    QFont textFont = font();
    textFont.setPixelSize(14);
    textWidth = QFontMetrics(textFont).horizontalAdvance(d->m_Text);
  }

  int totalWidth = leftPadding + iconWidth;
  if (iconWidth > 0 && textWidth > 0) {
    totalWidth += iconTextGap;
  }
  totalWidth += textWidth + arrowAreaWidth + arrowRightMargin + rightPadding;

  return QSize(totalWidth, 35);
}

NXDropDownButtonPrivate::NXDropDownButtonPrivate(QObject *parent)
    : QObject { parent }
{
}

NXDropDownButtonPrivate::~NXDropDownButtonPrivate()
{
}
