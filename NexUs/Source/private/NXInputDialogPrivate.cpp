#include "NXInputDialogPrivate.h"

#include "DeveloperComponents/NXMaskWidget.h"
#include "NXInputDialog.h"

NXInputDialogPrivate::NXInputDialogPrivate(QObject *parent)
    : QObject { parent }
{
}

NXInputDialogPrivate::~NXInputDialogPrivate()
{
}

void NXInputDialogPrivate::_doCloseAnimation(bool isAccept)
{
  Q_Q(NXInputDialog);
  _maskWidget->doMaskAnimation(0);
  isAccept ? q->accept() : q->reject();
}
