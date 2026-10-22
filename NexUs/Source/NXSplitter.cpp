#include "NXSplitter.h"

#include "DeveloperComponents/NXSplitterHandle.h"
#include "private/NXSplitterPrivate.h"

Q_PROPERTY_CREATE_CPP(NXSplitter, int, HandleWidth)
Q_PROPERTY_CREATE_CPP(NXSplitter, int, GripLength)

NXSplitter::NXSplitter(QWidget *parent)
    : QSplitter(parent)
    , d_ptr(new NXSplitterPrivate())
{
  Q_D(NXSplitter);
  d->q_ptr         = this;
  d->_pHandleWidth = 6;
  d->_pGripLength  = 36;
  setHandleWidth(d->_pHandleWidth);
  setChildrenCollapsible(false);
  setStyleSheet(QStringLiteral("QSplitter{background-color:transparent;}"));
}

NXSplitter::NXSplitter(Qt::Orientation orientation, QWidget *parent)
    : NXSplitter(parent)
{
  setOrientation(orientation);
}

NXSplitter::~NXSplitter()
{
}

QSplitterHandle * NXSplitter::createHandle()
{
  Q_D(NXSplitter);
  NXSplitterHandle *handle = new NXSplitterHandle(orientation(), this);
  handle->setGripLength(d->_pGripLength);
  return handle;
}
