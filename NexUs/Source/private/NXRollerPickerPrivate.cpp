#include "NXRollerPickerPrivate.h"

#include <QPropertyAnimation>

#include "NXRollerPicker.h"

NXRollerPickerPrivate::NXRollerPickerPrivate(QObject *parent)
    : QObject(parent)
{
}

NXRollerPickerPrivate::~NXRollerPickerPrivate()
{
}

void NXRollerPickerPrivate::onRollerPickerClicked()
{
  Q_Q(NXRollerPicker);
  QPoint targetPos(q->mapToGlobal(QPoint(
      -6, (q->height() - m_RollerPickerContainer->height() + m_RollerPickerContainer->getButtonAreaHeight()) / 2)));
  m_RollerPickerContainer->show();
  m_RollerPickerContainer->setGeometry(
      QRect(targetPos, QSize(_getRollerTotalWidth() + 16, m_PickerRollerHeight + m_RollerPickerContainer->getButtonAreaHeight())));
  m_RollerPickerContainer->doPickerAnimation();
}

void NXRollerPickerPrivate::onOverButtonClicked()
{
  Q_Q(NXRollerPicker);
  Q_EMIT q->currentDataChanged(q->getCurrentData());
}

void NXRollerPickerPrivate::onCancelButtonClicked()
{
}

int NXRollerPickerPrivate::_getRollerTotalWidth() const
{
  int totalWidth = 0;
  for (auto rollerItem : m_RollerPickerContainer->m_RollerList) {
    totalWidth += rollerItem->width();
  }
  return totalWidth;
}
