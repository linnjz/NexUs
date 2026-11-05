#include "NXText.h"

#include <QContextMenuEvent>
#include <QPainter>

#include "NXTheme.h"
#include "private/NXTextPrivate.h"
Q_PROPERTY_CREATE_CPP(NXText, QPS_PAEMIT(bool), IsClickEnable)

NXText::NXText(QWidget *parent)
    : QLabel(parent)
    , d_ptr(new NXTextPrivate())
{
  Q_D(NXText);
  d->q_ptr            = this;
  d->m_IsClickEnable  = false;
  d->m_IsWrapAnywhere = false;
  d->m_TextStyle      = NXTextType::NoStyle;
  d->m_NXIcon         = NXIconType::None;

  setObjectName("NXText");
  setStyleSheet(QStringLiteral("#NXText{background-color:transparent;}"));
  QFont textFont = font();
  textFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
  textFont.setPixelSize(28);
  textFont.setBold(false);
  setFont(textFont);
  setWordWrap(true);
  d->m_ThemeMode = nxTheme->getThemeMode();
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
  Q_EMIT signalIsBoldChanged();
}

bool NXText::getIsBold() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  return font().bold();
}

void NXText::setIsWrapAnywhere(bool isWrapAnywhere)
{
  Q_D(NXText);
  setWordWrap(isWrapAnywhere);
  d->m_IsWrapAnywhere = isWrapAnywhere;
}

bool NXText::getIsWrapAnywhere() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXText);
  return d->m_IsWrapAnywhere;
}

void NXText::setTextPixelSize(int size)
{
  QFont font = this->font();
  font.setPixelSize(size);
  setFont(font);
}

int NXText::getTextPixelSize() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  return this->font().pixelSize();
}

void NXText::setTextPointSize(int size)
{
  QFont font = this->font();
  font.setPointSize(size);
  setFont(font);
}

int NXText::getTextPointSize() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  return this->font().pointSize();
}

void NXText::setTextStyle(NXTextType::TextStyle textStyle)
{
  Q_D(NXText);
  QFont textFont = font();
  d->m_TextStyle = textStyle;
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

NXTextType::TextStyle NXText::getTextStyle() const noexcept(std::is_nothrow_copy_constructible_v<NXTextType::TextStyle>)
{
  Q_CONST_D(NXText);
  return d->m_TextStyle;
}

void NXText::setNXIcon(NXIconType::IconName icon)
{
  Q_D(NXText);
  d->m_NXIcon = icon;
  update();
  Q_EMIT signalNXIconChanged();
}

NXIconType::IconName NXText::getNXIcon() const noexcept(std::is_nothrow_copy_constructible_v<NXIconType::IconName>)
{
  Q_CONST_D(NXText);
  return d->m_NXIcon;
}

void NXText::mouseReleaseEvent(QMouseEvent *event)
{
  if (d_ptr->m_IsClickEnable && event->button() == Qt::LeftButton) {
    Q_EMIT clicked();
  }
  QLabel::mouseReleaseEvent(event);
}

void NXText::enterEvent(QEnterEvent *event)
{
  if (d_ptr->m_IsClickEnable) {
    setCursor(QCursor(Qt::PointingHandCursor));
  }
  QLabel::enterEvent(event);
}

void NXText::leaveEvent(QEvent *event)
{
  if (d_ptr->m_IsClickEnable) {
    setCursor(QCursor(Qt::ArrowCursor));
  }
  QLabel::leaveEvent(event);
}

void NXText::paintEvent(QPaintEvent *event)
{
  Q_D(NXText);
  if (palette().color(QPalette::WindowText) != NXThemeColor(d->m_ThemeMode, BasicText)) {
    d->onThemeChanged(d->m_ThemeMode);
  }
  if (d->m_NXIcon != NXIconType::None) {
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);

    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(this->font().pixelSize());
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));

    QChar iconChar = QChar(d->m_NXIcon);
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

    int const spacing = 8;
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
    if (d->m_IsWrapAnywhere)
      textFlags |= Qt::TextWrapAnywhere;

    QRect textRect(startX + iconWidth + spacing, 0, width() - (startX + iconWidth + spacing), height());
    painter.setFont(textFont);
    painter.drawText(textRect, textFlags, displayText);

    painter.restore();
  } else {
    if (wordWrap() && d->m_IsWrapAnywhere) {
      QPainter painter(this);
      painter.save();
      painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
      painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap | Qt::TextWrapAnywhere, text());
      painter.restore();
    } else {
      QLabel::paintEvent(event);
    }
  }
}
