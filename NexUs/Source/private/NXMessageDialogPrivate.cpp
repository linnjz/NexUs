#include "NXMessageDialogPrivate.h"

#include <QMouseEvent>
#include <QPainter>

#include "NXTheme.h"

NXMessageDialogButton::NXMessageDialogButton(ButtonType type, QWidget *parent)
    : QWidget { parent }
    , m_ButtonType { type }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
    update();
  });
  setCursor(Qt::PointingHandCursor);
}

NXMessageDialogButton::~NXMessageDialogButton()
{
}

void NXMessageDialogButton::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

  if (m_IsPressed) {
    painter.fillRect(rect(), QColor(0, 0, 0, 20));
  } else if (m_IsHovered) {
    painter.fillRect(rect(), QColor(0, 0, 0, 10));
  }

  int centerX = width() / 2;
  int centerY = height() / 2;

  painter.setPen(QPen(NXThemeColor(m_ThemeMode, BasicText), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

  if (m_ButtonType == Confirm) {
    QPoint p1(centerX - 6, centerY);
    QPoint p2(centerX - 2, centerY + 4);
    QPoint p3(centerX + 6, centerY - 4);

    painter.drawLine(p1, p2);
    painter.drawLine(p2, p3);
  } else {
    int offset = 5;
    painter.drawLine(centerX - offset, centerY - offset, centerX + offset, centerY + offset);
    painter.drawLine(centerX - offset, centerY + offset, centerX + offset, centerY - offset);
  }
}

void NXMessageDialogButton::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
    m_IsPressed = true;
    update();
  }
  QWidget::mousePressEvent(event);
}

void NXMessageDialogButton::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton && m_IsPressed) {
    m_IsPressed = false;
    update();
    if (rect().contains(event->pos())) {
      Q_EMIT clicked();
    }
  }
  QWidget::mouseReleaseEvent(event);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void NXMessageDialogButton::enterEvent(QEnterEvent *event)
#else
void NXMessageDialogButton::enterEvent(QEvent *event)
#endif
{
  m_IsHovered = true;
  update();
  QWidget::enterEvent(event);
}

void NXMessageDialogButton::leaveEvent(QEvent *event)
{
  m_IsHovered = false;
  m_IsPressed = false;
  update();
  QWidget::leaveEvent(event);
}

NXMessageDialogPrivate::NXMessageDialogPrivate(QObject *parent)
    : QObject(parent)
{
}

NXMessageDialogPrivate::~NXMessageDialogPrivate()
{
}
