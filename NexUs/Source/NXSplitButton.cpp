#include "NXSplitButton.h"

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "NXMenu.h"
#include "NXTheme.h"
#include "private/NXSplitButtonPrivate.h"
Q_PROPERTY_CREATE_CPP(NXSplitButton, QPS_SCREF_PAEMIT(QString), Text)
Q_PROPERTY_CREATE_CPP(NXSplitButton, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSplitButton, QPS_PAEMIT(NXIconType::IconName), NXIcon)

NXSplitButton::NXSplitButton(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXSplitButtonPrivate())
{
  Q_D(NXSplitButton);
  d->q_ptr          = this;
  d->m_BorderRadius = 3;
  d->m_Text         = QString();
  d->m_NXIcon       = NXIconType::None;
  d->m_ThemeMode     = nxTheme->getThemeMode();
  setMouseTracking(true);
  setFixedHeight(35);
  QFont font = this->font();
  font.setPixelSize(15);
  setFont(font);
  setObjectName("NXSplitButton");
  setStyleSheet(QStringLiteral("#NXSplitButton{background-color:transparent;}"));
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXSplitButton::~NXSplitButton()
{
}

void NXSplitButton::setMenu(NXMenu *menu)
{
  Q_D(NXSplitButton);
  d->m_Menu = menu;
}

NXMenu *NXSplitButton::getMenu() const
{
  Q_CONST_D(NXSplitButton);
  return d->m_Menu;
}

void NXSplitButton::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXSplitButton);
  if (event->button() == Qt::LeftButton) {
    int splitX = width() - d->m_DropdownWidth;
    if (event->pos().x() < splitX) {
      d->m_IsLeftPressed = true;
    } else {
      d->m_IsRightPressed = true;
    }
    update();
  }
  QWidget::mousePressEvent(event);
}

void NXSplitButton::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXSplitButton);
  if (event->button() == Qt::LeftButton) {
    int splitX = width() - d->m_DropdownWidth;
    if (d->m_IsLeftPressed && event->pos().x() < splitX) {
      Q_EMIT clicked();
    } else if (d->m_IsRightPressed && event->pos().x() >= splitX) {
      if (d->m_Menu) {
        QPoint pos = mapToGlobal(QPoint(0, height()));
        d->m_Menu->popup(pos);
      }
    }
    d->m_IsLeftPressed  = false;
    d->m_IsRightPressed = false;
    update();
  }
  QWidget::mouseReleaseEvent(event);
}

void NXSplitButton::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXSplitButton);
  int splitX        = width() - d->m_DropdownWidth;
  bool leftHovered  = event->pos().x() < splitX;
  bool rightHovered = !leftHovered;
  if (d->m_IsLeftHovered != leftHovered || d->m_IsRightHovered != rightHovered) {
    d->m_IsLeftHovered  = leftHovered;
    d->m_IsRightHovered = rightHovered;
    update();
  }
  QWidget::mouseMoveEvent(event);
}

void NXSplitButton::leaveEvent(QEvent *event)
{
  Q_D(NXSplitButton);
  d->m_IsLeftHovered  = false;
  d->m_IsRightHovered = false;
  d->m_IsLeftPressed  = false;
  d->m_IsRightPressed = false;
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

  int borderRadius = d->m_BorderRadius;
  int splitX       = width() - d->m_DropdownWidth;
  QRect leftRect(0, 0, splitX, height());
  QRect rightRect(splitX, 0, d->m_DropdownWidth, height());
  QRect fullRect(0, 0, width(), height());

  QColor baseColor   = NXThemeColor(d->m_ThemeMode, BasicBase);
  QColor hoverColor  = NXThemeColor(d->m_ThemeMode, BasicHover);
  QColor pressColor  = NXThemeColor(d->m_ThemeMode, BasicPress);
  QColor textColor   = NXThemeColor(d->m_ThemeMode, BasicText);
  QColor borderColor = NXThemeColor(d->m_ThemeMode, BasicBorder);

  // Determine left and right fill colors
  QColor leftColor = baseColor;
  if (d->m_IsLeftPressed) {
    leftColor = pressColor;
  } else if (d->m_IsLeftHovered) {
    leftColor = hoverColor;
  }

  QColor rightColor = baseColor;
  if (d->m_IsRightPressed) {
    rightColor = pressColor;
  } else if (d->m_IsRightHovered) {
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
  if (d->m_ThemeMode == NXThemeType::Light) {
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
  if (d->m_NXIcon != NXIconType::None) {
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(18);
    QFontMetrics iconFm(iconFont);
    int iconWidth = iconFm.horizontalAdvance(QChar((unsigned short) d->m_NXIcon));

    QFont textFont = this->font();
    QFontMetrics textFm(textFont);
    int textWidth = d->m_Text.isEmpty() ? 0 : textFm.horizontalAdvance(d->m_Text);

    int spacing    = d->m_Text.isEmpty() ? 0 : 8;
    int totalWidth = iconWidth + spacing + textWidth;
    int startX     = leftRect.x() + (leftRect.width() - totalWidth) / 2;

    painter.setFont(iconFont);
    painter.drawText(QRect(startX, leftRect.y(), iconWidth, leftRect.height()), Qt::AlignCenter, QChar((unsigned short) d->m_NXIcon));

    if (!d->m_Text.isEmpty()) {
      painter.setFont(textFont);
      painter.drawText(QRect(startX + iconWidth + spacing, leftRect.y(), textWidth, leftRect.height()), Qt::AlignVCenter | Qt::AlignLeft, d->m_Text);
    }
  } else {
    painter.setFont(this->font());
    painter.drawText(leftRect, Qt::AlignCenter, d->m_Text);
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
  Q_CONST_D(NXSplitButton);
  QFontMetrics fm(font());
  int textWidth = d->m_Text.isEmpty() ? 0 : fm.horizontalAdvance(d->m_Text);
  int iconWidth = (d->m_NXIcon != NXIconType::None) ? 24 : 0;
  int spacing   = (!d->m_Text.isEmpty() && d->m_NXIcon != NXIconType::None) ? 8 : 0;
  int leftWidth = iconWidth + spacing + textWidth + 20;
  return QSize(leftWidth + d->m_DropdownWidth, 35);
}

NXSplitButtonPrivate::NXSplitButtonPrivate(QObject *parent)
    : QObject(parent)
{
}

NXSplitButtonPrivate::~NXSplitButtonPrivate()
{
}
