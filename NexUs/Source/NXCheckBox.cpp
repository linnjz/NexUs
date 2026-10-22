#include "NXCheckBox.h"

#include "NXApplication.h"
#include "DeveloperComponents/NXCheckBoxStyle.h"
#include "private/NXCheckBoxPrivate.h"

NXCheckBox::NXCheckBox(QWidget *parent)
    : QCheckBox(parent)
    , d_ptr(new NXCheckBoxPrivate())
{
  Q_D(NXCheckBox);
  d->q_ptr = this;
  setMouseTracking(true);
#ifdef Q_OS_MACOS
  setAttribute(Qt::WA_Hover);
#endif
  setObjectName("NXCheckBox");
  d->_checkBoxStyle = new NXCheckBoxStyle(style());
  setStyle(d->_checkBoxStyle);
  QFont font = this->font();
  font.setPixelSize(nxApp->getFontPixelSize());
  setFont(font);
}

NXCheckBox::NXCheckBox(const QString &text, QWidget *parent)
    : NXCheckBox(parent)
{
  setText(text);
}

NXCheckBox::~NXCheckBox()
{
  delete this->style();
}

void NXCheckBox::setTextPixelSize(int size)
{
  QFont font = this->font();
  font.setPixelSize(size);
  setFont(font);
}

int NXCheckBox::getTextPixelSize() const
{
  return this->font().pixelSize();
}

void NXCheckBox::setTextPointSize(int size)
{
  QFont font = this->font();
  font.setPointSize(size);
  setFont(font);
}

int NXCheckBox::getTextPointSize() const
{
  return this->font().pointSize();
}

void NXCheckBox::setTextStyle(NXTextType::TextStyle textStyle)
{
  Q_D(NXCheckBox);
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

NXTextType::TextStyle NXCheckBox::getTextStyle() const
{
  Q_D(const NXCheckBox);
  return d->_textStyle;
}

void NXCheckBox::setBorderRadius(int borderRadius)
{
  Q_D(const NXCheckBox);
  d->_checkBoxStyle->setBorderRadius(borderRadius);
}

int NXCheckBox::getBorderRadius() const
{
  Q_D(const NXCheckBox);
  return d->_checkBoxStyle->getBorderRadius();
}

void NXCheckBox::setCheckIndicatorWidth(int indicatorWidth)
{
  Q_D(const NXCheckBox);
  d->_checkBoxStyle->setCheckIndicatorWidth(indicatorWidth);
}

int NXCheckBox::getCheckIndicatorWidth() const
{
  Q_D(const NXCheckBox);
  return d->_checkBoxStyle->getCheckIndicatorWidth();
}
