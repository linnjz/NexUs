#include "NXSheetPanel.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QVBoxLayout>

#include "DeveloperComponents/NXMaskWidget.h"
#include "NXTheme.h"
#include "private/NXSheetPanelPrivate.h"
Q_PROPERTY_CREATE_CPP(NXSheetPanel, qreal, PeekRatio)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, qreal, HalfRatio)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, qreal, FullRatio)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, qreal, OverlayOpacity)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, NXSheetPanelType::Direction, Direction)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, bool, DragHandleVisible)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, bool, CloseOnOverlayClick)

NXSheetPanel::NXSheetPanel(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXSheetPanelPrivate())
{
  Q_D(NXSheetPanel);
  d->q_ptr                 = this;
  d->_pBorderRadius        = 12;
  d->_pDirection           = NXSheetPanelType::Bottom;
  d->_pPeekRatio           = 0.15;
  d->_pHalfRatio           = 0.50;
  d->_pFullRatio           = 0.85;
  d->_pDragHandleVisible   = true;
  d->_pCloseOnOverlayClick = true;
  d->_pOverlayOpacity      = 0.4;
  d->_themeMode            = nxTheme->getThemeMode();

  setVisible(false);

  d->_maskWidget = new NXMaskWidget(parent);
  d->_maskWidget->setVisible(false);

  d->_panelWidget = new QWidget(parent);
  d->_panelWidget->setVisible(false);
  d->_panelWidget->setMouseTracking(true);
  d->_panelWidget->installEventFilter(this);

  d->_dragHandle = new QWidget(d->_panelWidget);
  d->_dragHandle->setFixedSize(40, 4);
  d->_dragHandle->installEventFilter(this);
  d->_dragHandle->setCursor(Qt::SizeVerCursor);

  d->_panelLayout = new QVBoxLayout(d->_panelWidget);
  d->_panelLayout->setContentsMargins(0, 12, 0, 0);
  d->_panelLayout->setSpacing(0);
  d->_panelLayout->addWidget(d->_dragHandle, 0, Qt::AlignHCenter);
  d->_panelLayout->addSpacing(8);

  if (parent) {
    parent->installEventFilter(this);
    d->_maskWidget->setFixedSize(parent->size());
  }

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    d->_panelWidget->update();
  });
}

NXSheetPanel::~NXSheetPanel()
{
  Q_D(NXSheetPanel);
  if (d->_maskWidget) {
    d->_maskWidget->deleteLater();
  }
  if (d->_panelWidget) {
    d->_panelWidget->deleteLater();
  }
}

void NXSheetPanel::setCentralWidget(QWidget *widget)
{
  Q_D(NXSheetPanel);
  if (d->_centralWidget) {
    d->_panelLayout->removeWidget(d->_centralWidget);
    d->_centralWidget->deleteLater();
  }
  d->_centralWidget = widget;
  if (widget) {
    d->_panelLayout->addWidget(widget);
  }
}

void NXSheetPanel::open(NXSheetPanelType::DetentLevel level)
{
  Q_D(NXSheetPanel);
  if (d->_isOpened && d->_currentDetent == level) {
    return;
  }
  d->_isOpened      = true;
  d->_currentDetent = level;

  if (parentWidget()) {
    d->_maskWidget->setFixedSize(parentWidget()->size());
    d->_maskWidget->move(0, 0);
  }
  d->_maskWidget->setVisible(true);
  d->_maskWidget->raise();
  d->_maskWidget->doMaskAnimation(static_cast<int>(d->_pOverlayOpacity * 255));

  d->_panelWidget->setVisible(true);
  d->_panelWidget->raise();
  d->_doOpenAnimation(level);

  Q_EMIT opened();
  Q_EMIT detentChanged(level);
}

void NXSheetPanel::close()
{
  Q_D(NXSheetPanel);
  if (!d->_isOpened) {
    return;
  }
  d->_doCloseAnimation();
}

NXSheetPanelType::DetentLevel NXSheetPanel::currentDetent() const
{
  return d_ptr->_currentDetent;
}

bool NXSheetPanel::isOpened() const
{
  return d_ptr->_isOpened;
}

bool NXSheetPanel::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXSheetPanel);
  if (watched == parentWidget()) {
    if (event->type() == QEvent::Resize) {
      if (d->_isOpened && parentWidget()) {
        d->_maskWidget->setFixedSize(parentWidget()->size());
        int targetSize = d->_getTargetSize(d->_currentDetent);
        d->_updatePanelGeometry(targetSize, false);
      }
    }
  } else if (watched == d->_maskWidget) {
    if (event->type() == QEvent::MouseButtonPress && d->_pCloseOnOverlayClick) {
      close();
      return true;
    }
  } else if (watched == d->_dragHandle || watched == d->_panelWidget) {
    if (event->type() == QEvent::MouseButtonPress) {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
      if (mouseEvent->button() == Qt::LeftButton && watched == d->_dragHandle) {
        d->_isDragging = true;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        d->_dragStartPos = mouseEvent->globalPosition().toPoint();
#else
        d->_dragStartPos = mouseEvent->globalPos();
#endif
        if (d->_pDirection == NXSheetPanelType::Bottom) {
          d->_dragStartSize = d->_panelWidget->height();
        } else {
          d->_dragStartSize = d->_panelWidget->width();
        }
        return true;
      }
    } else if (event->type() == QEvent::MouseMove && d->_isDragging) {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
      QPoint delta = mouseEvent->globalPosition().toPoint() - d->_dragStartPos;
#else
      QPoint delta = mouseEvent->globalPos() - d->_dragStartPos;
#endif
      int newSize = d->_dragStartSize;
      if (d->_pDirection == NXSheetPanelType::Bottom) {
        newSize = d->_dragStartSize - delta.y();
      } else if (d->_pDirection == NXSheetPanelType::Right) {
        newSize = d->_dragStartSize - delta.x();
      } else {
        newSize = d->_dragStartSize + delta.x();
      }
      newSize = qMax(20, newSize);
      d->_updatePanelGeometry(newSize, false);
      return true;
    } else if (event->type() == QEvent::MouseButtonRelease && d->_isDragging) {
      d->_isDragging = false;
      int currentSize;
      if (d->_pDirection == NXSheetPanelType::Bottom) {
        currentSize = d->_panelWidget->height();
      } else {
        currentSize = d->_panelWidget->width();
      }
      int peekSize = d->_getTargetSize(NXSheetPanelType::Peek);
      if (currentSize < peekSize / 2) {
        close();
      } else {
        NXSheetPanelType::DetentLevel nearestLevel = d->_getNearestDetent(currentSize);
        d->_currentDetent                          = nearestLevel;
        d->_updatePanelGeometry(d->_getTargetSize(nearestLevel), true);
        Q_EMIT detentChanged(nearestLevel);
      }
      return true;
    }
  }
  return QWidget::eventFilter(watched, event);
}
