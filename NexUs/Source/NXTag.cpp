#include "NXTag.h"

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXTagPrivate.h"

Q_PROPERTY_CREATE_CPP(NXTag, QS_SET_CREF(QString), TagText)
Q_PROPERTY_CREATE_CPP(NXTag, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXTag, bool, IsClosable)
Q_PROPERTY_CREATE_CPP(NXTag, bool, IsCheckable)
Q_PROPERTY_CREATE_CPP(NXTag, bool, IsChecked)

NXTagPrivate::NXTagPrivate(QObject *parent)
    : QObject(parent)
{
}

NXTagPrivate::~NXTagPrivate()
{
}

QColor NXTagPrivate::_getBackgroundColor() const
{
  switch (_tagColor) {
  case NXTag::Default : return NXThemeColor(_themeMode, BasicBase);
  case NXTag::Primary : {
    QColor c = NXThemeColor(_themeMode, PrimaryNormal);
    c.setAlpha(30);
    return c;
  }
  case NXTag::Success : {
    QColor c(0x0F, 0x7B, 0x0F);
    c.setAlpha(30);
    return c;
  }
  case NXTag::Warning : {
    QColor c(0xF7, 0x93, 0x0E);
    c.setAlpha(30);
    return c;
  }
  case NXTag::Danger : {
    QColor c = NXThemeColor(_themeMode, StatusDanger);
    c.setAlpha(30);
    return c;
  }
  }
  return NXThemeColor(_themeMode, BasicBase);
}

QColor NXTagPrivate::_getForegroundColor() const
{
  switch (_tagColor) {
  case NXTag::Default : return NXThemeColor(_themeMode, BasicText);
  case NXTag::Primary : return NXThemeColor(_themeMode, PrimaryNormal);
  case NXTag::Success : return QColor(0x0F, 0x7B, 0x0F);
  case NXTag::Warning : return QColor(0xF7, 0x93, 0x0E);
  case NXTag::Danger  : return NXThemeColor(_themeMode, StatusDanger);
  }
  return NXThemeColor(_themeMode, BasicText);
}

NXTag::NXTag(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXTagPrivate())
{
  Q_D(NXTag);
  d->q_ptr          = this;
  d->_pBorderRadius = 4;
  d->_pTagText      = QStringLiteral("");
  d->_pIsClosable   = false;
  d->_pIsCheckable  = false;
  d->_pIsChecked    = false;
  setObjectName("NXTag");
  setFixedHeight(28);
  setMouseTracking(true);
  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
  connect(this, &NXTag::pTagTextChanged, this, [=]() {
    updateGeometry();
    adjustSize();
    update();
  });
  connect(this, &NXTag::pIsClosableChanged, this, [=]() {
    updateGeometry();
    adjustSize();
    update();
  });
  connect(this, &NXTag::pIsCheckableChanged, this, [=]() {
    updateGeometry();
    adjustSize();
    update();
  });
  connect(this, &NXTag::pIsCheckedChanged, this, [=]() {
    updateGeometry();
    adjustSize();
    update();
  });
}

NXTag::NXTag(const QString &text, QWidget *parent)
    : NXTag(parent)
{
  Q_D(NXTag);
  d->_pTagText = text;
}

NXTag::~NXTag()
{
}

void NXTag::setTagColor(TagColor color)
{
  Q_D(NXTag);
  d->_tagColor = color;
  update();
}

NXTag::TagColor NXTag::getTagColor() const
{
  return d_ptr->_tagColor;
}

void NXTag::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXTag);
  if (event->button() == Qt::LeftButton) {
    if (d->_pIsClosable) {
      // Check if click is in the close button area (right side)
      int closeAreaX = width() - d->_closeButtonWidth - d->_padding / 2;
      if (event->pos().x() >= closeAreaX) {
        Q_EMIT closed();
        return;
      }
    }
    if (d->_pIsCheckable) {
      setIsChecked(!d->_pIsChecked);
      Q_EMIT checkedChanged(d->_pIsChecked);
    }
    Q_EMIT clicked();
  }
  QWidget::mousePressEvent(event);
}

void NXTag::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXTag);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QColor bgColor = d->_getBackgroundColor();
  QColor fgColor = d->_getForegroundColor();

  // When checkable and checked, override to primary color
  if (d->_pIsCheckable && d->_pIsChecked) {
    QColor primaryColor = NXThemeColor(d->_themeMode, PrimaryNormal);
    bgColor             = primaryColor;
    bgColor.setAlpha(30);
    fgColor = primaryColor;
  }

  // Draw background rounded rectangle
  painter.setPen(Qt::NoPen);
  painter.setBrush(bgColor);
  QPainterPath path;
  path.addRoundedRect(QRectF(0, 0, width(), height()), d->_pBorderRadius, d->_pBorderRadius);
  painter.drawPath(path);

  // Calculate text draw area
  int xOffset = d->_padding;

  // Draw check mark if checkable and checked
  if (d->_pIsCheckable && d->_pIsChecked) {
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(12);
    painter.setFont(iconFont);
    painter.setPen(fgColor);
    QRectF checkRect(xOffset, 0, d->_checkIconWidth, height());
    painter.drawText(checkRect, Qt::AlignCenter, QChar((unsigned short) 0xea'6c));
    xOffset += d->_checkIconWidth;
  }

  // Draw text
  QFont textFont = this->font();
  textFont.setPixelSize(13);
  painter.setFont(textFont);
  painter.setPen(fgColor);
  int textWidth = width() - xOffset - d->_padding;
  if (d->_pIsClosable) {
    textWidth -= d->_closeButtonWidth;
  }
  QRectF textRect(xOffset, 0, textWidth, height());
  painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, d->_pTagText);

  // Draw close button if closable
  if (d->_pIsClosable) {
    QFont iconFont = QFont("NXAwesome");
    iconFont.setPixelSize(10);
    painter.setFont(iconFont);
    painter.setPen(fgColor);
    int closeX = width() - d->_closeButtonWidth - d->_padding / 2;
    QRectF closeRect(closeX, 0, d->_closeButtonWidth, height());
    painter.drawText(closeRect, Qt::AlignCenter, QChar((unsigned short) 0xf4'ce));
  }
}

QSize NXTag::sizeHint() const
{
  Q_D(const NXTag);
  QFont textFont = this->font();
  textFont.setPixelSize(13);
  QFontMetrics fm(textFont);
  int textWidth = fm.horizontalAdvance(d->_pTagText);
  int w         = textWidth + d->_padding * 2;
  if (d->_pIsClosable) {
    w += d->_closeButtonWidth;
  }
  if (d->_pIsCheckable && d->_pIsChecked) {
    w += d->_checkIconWidth;
  }
  return QSize(qMax(w, 32), 28);
}
