#include "NXSplitterHandle.h"

#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"

NXSplitterHandle::NXSplitterHandle(Qt::Orientation orientation, QSplitter *parent)
    : QSplitterHandle(orientation, parent)
{
  setMouseTracking(true);
  _themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
    update();
  });
}

NXSplitterHandle::~NXSplitterHandle()
{
}

void NXSplitterHandle::setGripLength(int length)
{
  _gripLength = length;
  update();
}

int NXSplitterHandle::getGripLength() const
{
  return _gripLength;
}

void NXSplitterHandle::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);

  QColor lineColor = NXThemeColor(_themeMode, BasicBorder);
  if (orientation() == Qt::Horizontal) {
    int centerX = width() / 2;
    painter.setPen(QPen(lineColor, 1));
    painter.drawLine(centerX, 0, centerX, height());

    int gripWidth = 4;
    int gripY     = (height() - _gripLength) / 2;
    QRectF gripRect(centerX - gripWidth / 2.0, gripY, gripWidth, _gripLength);

    QColor gripColor;
    if (_isPressed) {
      gripColor = NXThemeColor(_themeMode, PrimaryNormal);
    } else if (_isHover) {
      gripColor = NXThemeColor(_themeMode, BasicTextPress);
    } else {
      gripColor = NXThemeColor(_themeMode, BasicBorderDeep);
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(gripColor);
    painter.drawRoundedRect(gripRect, gripWidth / 2.0, gripWidth / 2.0);
  } else {
    int centerY = height() / 2;
    painter.setPen(QPen(lineColor, 1));
    painter.drawLine(0, centerY, width(), centerY);

    int gripHeight = 4;
    int gripX      = (width() - _gripLength) / 2;
    QRectF gripRect(gripX, centerY - gripHeight / 2.0, _gripLength, gripHeight);

    QColor gripColor;
    if (_isPressed) {
      gripColor = NXThemeColor(_themeMode, PrimaryNormal);
    } else if (_isHover) {
      gripColor = NXThemeColor(_themeMode, BasicTextPress);
    } else {
      gripColor = NXThemeColor(_themeMode, BasicBorderDeep);
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(gripColor);
    painter.drawRoundedRect(gripRect, gripHeight / 2.0, gripHeight / 2.0);
  }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void NXSplitterHandle::enterEvent(QEnterEvent *event)
#else
void NXSplitterHandle::enterEvent(QEvent *event)
#endif
{
  Q_UNUSED(event);
  _isHover = true;
  update();
}

void NXSplitterHandle::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  _isHover = false;
  update();
}

void NXSplitterHandle::mousePressEvent(QMouseEvent *event)
{
  _isPressed = true;
  update();
  QSplitterHandle::mousePressEvent(event);
}

void NXSplitterHandle::mouseReleaseEvent(QMouseEvent *event)
{
  _isPressed = false;
  update();
  QSplitterHandle::mouseReleaseEvent(event);
}
