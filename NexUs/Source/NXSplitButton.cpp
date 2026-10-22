#include "NXSplitButton.h"

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "NXMenu.h"
#include "NXTheme.h"
#include "private/NXSplitButtonPrivate.h"
Q_PROPERTY_CREATE_CPP(NXSplitButton, QS_SET_CREF(QString), Text)
Q_PROPERTY_CREATE_CPP(NXSplitButton, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSplitButton, NXIconType::IconName, NXIcon)

NXSplitButton::NXSplitButton(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXSplitButtonPrivate())
{
  Q_D(NXSplitButton);
  d->q_ptr          = this;
  d->_pBorderRadius = 3;
  d->_pText         = QString();
  d->_pNXIcon       = NXIconType::None;
  d->_themeMode     = nxTheme->getThemeMode();
  setMouseTracking(true);
  setFixedHeight(35);
  QFont font = this->font();
  font.setPixelSize(15);
  setFont(font);
  setObjectName("NXSplitButton");
  setStyleSheet(QStringLiteral("#NXSplitButton{background-color:transparent;}"));
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXSplitButton::~NXSplitButton()
{
}

void NXSplitButton::setMenu(NXMenu *menu)
{
  Q_D(NXSplitButton);
  d->_menu = menu;
}

NXMenu * NXSplitButton::getMenu() const
{
  Q_D(const NXSplitButton);
  return d->_menu;
}

void NXSplitButton::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXSplitButton);
  if (event->button() == Qt::LeftButton) {
    int splitX = width() - d->_dropdownWidth;
    if (event->pos().x() < splitX) {
      d->_isLeftPressed = true;
    } else {
      d->_isRightPressed = true;
    }
    update();
  }
  QWidget::mousePressEvent(event);
}

void NXSplitButton::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXSplitButton);
  if (event->button() == Qt::LeftButton) {
    int splitX = width() - d->_dropdownWidth;
    if (d->_isLeftPressed && event->pos().x() < splitX) {
      Q_EMIT clicked();
    } else if (d->_isRightPressed && event->pos().x() >= splitX) {
      if (d->_menu) {
        QPoint pos = mapToGlobal(QPoint(0, height()));
        d->_menu->popup(pos);
      }
    }
    d->_isLeftPressed  = false;
    d->_isRightPressed = false;
    update();
  }
  QWidget::mouseReleaseEvent(event);
}

void NXSplitButton::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXSplitButton);
  int splitX        = width() - d->_dropdownWidth;
  bool leftHovered  = event->pos().x() < splitX;
  bool rightHovered = !leftHovered;
  if (d->_isLeftHovered != leftHovered || d->_isRightHovered != rightHovered) {
    d->_isLeftHovered  = leftHovered;
    d->_isRightHovered = rightHovered;
    update();
  }
  QWidget::mouseMoveEvent(event);
}

void NXSplitButton::leaveEvent(QEvent *event)
{
  Q_D(NXSplitButton);
  d->_isLeftHovered  = false;
  d->_isRightHovered = false;
  d->_isLeftPressed  = false;
  d->_isRightPressed = false;
  update();
  QWidget::leaveEvent(event);
}

void NXSplitButton::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  Q_D(NXSplitButton);
  QPainter painter(this);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.save();

  int borderRadius = d->_pBorderRadius;
  int splitX       = width() - d->_dropdownWidth;
  QRect leftRect(0, 0, splitX, height());
  QRect rightRect(splitX, 0, d->_dropdownWidth, height());
  QRect fullRect(0, 0, width(), height());

  QColor baseColor   = NXThemeColor(d->_themeMode, BasicBase);
  QColor hoverColor  = NXThemeColor(d->_themeMode, BasicHover);
  QColor pressColor  = NXThemeColor(d->_themeMode, BasicPress);
  QColor textColor   = NXThemeColor(d->_themeMode, BasicText);
  QColor borderColor = NXThemeColor(d->_themeMode, BasicBorder);

  // Determine left and right fill colors
  QColor leftColor = baseColor;
  if (d->_isLeftPressed) {
    leftColor = pressColor;
  } else if (d->_isLeftHovered) {
    leftColor = hoverColor;
  }

  QColor rightColor = baseColor;
  if (d->_isRightPressed) {
    rightColor = pressColor;
  } else if (d->_isRightHovered) {
    rightColor = hoverColor;
  }

  // Draw full rounded rect background with base color
  QPainterPath fullPath;
  fullPath.addRoundedRect(fullRect, borderRadius, borderRadius);
  painter.setClipPath(fullPath);

  // Draw left area
  painter.setPen(Qt::NoPen);
  painter.setBrush(leftColor);
  painter.drawRect(leftRect);

  // Draw right area
  painter.setBrush(rightColor);
  painter.drawRect(rightRect);

  // Reset clip and draw border
  painter.setClipPath(fullPath);
  if (d->_themeMode == NXThemeType::Light) {
    painter.setPen(borderColor);
  } else {
    painter.setPen(Qt::NoPen);
  }
  painter.setBrush(Qt::NoBrush);
  painter.drawRoundedRect(fullRect, borderRadius, borderRadius);

  // Draw vertical separator line
  painter.setPen(QPen(borderColor, 1));
  painter.drawLine(splitX, 4, splitX, height() - 4);

  // Draw left area content: icon + text
  painter.setPen(textColor);
  if (d->_pNXIcon != NXIconType::None) {
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(18);
    QFontMetrics iconFm(iconFont);
    int iconWidth = iconFm.horizontalAdvance(QChar((unsigned short) d->_pNXIcon));

    QFont textFont = this->font();
    QFontMetrics textFm(textFont);
    int textWidth = d->_pText.isEmpty() ? 0 : textFm.horizontalAdvance(d->_pText);

    int spacing    = d->_pText.isEmpty() ? 0 : 8;
    int totalWidth = iconWidth + spacing + textWidth;
    int startX     = leftRect.x() + (leftRect.width() - totalWidth) / 2;

    painter.setFont(iconFont);
    painter.drawText(QRect(startX, leftRect.y(), iconWidth, leftRect.height()), Qt::AlignCenter, QChar((unsigned short) d->_pNXIcon));

    if (!d->_pText.isEmpty()) {
      painter.setFont(textFont);
      painter.drawText(QRect(startX + iconWidth + spacing, leftRect.y(), textWidth, leftRect.height()), Qt::AlignVCenter | Qt::AlignLeft, d->_pText);
    }
  } else {
    painter.setFont(this->font());
    painter.drawText(leftRect, Qt::AlignCenter, d->_pText);
  }

  // Draw right area: AngleDown icon
  QFont arrowFont = QFont(QStringLiteral("NXAwesome"));
  arrowFont.setPixelSize(14);
  painter.setFont(arrowFont);
  painter.setPen(textColor);
  painter.drawText(rightRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::AngleDown));

  painter.restore();
}

QSize NXSplitButton::sizeHint() const
{
  Q_D(const NXSplitButton);
  QFontMetrics fm(font());
  int textWidth = d->_pText.isEmpty() ? 0 : fm.horizontalAdvance(d->_pText);
  int iconWidth = (d->_pNXIcon != NXIconType::None) ? 24 : 0;
  int spacing   = (!d->_pText.isEmpty() && d->_pNXIcon != NXIconType::None) ? 8 : 0;
  int leftWidth = iconWidth + spacing + textWidth + 20;
  return QSize(leftWidth + d->_dropdownWidth, 35);
}

NXSplitButtonPrivate::NXSplitButtonPrivate(QObject *parent)
    : QObject(parent)
{
}

NXSplitButtonPrivate::~NXSplitButtonPrivate()
{
}
