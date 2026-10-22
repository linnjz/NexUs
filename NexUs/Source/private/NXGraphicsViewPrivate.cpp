#include "NXGraphicsViewPrivate.h"

#include "NXGraphicsView.h"
#include "NXScrollBar.h"

NXGraphicsViewPrivate::NXGraphicsViewPrivate(QObject *parent)
    : QObject { parent }
{
}

NXGraphicsViewPrivate::~NXGraphicsViewPrivate()
{
}

void NXGraphicsViewPrivate::_initStyle()
{
  Q_Q(NXGraphicsView);
  q->setDragMode(QGraphicsView::RubberBandDrag);
  q->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  q->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
  q->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  q->setMouseTracking(true);
  q->setObjectName("NXGraphicsView");
  q->setStyleSheet(QStringLiteral("#NXGraphicsView{background-color:white;border:0px;}"));
  q->setVerticalScrollBar(new NXScrollBar(q));
  q->setHorizontalScrollBar(new NXScrollBar(q));
}
