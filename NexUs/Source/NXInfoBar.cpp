#include "NXInfoBar.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXInfoBarPrivate.h"

Q_PROPERTY_CREATE_CPP(NXInfoBar, QS_SET_CREF(QString), Title)
Q_PROPERTY_CREATE_CPP(NXInfoBar, QS_SET_CREF(QString), Message)
Q_PROPERTY_CREATE_CPP(NXInfoBar, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXInfoBar, NXInfoBarType::InfoBarSeverity, Severity)
Q_PROPERTY_CREATE_CPP(NXInfoBar, NXIconType::IconName, InfoBarIcon)
Q_PROPERTY_CREATE_CPP(NXInfoBar, bool, IsClosable)

NXInfoBar::NXInfoBar(QWidget *parent)
    : QFrame { parent }
    , d_ptr(new NXInfoBarPrivate())
{
  Q_D(NXInfoBar);
  d->q_ptr          = this;
  d->_pBorderRadius = 4;
  d->_pTitle        = QStringLiteral("");
  d->_pMessage      = QStringLiteral("");
  d->_pSeverity     = NXInfoBarType::Informational;
  d->_pIsClosable   = true;
  d->_pInfoBarIcon  = NXIconType::None;
  d->_themeMode     = nxTheme->getThemeMode();
  setObjectName("NXInfoBar");
  setMouseTracking(true);
  setMinimumHeight(48);
  setFrameShape(QFrame::NoFrame);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXInfoBar::NXInfoBar(NXInfoBarType::InfoBarSeverity severity, QWidget *parent)
    : NXInfoBar(parent)
{
  Q_D(NXInfoBar);
  d->_pSeverity = severity;
}

NXInfoBar::~NXInfoBar()
{
}

void NXInfoBar::addAction(const QString &text, const std::function<void()> &callback)
{
  Q_D(NXInfoBar);
  d->_actions.append({ text, callback });
  update();
}

void NXInfoBar::clearActions()
{
  Q_D(NXInfoBar);
  d->_actions.clear();
  d->_actionButtonRects.clear();
  update();
}

void NXInfoBar::closeInfoBar()
{
  Q_D(NXInfoBar);
  Q_EMIT closeButtonClicked();
  d->_doCloseAnimation();
}

void NXInfoBar::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXInfoBar);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QColor severityColor              = d->_getSeverityColor();
  NXIconType::IconName severityIcon = d->_getSeverityIcon();

  QColor bgColor;
  if (d->_themeMode == NXThemeType::Light) {
    switch (d->_pSeverity) {
    case NXInfoBarType::Success       : bgColor = QColor(0xDF, 0xF6, 0xDD); break;
    case NXInfoBarType::Warning       : bgColor = QColor(0xFE, 0xF7, 0xE1); break;
    case NXInfoBarType::Error         : bgColor = QColor(0xFD, 0xE7, 0xE9); break;
    case NXInfoBarType::Informational :
    default                           : bgColor = QColor(0xF6, 0xF6, 0xF6); break;
    }
  } else {
    switch (d->_pSeverity) {
    case NXInfoBarType::Success       : bgColor = QColor(0x39, 0x3D, 0x1B); break;
    case NXInfoBarType::Warning       : bgColor = QColor(0x43, 0x3D, 0x19); break;
    case NXInfoBarType::Error         : bgColor = QColor(0x44, 0x21, 0x26); break;
    case NXInfoBarType::Informational :
    default                           : bgColor = QColor(0x2B, 0x2B, 0x2B); break;
    }
  }

  QPainterPath bgPath;
  bgPath.addRoundedRect(QRectF(rect()), d->_pBorderRadius, d->_pBorderRadius);
  painter.setPen(QPen(NXThemeColor(d->_themeMode, BasicBorder), 1));
  painter.setBrush(bgColor);
  painter.drawPath(bgPath);

  QPainterPath stripPath;
  int stripWidth = 3;
  QRectF stripRect(0, 0, d->_pBorderRadius * 2 + stripWidth, height());
  stripPath.addRoundedRect(stripRect, d->_pBorderRadius, d->_pBorderRadius);
  QPainterPath clipPath;
  clipPath.addRect(QRectF(0, 0, stripWidth, height()));
  stripPath = stripPath.intersected(clipPath);
  painter.setPen(Qt::NoPen);
  painter.setBrush(severityColor);
  painter.drawPath(stripPath);

  int contentLeft    = 16;
  int contentCenterY = height() / 2;

  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(16);
  painter.setFont(iconFont);
  painter.setPen(severityColor);
  QRect iconRect(contentLeft, contentCenterY - 10, 20, 20);
  painter.drawText(iconRect, Qt::AlignCenter, QChar((unsigned short) severityIcon));
  contentLeft += 28;

  int rightPadding    = 12;
  int closeButtonSize = 20;
  int rightContentEnd = width() - rightPadding;

  if (d->_pIsClosable) {
    d->_closeButtonRect = QRect(rightContentEnd - closeButtonSize, contentCenterY - closeButtonSize / 2, closeButtonSize, closeButtonSize);
    rightContentEnd     = d->_closeButtonRect.left() - 8;

    if (d->_closeButtonHover) {
      painter.setPen(Qt::NoPen);
      painter.setBrush(NXThemeColor(d->_themeMode, BasicHover));
      painter.drawRoundedRect(d->_closeButtonRect.adjusted(-2, -2, 2, 2), 3, 3);
    }

    QFont closeFont = QFont(QStringLiteral("NXAwesome"));
    closeFont.setPixelSize(10);
    painter.setFont(closeFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicText));
    painter.drawText(d->_closeButtonRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Xmark));
  }

  d->_actionButtonRects.clear();
  if (!d->_actions.isEmpty()) {
    QFont buttonFont = font();
    buttonFont.setPixelSize(12);
    QFontMetrics buttonFm(buttonFont);
    int buttonHeight   = 26;
    int buttonSpacing  = 6;
    int buttonPaddingH = 14;

    int buttonX = rightContentEnd;
    for (int i = d->_actions.size() - 1; i >= 0; --i) {
      int textWidth   = buttonFm.horizontalAdvance(d->_actions[i].text);
      int buttonWidth = textWidth + buttonPaddingH * 2;
      buttonX -= buttonWidth;
      QRect btnRect(buttonX, contentCenterY - buttonHeight / 2, buttonWidth, buttonHeight);
      d->_actionButtonRects.prepend(btnRect);
      buttonX -= buttonSpacing;

      painter.setPen(QPen(NXThemeColor(d->_themeMode, BasicBorder), 1));
      if (d->_hoverActionIndex == i) {
        painter.setBrush(NXThemeColor(d->_themeMode, BasicHover));
      } else {
        painter.setBrush(NXThemeColor(d->_themeMode, BasicBase));
      }
      painter.drawRoundedRect(btnRect, 4, 4);

      painter.setFont(buttonFont);
      painter.setPen(NXThemeColor(d->_themeMode, BasicText));
      painter.drawText(btnRect, Qt::AlignCenter, d->_actions[i].text);
    }
    rightContentEnd = buttonX - 4;
  }

  int textAreaWidth = rightContentEnd - contentLeft;
  QFont titleFont   = font();
  titleFont.setPixelSize(13);
  titleFont.setBold(true);
  QFontMetrics titleFm(titleFont);

  QFont messageFont = font();
  messageFont.setPixelSize(13);
  QFontMetrics messageFm(messageFont);

  painter.setPen(NXThemeColor(d->_themeMode, BasicText));

  if (!d->_pTitle.isEmpty() && !d->_pMessage.isEmpty()) {
    QString elidedTitle = titleFm.elidedText(d->_pTitle, Qt::ElideRight, textAreaWidth);
    painter.setFont(titleFont);
    int titleWidth = titleFm.horizontalAdvance(elidedTitle);
    painter.drawText(QRect(contentLeft, contentCenterY - 10, titleWidth, 20), Qt::AlignVCenter | Qt::AlignLeft, elidedTitle);

    int messageLeft  = contentLeft + titleWidth + 8;
    int messageWidth = rightContentEnd - messageLeft;
    if (messageWidth > 0) {
      painter.setFont(messageFont);
      painter.setPen(NXThemeColor(d->_themeMode, BasicDetailsText));
      QString elidedMessage = messageFm.elidedText(d->_pMessage, Qt::ElideRight, messageWidth);
      painter.drawText(QRect(messageLeft, contentCenterY - 10, messageWidth, 20), Qt::AlignVCenter | Qt::AlignLeft, elidedMessage);
    }
  } else if (!d->_pTitle.isEmpty()) {
    painter.setFont(titleFont);
    QString elidedTitle = titleFm.elidedText(d->_pTitle, Qt::ElideRight, textAreaWidth);
    painter.drawText(QRect(contentLeft, contentCenterY - 10, textAreaWidth, 20), Qt::AlignVCenter | Qt::AlignLeft, elidedTitle);
  } else if (!d->_pMessage.isEmpty()) {
    painter.setFont(messageFont);
    QString elidedMessage = messageFm.elidedText(d->_pMessage, Qt::ElideRight, textAreaWidth);
    painter.drawText(QRect(contentLeft, contentCenterY - 10, textAreaWidth, 20), Qt::AlignVCenter | Qt::AlignLeft, elidedMessage);
  }
}

void NXInfoBar::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXInfoBar);
  if (event->button() != Qt::LeftButton) {
    QFrame::mousePressEvent(event);
    return;
  }

  if (d->_pIsClosable && d->_closeButtonRect.contains(event->pos())) {
    closeInfoBar();
    return;
  }

  for (int i = 0; i < d->_actionButtonRects.size(); ++i) {
    if (d->_actionButtonRects[i].contains(event->pos())) {
      if (d->_actions[i].callback) {
        d->_actions[i].callback();
      }
      return;
    }
  }
  QFrame::mousePressEvent(event);
}

void NXInfoBar::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXInfoBar);
  bool needUpdate = false;

  if (d->_pIsClosable) {
    bool hover = d->_closeButtonRect.contains(event->pos());
    if (hover != d->_closeButtonHover) {
      d->_closeButtonHover = hover;
      needUpdate           = true;
    }
  }

  int hoverIndex = -1;
  for (int i = 0; i < d->_actionButtonRects.size(); ++i) {
    if (d->_actionButtonRects[i].contains(event->pos())) {
      hoverIndex = i;
      break;
    }
  }
  if (hoverIndex != d->_hoverActionIndex) {
    d->_hoverActionIndex = hoverIndex;
    needUpdate           = true;
  }

  if (needUpdate) {
    setCursor(d->_closeButtonHover || hoverIndex >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
    update();
  }
  QFrame::mouseMoveEvent(event);
}

void NXInfoBar::leaveEvent(QEvent *event)
{
  Q_D(NXInfoBar);
  bool needUpdate = false;
  if (d->_closeButtonHover) {
    d->_closeButtonHover = false;
    needUpdate           = true;
  }
  if (d->_hoverActionIndex != -1) {
    d->_hoverActionIndex = -1;
    needUpdate           = true;
  }
  if (needUpdate) {
    setCursor(Qt::ArrowCursor);
    update();
  }
  QFrame::leaveEvent(event);
}
