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
  if (m_IsClosing) {
    return;
  }
  m_IsClosing    = true;
  m_ActiveButton = triggeredButton;
  Q_Q(NXContentDialog);
  connect(m_MaskWidget, &NXMaskWidget::animationFinished, this, &NXContentDialogPrivate::_handleCloseAnimation);
  m_MaskWidget->doMaskAnimation(0);
}

void NXContentDialogPrivate::_handleCloseAnimation()
{
  m_IsClosing = false;
  Q_Q(NXContentDialog);
  disconnect(m_MaskWidget, &NXMaskWidget::animationFinished, this, &NXContentDialogPrivate::_handleCloseAnimation);
  if (m_ActiveButton) {
    Q_EMIT q->buttonClicked(m_ActiveButton);
  }
  q->done(q->result());
}

void NXContentDialogPrivate::_moveToCenter()
{
  Q_Q(NXContentDialog);
  int width      = q->width();
  int height     = q->height();
  auto globalPos = m_MaskWidget->mapToGlobal(QPoint { 0, 0 });
  q->setGeometry(globalPos.x() + (m_MaskWidget->width() - width) / 2, globalPos.y() + (m_MaskWidget->height() - height) / 2, width, height);
}
