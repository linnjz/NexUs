#include "NXRadioButton.h"

#include "NXApplication.h"
#include "DeveloperComponents/NXRadioButtonStyle.h"
#include "NXTheme.h"
#include "private/NXRadioButtonPrivate.h"

NXRadioButton::NXRadioButton(QWidget *parent)
    : QRadioButton(parent)
    , d_ptr(new NXRadioButtonPrivate())
{
  Q_D(NXRadioButton);
  setMouseTracking(true);
  setCursor(Qt::PointingHandCursor);
  d->q_ptr = this;
  setAttribute(Qt::WA_Hover);
  setMouseTracking(true);
  QFont font = this->font();
  font.setPixelSize(nxApp->getFontPixelSize());
  setFont(font);
  setStyle(new NXRadioButtonStyle(style()));
  d->onThemeChanged(nxTheme->getThemeMode());
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXRadioButtonPrivate::onThemeChanged);
}

NXRadioButton::NXRadioButton(const QString &text, QWidget *parent)
    : NXRadioButton(parent)
{
  setText(text);
}

NXRadioButton::~NXRadioButton()
{
  delete this->style();
}

void NXRadioButton::setTextPixelSize(int size)
{
  QFont font = this->font();
  font.setPixelSize(size);
  setFont(font);
}

int NXRadioButton::getTextPixelSize() const
{
  return this->font().pixelSize();
}

void NXRadioButton::setTextPointSize(int size)
{
  QFont font = this->font();
  font.setPointSize(size);
  setFont(font);
}

int NXRadioButton::getTextPointSize() const
{
  return this->font().pointSize();
}

void NXRadioButton::setTextStyle(NXTextType::TextStyle textStyle)
{
  Q_D(NXRadioButton);
  QFont textFont = font();
  d->_textStyle  = textStyle;
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
    break;
  }
  }
  setFont(textFont);
}

NXTextType::TextStyle NXRadioButton::getTextStyle() const
{
  Q_D(const NXRadioButton);
  return d->_textStyle;
}

void NXRadioButton::paintEvent(QPaintEvent *event)
{
  Q_D(NXRadioButton);
  if (palette().color(QPalette::WindowText) != NXThemeColor(d->_themeMode, BasicText)) {
    d->onThemeChanged(d->_themeMode);
  }
  QRadioButton::paintEvent(event);
}
