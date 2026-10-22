#include "NXMessageDialogPrivate.h"

#include <QMouseEvent>
#include <QPainter>

#include "NXTheme.h"

NXMessageDialogButton::NXMessageDialogButton(ButtonType type, QWidget *parent)
    : QWidget(parent)
    , _buttonType(type)
{
  _themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
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

  if (_isPressed) {
    painter.fillRect(rect(), QColor(0, 0, 0, 20));
  } else if (_isHovered) {
    painter.fillRect(rect(), QColor(0, 0, 0, 10));
  }

  int centerX = width() / 2;
  int centerY = height() / 2;

  painter.setPen(QPen(NXThemeColor(_themeMode, BasicText), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

  if (_buttonType == Confirm) {
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
    _isPressed = true;
    update();
  }
  QWidget::mousePressEvent(event);
}

void NXMessageDialogButton::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton && _isPressed) {
    _isPressed = false;
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
  _isHovered = true;
  update();
  QWidget::enterEvent(event);
}

void NXMessageDialogButton::leaveEvent(QEvent *event)
{
  _isHovered = false;
  _isPressed = false;
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
