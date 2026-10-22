#include "NXAutoCompleteContainer.h"

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>

#include "NXLineEdit.h"
#include "NXTheme.h"

NXAutoCompleteContainer::NXAutoCompleteContainer(QWidget *parent)
    : QWidget(parent)
{
  setContentsMargins(4, 4, 4, 4);
  setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  _themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
    update();
  });
}

NXAutoCompleteContainer::~NXAutoCompleteContainer()
{
}

void NXAutoCompleteContainer::setForwardTarget(NXLineEdit *target)
{
  _forwardTarget = target;
}

void NXAutoCompleteContainer::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);

  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(QRectF(0.5, 0.5, width() - 1, height() - 1), 6, 6);
  painter.fillPath(backgroundPath, NXThemeColor(_themeMode, DialogBase));

  painter.setPen(QPen(NXThemeColor(_themeMode, BasicBorder), 1));
  painter.drawPath(backgroundPath);
}

void NXAutoCompleteContainer::keyPressEvent(QKeyEvent *event)
{
  if (_forwardTarget) {
    if (event->key() == Qt::Key_Escape) {
      hide();
      _forwardTarget->setFocus();
      return;
    }
    if (event->key() != Qt::Key_Up && event->key() != Qt::Key_Down && event->key() != Qt::Key_Enter &&
        event->key() != Qt::Key_Return) {
      QApplication::sendEvent(_forwardTarget, event);
      return;
    }
  }
  QWidget::keyPressEvent(event);
}

void NXAutoCompleteContainer::hideEvent(QHideEvent *event)
{
  QWidget::hideEvent(event);
  Q_EMIT popupClosed();
}
