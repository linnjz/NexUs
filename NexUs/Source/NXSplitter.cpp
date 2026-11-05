#include "NXSplitter.h"

#include "DeveloperComponents/NXSplitterHandle.h"
#include "private/NXSplitterPrivate.h"

Q_PROPERTY_CREATE_CPP(NXSplitter, QPS_PAEMIT(int), HandleWidth)
Q_PROPERTY_CREATE_CPP(NXSplitter, QPS_PAEMIT(int), GripLength)

NXSplitter::NXSplitter(QWidget *parent)
    : QSplitter(parent)
    , d_ptr(new NXSplitterPrivate())
{
  Q_D(NXSplitter);
  d->q_ptr         = this;
  d->m_HandleWidth = 6;
  d->m_GripLength  = 36;
  setHandleWidth(d->m_HandleWidth);
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

QSplitterHandle *NXSplitter::createHandle()
{
  Q_D(NXSplitter);
  NXSplitterHandle *handle = new NXSplitterHandle(orientation(), this);
  handle->setGripLength(d->m_GripLength);
  return handle;
}
