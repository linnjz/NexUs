#include "NXSplitterHandle.h"

#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"

NXSplitterHandle::NXSplitterHandle(Qt::Orientation orientation, QSplitter *parent)
    : QSplitterHandle(orientation, parent)
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  setMouseTracking(true);
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
    update();
  });
}

NXSplitterHandle::~NXSplitterHandle()
{
}

void NXSplitterHandle::setGripLength(int length)
{
  m_GripLength = length;
  update();
}

int NXSplitterHandle::getGripLength() const
{
  return m_GripLength;
}

void NXSplitterHandle::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);

  QColor lineColor = NXThemeColor(m_ThemeMode, BasicBorder);
  if (orientation() == Qt::Horizontal) {
    int centerX = width() / 2;
    painter.setPen(QPen(lineColor, 1));
    painter.drawLine(centerX, 0, centerX, height());

    int gripWidth = 4;
    int gripY     = (height() - m_GripLength) / 2;
    QRectF gripRect(centerX - gripWidth / 2.0, gripY, gripWidth, m_GripLength);

    QColor gripColor;
    if (m_IsPressed) {
      gripColor = NXThemeColor(m_ThemeMode, PrimaryNormal);
    } else if (m_IsHover) {
      gripColor = NXThemeColor(m_ThemeMode, BasicTextPress);
    } else {
      gripColor = NXThemeColor(m_ThemeMode, BasicBorderDeep);
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(gripColor);
    painter.drawRoundedRect(gripRect, gripWidth / 2.0, gripWidth / 2.0);
  } else {
    int centerY = height() / 2;
    painter.setPen(QPen(lineColor, 1));
    painter.drawLine(0, centerY, width(), centerY);

    int gripHeight = 4;
    int gripX      = (width() - m_GripLength) / 2;
    QRectF gripRect(gripX, centerY - gripHeight / 2.0, m_GripLength, gripHeight);

    QColor gripColor;
    if (m_IsPressed) {
      gripColor = NXThemeColor(m_ThemeMode, PrimaryNormal);
    } else if (m_IsHover) {
      gripColor = NXThemeColor(m_ThemeMode, BasicTextPress);
    } else {
      gripColor = NXThemeColor(m_ThemeMode, BasicBorderDeep);
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
  m_IsHover = true;
  update();
}

void NXSplitterHandle::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  m_IsHover = false;
  update();
}

void NXSplitterHandle::mousePressEvent(QMouseEvent *event)
{
  m_IsPressed = true;
  update();
  QSplitterHandle::mousePressEvent(event);
}

void NXSplitterHandle::mouseReleaseEvent(QMouseEvent *event)
{
  m_IsPressed = false;
  update();
  QSplitterHandle::mouseReleaseEvent(event);
}
