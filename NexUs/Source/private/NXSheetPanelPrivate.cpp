#include "NXSheetPanelPrivate.h"

#include <QPropertyAnimation>
#include <QVBoxLayout>

#include "DeveloperComponents/NXMaskWidget.h"
#include "NXSheetPanel.h"
#include "NXTheme.h"

NXSheetPanelPrivate::NXSheetPanelPrivate(QObject *parent)
    : QObject { parent }
{
}

NXSheetPanelPrivate::~NXSheetPanelPrivate()
{
}

void NXSheetPanelPrivate::_doOpenAnimation(NXSheetPanelType::DetentLevel level)
{
  Q_Q(NXSheetPanel);
  int targetSize  = _getTargetSize(level);
  QWidget *parent = q->parentWidget();
  if (!parent) {
    return;
  }

  if (m_Direction == NXSheetPanelType::Bottom) {
    m_PanelWidget->setGeometry(0, parent->height(), parent->width(), 0);
    QPropertyAnimation *animation = new QPropertyAnimation(m_PanelWidget, "geometry");
    animation->setDuration(350);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(QRect(0, parent->height(), parent->width(), 0));
    animation->setEndValue(QRect(0, parent->height() - targetSize, parent->width(), targetSize));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  } else if (m_Direction == NXSheetPanelType::Right) {
    m_PanelWidget->setGeometry(parent->width(), 0, 0, parent->height());
    QPropertyAnimation *animation = new QPropertyAnimation(m_PanelWidget, "geometry");
    animation->setDuration(350);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(QRect(parent->width(), 0, 0, parent->height()));
    animation->setEndValue(QRect(parent->width() - targetSize, 0, targetSize, parent->height()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  } else {
    m_PanelWidget->setGeometry(-targetSize, 0, 0, parent->height());
    QPropertyAnimation *animation = new QPropertyAnimation(m_PanelWidget, "geometry");
    animation->setDuration(350);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(QRect(0, 0, 0, parent->height()));
    animation->setEndValue(QRect(0, 0, targetSize, parent->height()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  }

  m_PanelWidget->setStyleSheet(
      QString(QStringLiteral("background-color: %1; border-top-left-radius: %2px; border-top-right-radius: %2px;"))
          .arg(NXThemeColor(m_ThemeMode, DialogBase).name())
          .arg(m_BorderRadius));

  m_DragHandle->setStyleSheet(QString(QStringLiteral("background-color: %1; border-radius: 2px;"))
                                 .arg(NXThemeColor(m_ThemeMode, BasicBorderDeep).name()));
  m_DragHandle->setVisible(m_DragHandleVisible);
}

void NXSheetPanelPrivate::_doCloseAnimation()
{
  Q_Q(NXSheetPanel);
  QWidget *parent = q->parentWidget();
  if (!parent) {
    return;
  }
  m_MaskWidget->doMaskAnimation(0);

  QRect startRect = m_PanelWidget->geometry();
  QRect endRect;
  if (m_Direction == NXSheetPanelType::Bottom) {
    endRect = QRect(0, parent->height(), parent->width(), 0);
  } else if (m_Direction == NXSheetPanelType::Right) {
    endRect = QRect(parent->width(), 0, 0, parent->height());
  } else {
    endRect = QRect(0, 0, 0, parent->height());
  }

  QPropertyAnimation *animation = new QPropertyAnimation(m_PanelWidget, "geometry");
  connect(animation, &QPropertyAnimation::finished, q, [=]() {
    m_PanelWidget->setVisible(false);
    m_IsOpened = false;
    Q_EMIT q->closed();
  });
  animation->setDuration(300);
  animation->setEasingCurve(QEasingCurve::InCubic);
  animation->setStartValue(startRect);
  animation->setEndValue(endRect);
  animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXSheetPanelPrivate::_updatePanelGeometry(int targetSize, bool animate)
{
  Q_Q(NXSheetPanel);
  QWidget *parent = q->parentWidget();
  if (!parent) {
    return;
  }
  QRect targetRect;
  if (m_Direction == NXSheetPanelType::Bottom) {
    targetRect = QRect(0, parent->height() - targetSize, parent->width(), targetSize);
  } else if (m_Direction == NXSheetPanelType::Right) {
    targetRect = QRect(parent->width() - targetSize, 0, targetSize, parent->height());
  } else {
    targetRect = QRect(0, 0, targetSize, parent->height());
  }

  if (animate) {
    QPropertyAnimation *animation = new QPropertyAnimation(m_PanelWidget, "geometry");
    animation->setDuration(250);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(m_PanelWidget->geometry());
    animation->setEndValue(targetRect);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  } else {
    m_PanelWidget->setGeometry(targetRect);
  }
}

int NXSheetPanelPrivate::_getTargetSize(NXSheetPanelType::DetentLevel level) const
{
  Q_Q(NXSheetPanel const);
  QWidget *parent = q->parentWidget();
  if (!parent) {
    return 100;
  }
  int totalSize;
  if (m_Direction == NXSheetPanelType::Bottom) {
    totalSize = parent->height();
  } else {
    totalSize = parent->width();
  }
  switch (level) {
  case NXSheetPanelType::Peek : return static_cast<int>(totalSize * m_PeekRatio);
  case NXSheetPanelType::Half : return static_cast<int>(totalSize * m_HalfRatio);
  case NXSheetPanelType::Full : return static_cast<int>(totalSize * m_FullRatio);
  default                     : return static_cast<int>(totalSize * m_HalfRatio);
  }
}

NXSheetPanelType::DetentLevel NXSheetPanelPrivate::_getNearestDetent(int currentSize) const
{
  int peekSize = _getTargetSize(NXSheetPanelType::Peek);
  int halfSize = _getTargetSize(NXSheetPanelType::Half);
  int fullSize = _getTargetSize(NXSheetPanelType::Full);

  int peekDist = qAbs(currentSize - peekSize);
  int halfDist = qAbs(currentSize - halfSize);
  int fullDist = qAbs(currentSize - fullSize);

  if (peekDist <= halfDist && peekDist <= fullDist) {
    return NXSheetPanelType::Peek;
  } else if (halfDist <= fullDist) {
    return NXSheetPanelType::Half;
  }
  return NXSheetPanelType::Full;
}
