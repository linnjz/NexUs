#include "NXGraphicsView.h"

#include <QEvent>
#include <QScrollBar>
#include <QWheelEvent>

#include "private/NXGraphicsViewPrivate.h"
Q_PROPERTY_CREATE_CPP(NXGraphicsView, qreal, MaxTransform)
Q_PROPERTY_CREATE_CPP(NXGraphicsView, qreal, MinTransform)

NXGraphicsView::NXGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , d_ptr(new NXGraphicsViewPrivate())
{
  Q_D(NXGraphicsView);
  d->q_ptr          = this;
  d->_pMaxTransform = 5;
  d->_pMinTransform = 0.15;
  d->_initStyle();
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

NXGraphicsView::NXGraphicsView(QGraphicsScene *scene, QWidget *parent)
    : NXGraphicsView(parent)
{
  setScene(scene);
}

NXGraphicsView::~NXGraphicsView()
{
}

void NXGraphicsView::wheelEvent(QWheelEvent *event)
{
  Q_D(NXGraphicsView);
  if (event->modifiers() == Qt::CTRL) {
    // 放大
    if ((event->angleDelta().y() > 0) && transform().m11() <= d->_pMaxTransform) {
      this->scale(1.1, 1.1);
    } else if ((event->angleDelta().y() < 0) && transform().m11() >= d->_pMinTransform) {
      this->scale(1.0 / 1.1, 1.0 / 1.1);
    }
    event->accept();
    return;
  }
  QGraphicsView::wheelEvent(event);
}

void NXGraphicsView::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Alt) {
    setDragMode(QGraphicsView::ScrollHandDrag);
  }
  QGraphicsView::keyPressEvent(event);
}

void NXGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Alt) {
    setDragMode(QGraphicsView::RubberBandDrag);
  }
  QGraphicsView::keyReleaseEvent(event);
}
