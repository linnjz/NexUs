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
Q_PROPERTY_CREATE_CPP(NXSheetPanel, QPS_PAEMIT(qreal), PeekRatio)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, QPS_PAEMIT(qreal), HalfRatio)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, QPS_PAEMIT(qreal), FullRatio)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, QPS_PAEMIT(qreal), OverlayOpacity)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, QPS_PAEMIT(NXSheetPanelType::Direction), Direction)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, QPS_PAEMIT(bool), DragHandleVisible)
Q_PROPERTY_CREATE_CPP(NXSheetPanel, QPS_PAEMIT(bool), CloseOnOverlayClick)

NXSheetPanel::NXSheetPanel(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXSheetPanelPrivate())
{
  Q_D(NXSheetPanel);
  d->q_ptr                 = this;
  d->m_BorderRadius        = 12;
  d->m_Direction           = NXSheetPanelType::Bottom;
  d->m_PeekRatio           = 0.15;
  d->m_HalfRatio           = 0.50;
  d->m_FullRatio           = 0.85;
  d->m_DragHandleVisible   = true;
  d->m_CloseOnOverlayClick = true;
  d->m_OverlayOpacity      = 0.4;
  d->m_ThemeMode            = nxTheme->getThemeMode();

  setVisible(false);

  d->m_MaskWidget = new NXMaskWidget(parent);
  d->m_MaskWidget->setVisible(false);

  d->m_PanelWidget = new QWidget(parent);
  d->m_PanelWidget->setVisible(false);
  d->m_PanelWidget->setMouseTracking(true);
  d->m_PanelWidget->installEventFilter(this);

  d->m_DragHandle = new QWidget(d->m_PanelWidget);
  d->m_DragHandle->setFixedSize(40, 4);
  d->m_DragHandle->installEventFilter(this);
  d->m_DragHandle->setCursor(Qt::SizeVerCursor);

  d->m_PanelLayout = new QVBoxLayout(d->m_PanelWidget);
  d->m_PanelLayout->setContentsMargins(0, 12, 0, 0);
  d->m_PanelLayout->setSpacing(0);
  d->m_PanelLayout->addWidget(d->m_DragHandle, 0, Qt::AlignHCenter);
  d->m_PanelLayout->addSpacing(8);

  if (parent) {
    parent->installEventFilter(this);
    d->m_MaskWidget->setFixedSize(parent->size());
  }

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    d->m_PanelWidget->update();
  });
}

NXSheetPanel::~NXSheetPanel()
{
  Q_D(NXSheetPanel);
  if (d->m_MaskWidget) {
    d->m_MaskWidget->deleteLater();
  }
  if (d->m_PanelWidget) {
    d->m_PanelWidget->deleteLater();
  }
}

void NXSheetPanel::setCentralWidget(QWidget *widget)
{
  Q_D(NXSheetPanel);
  if (d->m_CentralWidget) {
    d->m_PanelLayout->removeWidget(d->m_CentralWidget);
    d->m_CentralWidget->deleteLater();
  }
  d->m_CentralWidget = widget;
  if (widget) {
    d->m_PanelLayout->addWidget(widget);
  }
}

void NXSheetPanel::open(NXSheetPanelType::DetentLevel level)
{
  Q_D(NXSheetPanel);
  if (d->m_IsOpened && d->m_CurrentDetent == level) {
    return;
  }
  d->m_IsOpened      = true;
  d->m_CurrentDetent = level;

  if (parentWidget()) {
    d->m_MaskWidget->setFixedSize(parentWidget()->size());
    d->m_MaskWidget->move(0, 0);
  }
  d->m_MaskWidget->setVisible(true);
  d->m_MaskWidget->raise();
  d->m_MaskWidget->doMaskAnimation(static_cast<int>(d->m_OverlayOpacity * 255));

  d->m_PanelWidget->setVisible(true);
  d->m_PanelWidget->raise();
  d->_doOpenAnimation(level);

  Q_EMIT opened();
  Q_EMIT detentChanged(level);
}

void NXSheetPanel::close()
{
  Q_D(NXSheetPanel);
  if (!d->m_IsOpened) {
    return;
  }
  d->_doCloseAnimation();
}

NXSheetPanelType::DetentLevel NXSheetPanel::currentDetent() const
{
  return d_ptr->m_CurrentDetent;
}

bool NXSheetPanel::isOpened() const
{
  return d_ptr->m_IsOpened;
}

bool NXSheetPanel::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXSheetPanel);
  if (watched == parentWidget()) {
    if (event->type() == QEvent::Resize) {
      if (d->m_IsOpened && parentWidget()) {
        d->m_MaskWidget->setFixedSize(parentWidget()->size());
        int targetSize = d->_getTargetSize(d->m_CurrentDetent);
        d->_updatePanelGeometry(targetSize, false);
      }
    }
  } else if (watched == d->m_MaskWidget) {
    if (event->type() == QEvent::MouseButtonPress && d->m_CloseOnOverlayClick) {
      close();
      return true;
    }
  } else if (watched == d->m_DragHandle || watched == d->m_PanelWidget) {
    if (event->type() == QEvent::MouseButtonPress) {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
      if (mouseEvent->button() == Qt::LeftButton && watched == d->m_DragHandle) {
        d->m_IsDragging = true;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        d->m_DragStartPos = mouseEvent->globalPosition().toPoint();
#else
        d->m_DragStartPos = mouseEvent->globalPos();
#endif
        if (d->m_Direction == NXSheetPanelType::Bottom) {
          d->m_DragStartSize = d->m_PanelWidget->height();
        } else {
          d->m_DragStartSize = d->m_PanelWidget->width();
        }
        return true;
      }
    } else if (event->type() == QEvent::MouseMove && d->m_IsDragging) {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
      QPoint delta = mouseEvent->globalPosition().toPoint() - d->m_DragStartPos;
#else
      QPoint delta = mouseEvent->globalPos() - d->m_DragStartPos;
#endif
      int newSize = d->m_DragStartSize;
      if (d->m_Direction == NXSheetPanelType::Bottom) {
        newSize = d->m_DragStartSize - delta.y();
      } else if (d->m_Direction == NXSheetPanelType::Right) {
        newSize = d->m_DragStartSize - delta.x();
      } else {
        newSize = d->m_DragStartSize + delta.x();
      }
      newSize = qMax(20, newSize);
      d->_updatePanelGeometry(newSize, false);
      return true;
    } else if (event->type() == QEvent::MouseButtonRelease && d->m_IsDragging) {
      d->m_IsDragging = false;
      int currentSize;
      if (d->m_Direction == NXSheetPanelType::Bottom) {
        currentSize = d->m_PanelWidget->height();
      } else {
        currentSize = d->m_PanelWidget->width();
      }
      int peekSize = d->_getTargetSize(NXSheetPanelType::Peek);
      if (currentSize < peekSize / 2) {
        close();
      } else {
        NXSheetPanelType::DetentLevel nearestLevel = d->_getNearestDetent(currentSize);
        d->m_CurrentDetent                          = nearestLevel;
        d->_updatePanelGeometry(d->_getTargetSize(nearestLevel), true);
        Q_EMIT detentChanged(nearestLevel);
      }
      return true;
    }
  }
  return QWidget::eventFilter(watched, event);
}
