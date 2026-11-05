#include "NXInfoBar.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXInfoBarPrivate.h"

Q_PROPERTY_CREATE_CPP(NXInfoBar, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXInfoBar, QPS_SCREF_PAEMIT(QString), Message)
Q_PROPERTY_CREATE_CPP(NXInfoBar, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXInfoBar, QPS_PAEMIT(NXInfoBarType::InfoBarSeverity), Severity)
Q_PROPERTY_CREATE_CPP(NXInfoBar, QPS_PAEMIT(NXIconType::IconName), InfoBarIcon)
Q_PROPERTY_CREATE_CPP(NXInfoBar, QPS_PAEMIT(bool), IsClosable)

NXInfoBar::NXInfoBar(QWidget *parent)
    : QFrame { parent }
    , d_ptr(new NXInfoBarPrivate())
{
  Q_D(NXInfoBar);
  d->q_ptr          = this;
  d->m_BorderRadius = 4;
  d->m_Title        = QStringLiteral("");
  d->m_Message      = QStringLiteral("");
  d->m_Severity     = NXInfoBarType::Informational;
  d->m_IsClosable   = true;
  d->m_InfoBarIcon  = NXIconType::None;
  d->m_ThemeMode     = nxTheme->getThemeMode();
  setObjectName("NXInfoBar");
  setMouseTracking(true);
  setMinimumHeight(48);
  setFrameShape(QFrame::NoFrame);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXInfoBar::NXInfoBar(NXInfoBarType::InfoBarSeverity severity, QWidget *parent)
    : NXInfoBar(parent)
{
  Q_D(NXInfoBar);
  d->m_Severity = severity;
}

NXInfoBar::~NXInfoBar()
{
}

void NXInfoBar::addAction(QString const &text, std::function<void()> const &callback)
{
  Q_D(NXInfoBar);
  d->m_Actions.append({ text, callback });
  update();
}

void NXInfoBar::clearActions()
{
  Q_D(NXInfoBar);
  d->m_Actions.clear();
  d->m_ActionButtonRects.clear();
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
  if (d->m_ThemeMode == NXThemeType::Light) {
    switch (d->m_Severity) {
    case NXInfoBarType::Success       : bgColor = QColor(0xDF, 0xF6, 0xDD); break;
    case NXInfoBarType::Warning       : bgColor = QColor(0xFE, 0xF7, 0xE1); break;
    case NXInfoBarType::Error         : bgColor = QColor(0xFD, 0xE7, 0xE9); break;
    case NXInfoBarType::Informational :
    default                           : bgColor = QColor(0xF6, 0xF6, 0xF6); break;
    }
  } else {
    switch (d->m_Severity) {
    case NXInfoBarType::Success       : bgColor = QColor(0x39, 0x3D, 0x1B); break;
    case NXInfoBarType::Warning       : bgColor = QColor(0x43, 0x3D, 0x19); break;
    case NXInfoBarType::Error         : bgColor = QColor(0x44, 0x21, 0x26); break;
    case NXInfoBarType::Informational :
    default                           : bgColor = QColor(0x2B, 0x2B, 0x2B); break;
    }
  }

  QPainterPath bgPath;
  bgPath.addRoundedRect(QRectF(rect()), d->m_BorderRadius, d->m_BorderRadius);
  painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
  painter.setBrush(bgColor);
  painter.drawPath(bgPath);

  QPainterPath stripPath;
  int stripWidth = 3;
  QRectF stripRect(0, 0, d->m_BorderRadius * 2 + stripWidth, height());
  stripPath.addRoundedRect(stripRect, d->m_BorderRadius, d->m_BorderRadius);
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

  if (d->m_IsClosable) {
    d->m_CloseButtonRect = QRect(rightContentEnd - closeButtonSize, contentCenterY - closeButtonSize / 2, closeButtonSize, closeButtonSize);
    rightContentEnd     = d->m_CloseButtonRect.left() - 8;

    if (d->m_CloseButtonHover) {
      painter.setPen(Qt::NoPen);
      painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicHover));
      painter.drawRoundedRect(d->m_CloseButtonRect.adjusted(-2, -2, 2, 2), 3, 3);
    }

    QFont closeFont = QFont(QStringLiteral("NXAwesome"));
    closeFont.setPixelSize(10);
    painter.setFont(closeFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    painter.drawText(d->m_CloseButtonRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Xmark));
  }

  d->m_ActionButtonRects.clear();
  if (!d->m_Actions.isEmpty()) {
    QFont buttonFont = font();
    buttonFont.setPixelSize(12);
    QFontMetrics buttonFm(buttonFont);
    int buttonHeight   = 26;
    int buttonSpacing  = 6;
    int buttonPaddingH = 14;

    int buttonX = rightContentEnd;
    for (int i = d->m_Actions.size() - 1; i >= 0; --i) {
      int textWidth   = buttonFm.horizontalAdvance(d->m_Actions[i].text);
      int buttonWidth = textWidth + buttonPaddingH * 2;
      buttonX -= buttonWidth;
      QRect btnRect(buttonX, contentCenterY - buttonHeight / 2, buttonWidth, buttonHeight);
      d->m_ActionButtonRects.prepend(btnRect);
      buttonX -= buttonSpacing;

      painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
      if (d->m_HoverActionIndex == i) {
        painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicHover));
      } else {
        painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBase));
      }
      painter.drawRoundedRect(btnRect, 4, 4);

      painter.setFont(buttonFont);
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
      painter.drawText(btnRect, Qt::AlignCenter, d->m_Actions[i].text);
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

  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));

  if (!d->m_Title.isEmpty() && !d->m_Message.isEmpty()) {
    QString elidedTitle = titleFm.elidedText(d->m_Title, Qt::ElideRight, textAreaWidth);
    painter.setFont(titleFont);
    int titleWidth = titleFm.horizontalAdvance(elidedTitle);
    painter.drawText(QRect(contentLeft, contentCenterY - 10, titleWidth, 20), Qt::AlignVCenter | Qt::AlignLeft, elidedTitle);

    int messageLeft  = contentLeft + titleWidth + 8;
    int messageWidth = rightContentEnd - messageLeft;
    if (messageWidth > 0) {
      painter.setFont(messageFont);
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
      QString elidedMessage = messageFm.elidedText(d->m_Message, Qt::ElideRight, messageWidth);
      painter.drawText(QRect(messageLeft, contentCenterY - 10, messageWidth, 20), Qt::AlignVCenter | Qt::AlignLeft, elidedMessage);
    }
  } else if (!d->m_Title.isEmpty()) {
    painter.setFont(titleFont);
    QString elidedTitle = titleFm.elidedText(d->m_Title, Qt::ElideRight, textAreaWidth);
    painter.drawText(QRect(contentLeft, contentCenterY - 10, textAreaWidth, 20), Qt::AlignVCenter | Qt::AlignLeft, elidedTitle);
  } else if (!d->m_Message.isEmpty()) {
    painter.setFont(messageFont);
    QString elidedMessage = messageFm.elidedText(d->m_Message, Qt::ElideRight, textAreaWidth);
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

  if (d->m_IsClosable && d->m_CloseButtonRect.contains(event->pos())) {
    closeInfoBar();
    return;
  }

  for (int i = 0; i < d->m_ActionButtonRects.size(); ++i) {
    if (d->m_ActionButtonRects[i].contains(event->pos())) {
      if (d->m_Actions[i].callback) {
        d->m_Actions[i].callback();
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

  if (d->m_IsClosable) {
    bool hover = d->m_CloseButtonRect.contains(event->pos());
    if (hover != d->m_CloseButtonHover) {
      d->m_CloseButtonHover = hover;
      needUpdate           = true;
    }
  }

  int hoverIndex = -1;
  for (int i = 0; i < d->m_ActionButtonRects.size(); ++i) {
    if (d->m_ActionButtonRects[i].contains(event->pos())) {
      hoverIndex = i;
      break;
    }
  }
  if (hoverIndex != d->m_HoverActionIndex) {
    d->m_HoverActionIndex = hoverIndex;
    needUpdate           = true;
  }

  if (needUpdate) {
    setCursor(d->m_CloseButtonHover || hoverIndex >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
    update();
  }
  QFrame::mouseMoveEvent(event);
}

void NXInfoBar::leaveEvent(QEvent *event)
{
  Q_D(NXInfoBar);
  bool needUpdate = false;
  if (d->m_CloseButtonHover) {
    d->m_CloseButtonHover = false;
    needUpdate           = true;
  }
  if (d->m_HoverActionIndex != -1) {
    d->m_HoverActionIndex = -1;
    needUpdate           = true;
  }
  if (needUpdate) {
    setCursor(Qt::ArrowCursor);
    update();
  }
  QFrame::leaveEvent(event);
}
