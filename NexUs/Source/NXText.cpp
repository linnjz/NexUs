#include "NXText.h"

#include <QContextMenuEvent>
#include <QPainter>

#include "NXTheme.h"
#include "private/NXTextPrivate.h"
Q_PROPERTY_CREATE_CPP(NXText, bool, IsClickEnable)

NXText::NXText(QWidget *parent)
    : QLabel(parent)
    , d_ptr(new NXTextPrivate())
{
  Q_D(NXText);
  d->q_ptr            = this;
  d->_pIsClickEnable  = false;
  d->_pIsWrapAnywhere = false;
  d->_pTextStyle      = NXTextType::NoStyle;
  d->_pNXIcon         = NXIconType::None;

  setObjectName("NXText");
  setStyleSheet(QStringLiteral("#NXText{background-color:transparent;}"));
  QFont textFont = font();
  textFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
  textFont.setPixelSize(28);
  textFont.setBold(false);
  setFont(textFont);
  setWordWrap(true);
  d->_themeMode = nxTheme->getThemeMode();
  d->onThemeChanged(nxTheme->getThemeMode());
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXTextPrivate::onThemeChanged);
}

NXText::NXText(QString text, QWidget *parent)
    : NXText(parent)
{
  setText(text);
}

NXText::NXText(QString text, int pixelSize, QWidget *parent)
    : NXText(text, parent)
{
  QFont font = this->font();
  font.setPixelSize(pixelSize);
  setFont(font);
}

NXText::~NXText()
{
}

void NXText::setIsBold(bool isBold)
{
  QFont textFont = font();
  textFont.setBold(isBold);
  setFont(textFont);
  Q_EMIT pIsBoldChanged();
}

bool NXText::getIsBold() const
{
  return font().bold();
}

void NXText::setIsWrapAnywhere(bool isWrapAnywhere)
{
  Q_D(NXText);
  setWordWrap(isWrapAnywhere);
  d->_pIsWrapAnywhere = isWrapAnywhere;
}

bool NXText::getIsWrapAnywhere() const
{
  Q_D(const NXText);
  return d->_pIsWrapAnywhere;
}

void NXText::setTextPixelSize(int size)
{
  QFont font = this->font();
  font.setPixelSize(size);
  setFont(font);
}

int NXText::getTextPixelSize() const
{
  return this->font().pixelSize();
}

void NXText::setTextPointSize(int size)
{
  QFont font = this->font();
  font.setPointSize(size);
  setFont(font);
}

int NXText::getTextPointSize() const
{
  return this->font().pointSize();
}

void NXText::setTextStyle(NXTextType::TextStyle textStyle)
{
  Q_D(NXText);
  QFont textFont = font();
  d->_pTextStyle = textStyle;
  switch (textStyle) {
  case NXTextType::NoStyle : {
    break;
  }
  case NXTextType::Caption : {
    textFont.setPixelSize(12);
    break;
  }
  case NXTextType::Body : {
    textFont.setPixelSize(13);
    break;
  }
  case NXTextType::BodyStrong : {
    textFont.setPixelSize(13);
    textFont.setWeight(QFont::DemiBold);
    break;
  }
  case NXTextType::Subtitle : {
    textFont.setPixelSize(20);
    textFont.setWeight(QFont::DemiBold);
    break;
  }
  case NXTextType::Title : {
    textFont.setPixelSize(28);
    textFont.setWeight(QFont::DemiBold);
    break;
  }
  case NXTextType::TitleLarge : {
    textFont.setPixelSize(40);
    textFont.setWeight(QFont::DemiBold);
    break;
  }
  case NXTextType::Display : {
    textFont.setPixelSize(48);
    textFont.setWeight(QFont::DemiBold);
    break;
  }
  default : {
    Q_UNREACHABLE();
  }
  }
  setFont(textFont);
}

NXTextType::TextStyle NXText::getTextStyle() const
{
  Q_D(const NXText);
  return d->_pTextStyle;
}

void NXText::setNXIcon(NXIconType::IconName icon)
{
  Q_D(NXText);
  d->_pNXIcon = icon;
  update();
  Q_EMIT pNXIconChanged();
}

NXIconType::IconName NXText::getNXIcon() const
{
  Q_D(const NXText);
  return d->_pNXIcon;
}

void NXText::mouseReleaseEvent(QMouseEvent *event)
{
  if (d_ptr->_pIsClickEnable && event->button() == Qt::LeftButton) {
    Q_EMIT clicked();
  }
  QLabel::mouseReleaseEvent(event);
}

void NXText::enterEvent(QEnterEvent *event)
{
  if (d_ptr->_pIsClickEnable) {
    setCursor(QCursor(Qt::PointingHandCursor));
  }
  QLabel::enterEvent(event);
}

void NXText::leaveEvent(QEvent *event)
{
  if (d_ptr->_pIsClickEnable) {
    setCursor(QCursor(Qt::ArrowCursor));
  }
  QLabel::leaveEvent(event);
}

void NXText::paintEvent(QPaintEvent *event)
{
  Q_D(NXText);
  if (palette().color(QPalette::WindowText) != NXThemeColor(d->_themeMode, BasicText)) {
    d->onThemeChanged(d->_themeMode);
  }
  if (d->_pNXIcon != NXIconType::None) {
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);

    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(this->font().pixelSize());
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicText));

    QChar iconChar = QChar(d->_pNXIcon);
    QFontMetrics iconFm(iconFont);
    int iconWidth = iconFm.horizontalAdvance(iconChar);

    QString displayText = text();
    if (displayText.isEmpty()) {
      QRect iconRect(0, 0, iconWidth, height());
      iconRect.moveLeft((width() - iconWidth) / 2);
      painter.drawText(iconRect, Qt::AlignVCenter | Qt::AlignLeft, iconChar);
      painter.restore();
      return;
    }

    QFont textFont = this->font();
    painter.setFont(textFont);
    QFontMetrics textFm(textFont);
    int textWidth = textFm.horizontalAdvance(displayText);

    const int spacing = 8;
    int overallWidth  = iconWidth + spacing + textWidth;
    int startX        = (width() - overallWidth) / 2;
    if (startX < 0)
      startX = 0;

    QRect iconRect(startX, 0, iconWidth, height());
    painter.setFont(iconFont);
    painter.drawText(iconRect, Qt::AlignVCenter | Qt::AlignLeft, iconChar);

    int textFlags = Qt::AlignVCenter | Qt::AlignLeft;
    if (wordWrap())
      textFlags |= Qt::TextWordWrap;
    if (d->_pIsWrapAnywhere)
      textFlags |= Qt::TextWrapAnywhere;

    QRect textRect(startX + iconWidth + spacing, 0, width() - (startX + iconWidth + spacing), height());
    painter.setFont(textFont);
    painter.drawText(textRect, textFlags, displayText);

    painter.restore();
  } else {
    if (wordWrap() && d->_pIsWrapAnywhere) {
      QPainter painter(this);
      painter.save();
      painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
      painter.setPen(NXThemeColor(d->_themeMode, BasicText));
      painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap | Qt::TextWrapAnywhere, text());
      painter.restore();
    } else {
      QLabel::paintEvent(event);
    }
  }
}
