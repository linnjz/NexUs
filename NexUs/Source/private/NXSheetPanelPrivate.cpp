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

  if (_pDirection == NXSheetPanelType::Bottom) {
    _panelWidget->setGeometry(0, parent->height(), parent->width(), 0);
    QPropertyAnimation *animation = new QPropertyAnimation(_panelWidget, "geometry");
    animation->setDuration(350);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(QRect(0, parent->height(), parent->width(), 0));
    animation->setEndValue(QRect(0, parent->height() - targetSize, parent->width(), targetSize));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  } else if (_pDirection == NXSheetPanelType::Right) {
    _panelWidget->setGeometry(parent->width(), 0, 0, parent->height());
    QPropertyAnimation *animation = new QPropertyAnimation(_panelWidget, "geometry");
    animation->setDuration(350);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(QRect(parent->width(), 0, 0, parent->height()));
    animation->setEndValue(QRect(parent->width() - targetSize, 0, targetSize, parent->height()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  } else {
    _panelWidget->setGeometry(-targetSize, 0, 0, parent->height());
    QPropertyAnimation *animation = new QPropertyAnimation(_panelWidget, "geometry");
    animation->setDuration(350);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(QRect(0, 0, 0, parent->height()));
    animation->setEndValue(QRect(0, 0, targetSize, parent->height()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  }

  _panelWidget->setStyleSheet(
      QString(QStringLiteral("background-color: %1; border-top-left-radius: %2px; border-top-right-radius: %2px;"))
          .arg(NXThemeColor(_themeMode, DialogBase).name())
          .arg(_pBorderRadius));

  _dragHandle->setStyleSheet(QString(QStringLiteral("background-color: %1; border-radius: 2px;"))
                                 .arg(NXThemeColor(_themeMode, BasicBorderDeep).name()));
  _dragHandle->setVisible(_pDragHandleVisible);
}

void NXSheetPanelPrivate::_doCloseAnimation()
{
  Q_Q(NXSheetPanel);
  QWidget *parent = q->parentWidget();
  if (!parent) {
    return;
  }
  _maskWidget->doMaskAnimation(0);

  QRect startRect = _panelWidget->geometry();
  QRect endRect;
  if (_pDirection == NXSheetPanelType::Bottom) {
    endRect = QRect(0, parent->height(), parent->width(), 0);
  } else if (_pDirection == NXSheetPanelType::Right) {
    endRect = QRect(parent->width(), 0, 0, parent->height());
  } else {
    endRect = QRect(0, 0, 0, parent->height());
  }

  QPropertyAnimation *animation = new QPropertyAnimation(_panelWidget, "geometry");
  connect(animation, &QPropertyAnimation::finished, q, [=]() {
    _panelWidget->setVisible(false);
    _isOpened = false;
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
  if (_pDirection == NXSheetPanelType::Bottom) {
    targetRect = QRect(0, parent->height() - targetSize, parent->width(), targetSize);
  } else if (_pDirection == NXSheetPanelType::Right) {
    targetRect = QRect(parent->width() - targetSize, 0, targetSize, parent->height());
  } else {
    targetRect = QRect(0, 0, targetSize, parent->height());
  }

  if (animate) {
    QPropertyAnimation *animation = new QPropertyAnimation(_panelWidget, "geometry");
    animation->setDuration(250);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(_panelWidget->geometry());
    animation->setEndValue(targetRect);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  } else {
    _panelWidget->setGeometry(targetRect);
  }
}

int NXSheetPanelPrivate::_getTargetSize(NXSheetPanelType::DetentLevel level) const
{
  Q_Q(const NXSheetPanel);
  QWidget *parent = q->parentWidget();
  if (!parent) {
    return 100;
  }
  int totalSize;
  if (_pDirection == NXSheetPanelType::Bottom) {
    totalSize = parent->height();
  } else {
    totalSize = parent->width();
  }
  switch (level) {
  case NXSheetPanelType::Peek : return static_cast<int>(totalSize * _pPeekRatio);
  case NXSheetPanelType::Half : return static_cast<int>(totalSize * _pHalfRatio);
  case NXSheetPanelType::Full : return static_cast<int>(totalSize * _pFullRatio);
  default                     : return static_cast<int>(totalSize * _pHalfRatio);
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
