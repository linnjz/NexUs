#include "NXContentDialogPrivate.h"

#include <QApplication>
#include <QDebug>
#include <QScreen>
#include <QWindow>

#include "DeveloperComponents/NXMaskWidget.h"
#include "NXContentDialog.h"

NXContentDialogPrivate::NXContentDialogPrivate(QObject *parent)
    : QObject { parent }
{
}

NXContentDialogPrivate::~NXContentDialogPrivate()
{
}

void NXContentDialogPrivate::startCloseAnimation(NXPushButton *triggeredButton)
{
  if (_isClosing) {
    return;
  }
  _isClosing    = true;
  _activeButton = triggeredButton;
  Q_Q(NXContentDialog);
  connect(_maskWidget, &NXMaskWidget::animationFinished, this, &NXContentDialogPrivate::_handleCloseAnimation);
  _maskWidget->doMaskAnimation(0);
}

void NXContentDialogPrivate::_handleCloseAnimation()
{
  _isClosing = false;
  Q_Q(NXContentDialog);
  disconnect(_maskWidget, &NXMaskWidget::animationFinished, this, &NXContentDialogPrivate::_handleCloseAnimation);
  if (_activeButton) {
    Q_EMIT q->buttonClicked(_activeButton);
  }
  q->done(q->result());
}

void NXContentDialogPrivate::_moveToCenter()
{
  Q_Q(NXContentDialog);
  int width      = q->width();
  int height     = q->height();
  auto globalPos = _maskWidget->mapToGlobal(QPoint { 0, 0 });
  q->setGeometry(globalPos.x() + (_maskWidget->width() - width) / 2, globalPos.y() + (_maskWidget->height() - height) / 2, width, height);
}
