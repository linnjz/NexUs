#include "NXTag.h"

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXTagPrivate.h"

Q_PROPERTY_CREATE_CPP(NXTag, QPS_SCREF_PAEMIT(QString), TagText)
Q_PROPERTY_CREATE_CPP(NXTag, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXTag, QPS_PAEMIT(bool), IsClosable)
Q_PROPERTY_CREATE_CPP(NXTag, QPS_PAEMIT(bool), IsCheckable)
Q_PROPERTY_CREATE_CPP(NXTag, QPS_PAEMIT(bool), IsChecked)

NXTagPrivate::NXTagPrivate(QObject *parent)
    : QObject(parent)
{
}

NXTagPrivate::~NXTagPrivate()
{
}

QColor NXTagPrivate::_getBackgroundColor() const
{
  switch (m_TagColor) {
  case NXTag::Default : return NXThemeColor(m_ThemeMode, BasicBase);
  case NXTag::Primary : {
    QColor c = NXThemeColor(m_ThemeMode, PrimaryNormal);
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
    QColor c = NXThemeColor(m_ThemeMode, StatusDanger);
    c.setAlpha(30);
    return c;
  }
  }
  return NXThemeColor(m_ThemeMode, BasicBase);
}

QColor NXTagPrivate::_getForegroundColor() const
{
  switch (m_TagColor) {
  case NXTag::Default : return NXThemeColor(m_ThemeMode, BasicText);
  case NXTag::Primary : return NXThemeColor(m_ThemeMode, PrimaryNormal);
  case NXTag::Success : return QColor(0x0F, 0x7B, 0x0F);
  case NXTag::Warning : return QColor(0xF7, 0x93, 0x0E);
  case NXTag::Danger  : return NXThemeColor(m_ThemeMode, StatusDanger);
  }
  return NXThemeColor(m_ThemeMode, BasicText);
}

NXTag::NXTag(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXTagPrivate())
{
  Q_D(NXTag);
  d->q_ptr          = this;
  d->m_BorderRadius = 4;
  d->m_TagText      = QStringLiteral("");
  d->m_IsClosable   = false;
  d->m_IsCheckable  = false;
  d->m_IsChecked    = false;
  setObjectName("NXTag");
  setFixedHeight(28);
  setMouseTracking(true);
  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
  connect(this, &NXTag::signalTagTextChanged, this, [=]() {
    updateGeometry();
    adjustSize();
    update();
  });
  connect(this, &NXTag::signalIsClosableChanged, this, [=]() {
    updateGeometry();
    adjustSize();
    update();
  });
  connect(this, &NXTag::signalIsCheckableChanged, this, [=]() {
    updateGeometry();
    adjustSize();
    update();
  });
  connect(this, &NXTag::signalIsCheckedChanged, this, [=]() {
    updateGeometry();
    adjustSize();
    update();
  });
}

NXTag::NXTag(QString const &text, QWidget *parent)
    : NXTag(parent)
{
  Q_D(NXTag);
  d->m_TagText = text;
}

NXTag::~NXTag()
{
}

void NXTag::setTagColor(TagColor color)
{
  Q_D(NXTag);
  d->m_TagColor = color;
  update();
}

NXTag::TagColor NXTag::getTagColor() const
{
  return d_ptr->m_TagColor;
}

void NXTag::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXTag);
  if (event->button() == Qt::LeftButton) {
    if (d->m_IsClosable) {
      // Check if click is in the close button area (right side)
      int closeAreaX = width() - d->m_CloseButtonWidth - d->m_Padding / 2;
      if (event->pos().x() >= closeAreaX) {
        Q_EMIT closed();
        return;
      }
    }
    if (d->m_IsCheckable) {
      setIsChecked(!d->m_IsChecked);
      Q_EMIT checkedChanged(d->m_IsChecked);
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
  if (d->m_IsCheckable && d->m_IsChecked) {
    QColor primaryColor = NXThemeColor(d->m_ThemeMode, PrimaryNormal);
    bgColor             = primaryColor;
    bgColor.setAlpha(30);
    fgColor = primaryColor;
  }

  // Draw background rounded rectangle
  painter.setPen(Qt::NoPen);
  painter.setBrush(bgColor);
  QPainterPath path;
  path.addRoundedRect(QRectF(0, 0, width(), height()), d->m_BorderRadius, d->m_BorderRadius);
  painter.drawPath(path);

  // Calculate text draw area
  int xOffset = d->m_Padding;

  // Draw check mark if checkable and checked
  if (d->m_IsCheckable && d->m_IsChecked) {
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(12);
    painter.setFont(iconFont);
    painter.setPen(fgColor);
    QRectF checkRect(xOffset, 0, d->m_CheckIconWidth, height());
    painter.drawText(checkRect, Qt::AlignCenter, QChar((unsigned short) 0xea'6c));
    xOffset += d->m_CheckIconWidth;
  }

  // Draw text
  QFont textFont = this->font();
  textFont.setPixelSize(13);
  painter.setFont(textFont);
  painter.setPen(fgColor);
  int textWidth = width() - xOffset - d->m_Padding;
  if (d->m_IsClosable) {
    textWidth -= d->m_CloseButtonWidth;
  }
  QRectF textRect(xOffset, 0, textWidth, height());
  painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, d->m_TagText);

  // Draw close button if closable
  if (d->m_IsClosable) {
    QFont iconFont = QFont("NXAwesome");
    iconFont.setPixelSize(10);
    painter.setFont(iconFont);
    painter.setPen(fgColor);
    int closeX = width() - d->m_CloseButtonWidth - d->m_Padding / 2;
    QRectF closeRect(closeX, 0, d->m_CloseButtonWidth, height());
    painter.drawText(closeRect, Qt::AlignCenter, QChar((unsigned short) 0xf4'ce));
  }
}

QSize NXTag::sizeHint() const
{
  Q_CONST_D(NXTag);
  QFont textFont = this->font();
  textFont.setPixelSize(13);
  QFontMetrics fm(textFont);
  int textWidth = fm.horizontalAdvance(d->m_TagText);
  int w         = textWidth + d->m_Padding * 2;
  if (d->m_IsClosable) {
    w += d->m_CloseButtonWidth;
  }
  if (d->m_IsCheckable && d->m_IsChecked) {
    w += d->m_CheckIconWidth;
  }
  return QSize(qMax(w, 32), 28);
}
