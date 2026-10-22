#include "NXComboBoxView.h"

#include <QMouseEvent>

NXComboBoxView::NXComboBoxView(QWidget *parent)
    : QListView(parent)
{
}

NXComboBoxView::~NXComboBoxView()
{
}

void NXComboBoxView::mousePressEvent(QMouseEvent *event)
{
  QModelIndex index = indexAt(event->pos());
  if (index.isValid()) {
    Q_EMIT itemPressed(index);
  }
  event->ignore();
}
