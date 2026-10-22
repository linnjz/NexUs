#include "NXScrollBar.h"

#include <QDebug>
#include <QPainter>
#include <QPointer>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWheelEvent>

#include "DeveloperComponents/NXScrollBarStyle.h"
#include "NXMenu.h"
#include "private/NXScrollBarPrivate.h"
Q_PROPERTY_CREATE_CPP(NXScrollBar, qreal, SpeedLimit)
Q_PROPERTY_CREATE_CPP(NXScrollBar, bool, IsAnimation)

NXScrollBar::NXScrollBar(QWidget *parent)
    : QScrollBar(parent)
    , d_ptr(new NXScrollBarPrivate())
{
  Q_D(NXScrollBar);
  d->q_ptr = this;
  setSingleStep(1);
  setObjectName("NXScrollBar");
  setAttribute(Qt::WA_OpaquePaintEvent, false);
  d->_pSpeedLimit    = 20;
  d->_pTargetMaximum = 0;
  d->_pIsAnimation   = false;
  connect(this, &NXScrollBar::rangeChanged, d, &NXScrollBarPrivate::onRangeChanged);
  NXScrollBarStyle *scrollBarStyle = new NXScrollBarStyle(style());
  scrollBarStyle->setScrollBar(this);
  setStyle(scrollBarStyle);
  d->_slideSmoothAnimation = new QPropertyAnimation(this, "value");
  d->_slideSmoothAnimation->setEasingCurve(QEasingCurve::OutSine);
  d->_slideSmoothAnimation->setDuration(300);
  connect(d->_slideSmoothAnimation, &QPropertyAnimation::finished, this, [=]() {
    d->_scrollValue = value();
  });

  d->_expandTimer = new QTimer(this);
  connect(d->_expandTimer, &QTimer::timeout, this, [=]() {
    d->_expandTimer->stop();
    d->_isExpand = underMouse();
    scrollBarStyle->startExpandAnimation(d->_isExpand);
  });
}

NXScrollBar::NXScrollBar(Qt::Orientation orientation, QWidget *parent)
    : NXScrollBar(parent)
{
  setOrientation(orientation);
}

NXScrollBar::NXScrollBar(QScrollBar *originScrollBar, QAbstractScrollArea *parent)
    : NXScrollBar(parent)
{
  Q_D(NXScrollBar);
  if (!originScrollBar || !parent) {
    qCritical() << "Invalid origin or parent!";
    return;
  }
  d->_originScrollArea        = parent;
  Qt::Orientation orientation = originScrollBar->orientation();
  setOrientation(orientation);
  orientation == Qt::Horizontal ? parent->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff)
                                : parent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  parent->installEventFilter(this);

  d->_originScrollBar = originScrollBar;
  d->_initAllConfig();

  connect(d->_originScrollBar, &QScrollBar::valueChanged, this, [=](int value) {
    d->_handleScrollBarValueChanged(this, value);
  });
  connect(this, &QScrollBar::valueChanged, this, [=](int value) {
    d->_handleScrollBarValueChanged(d->_originScrollBar, value);
  });
  connect(d->_originScrollBar, &QScrollBar::rangeChanged, this, [=](int min, int max) {
    d->_handleScrollBarRangeChanged(min, max);
  });
}

NXScrollBar::~NXScrollBar()
{
  delete this->style();
}

bool NXScrollBar::event(QEvent *event)
{
  Q_D(NXScrollBar);
  switch (event->type()) {
  case QEvent::Enter : {
    d->_expandTimer->stop();
    if (!d->_isExpand) {
      d->_expandTimer->start(350);
    }
    break;
  }
  case QEvent::Leave : {
    d->_expandTimer->stop();
    if (d->_isExpand) {
      d->_expandTimer->start(350);
    }
    break;
  }
  default : {
    break;
  }
  }
  return QScrollBar::event(event);
}

bool NXScrollBar::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXScrollBar);
  switch (event->type()) {
  case QEvent::Show :
  case QEvent::Resize :
  case QEvent::LayoutRequest : {
    d->_handleScrollBarGeometry();
    break;
  }
  default : {
    break;
  }
  }
  return QScrollBar::eventFilter(watched, event);
}

void NXScrollBar::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXScrollBar);
  d->_slideSmoothAnimation->stop();
  QScrollBar::mousePressEvent(event);
  d->_scrollValue = value();
}

void NXScrollBar::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXScrollBar);
  d->_slideSmoothAnimation->stop();
  QScrollBar::mouseReleaseEvent(event);
  d->_scrollValue = value();
}

void NXScrollBar::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXScrollBar);
  d->_slideSmoothAnimation->stop();
  QScrollBar::mouseMoveEvent(event);
  d->_scrollValue = value();
}

void NXScrollBar::wheelEvent(QWheelEvent *event)
{
  Q_D(NXScrollBar);
  int verticalDelta = event->angleDelta().y();
  if (d->_slideSmoothAnimation->state() == QAbstractAnimation::Stopped) {
    d->_scrollValue = value();
  }
  if (verticalDelta != 0) {
    if ((value() == minimum() && verticalDelta > 0) || (value() == maximum() && verticalDelta < 0)) {
      QScrollBar::wheelEvent(event);
      return;
    }
    d->_scroll(event->modifiers(), verticalDelta);
  } else {
    int horizontalDelta = event->angleDelta().x();
    if ((value() == minimum() && horizontalDelta > 0) || (value() == maximum() && horizontalDelta < 0)) {
      QScrollBar::wheelEvent(event);
      return;
    }
    d->_scroll(event->modifiers(), horizontalDelta);
  }
  event->accept();
}

void NXScrollBar::contextMenuEvent(QContextMenuEvent *event)
{
  Q_D(NXScrollBar);
  bool horiz            = this->orientation() == Qt::Horizontal;
  QPointer<NXMenu> menu = new NXMenu(this);
  menu->setMenuItemHeight(27);
  // Scroll here
  QAction *actScrollHere = menu->addNXIconAction(NXIconType::UpDownLeftRight, tr("滚动到此处"));
  menu->addSeparator();
  // Left edge Top
  QAction *actScrollTop = menu->addNXIconAction(horiz ? NXIconType::ArrowLeftToLine : NXIconType::ArrowUpToLine, horiz ? tr("左边缘") : tr("顶端"));
  // Right edge Bottom
  QAction *actScrollBottom = menu->addNXIconAction(horiz ? NXIconType::ArrowRightToLine : NXIconType::ArrowDownToLine, horiz ? tr("右边缘") : tr("底部"));
  menu->addSeparator();
  // Page left Page up
  QAction *actPageUp = menu->addNXIconAction(horiz ? NXIconType::AnglesLeft : NXIconType::AnglesUp, horiz ? tr("向左翻页") : tr("向上翻页"));
  //Page right Page down
  QAction *actPageDn = menu->addNXIconAction(horiz ? NXIconType::AnglesRight : NXIconType::AnglesDown, horiz ? tr("向右翻页") : tr("向下翻页"));
  menu->addSeparator();
  //Scroll left Scroll up
  QAction *actScrollUp = menu->addNXIconAction(horiz ? NXIconType::AngleLeft : NXIconType::AngleUp, horiz ? tr("向左滚动") : tr("向上滚动"));
  //Scroll right Scroll down
  QAction *actScrollDn    = menu->addNXIconAction(horiz ? NXIconType::AngleRight : NXIconType::AngleDown, horiz ? tr("向右滚动") : tr("向下滚动"));
  QAction *actionSelected = menu->exec(event->globalPos());
  delete menu;
  if (!actionSelected) {
    return;
  }
  if (actionSelected == actScrollHere) {
    setValue(d->_pixelPosToRangeValue(horiz ? event->pos().x() : event->pos().y()));
  } else if (actionSelected == actScrollTop) {
    triggerAction(QAbstractSlider::SliderToMinimum);
  } else if (actionSelected == actScrollBottom) {
    triggerAction(QAbstractSlider::SliderToMaximum);
  } else if (actionSelected == actPageUp) {
    triggerAction(QAbstractSlider::SliderPageStepSub);
  } else if (actionSelected == actPageDn) {
    triggerAction(QAbstractSlider::SliderPageStepAdd);
  } else if (actionSelected == actScrollUp) {
    triggerAction(QAbstractSlider::SliderSingleStepSub);
  } else if (actionSelected == actScrollDn) {
    triggerAction(QAbstractSlider::SliderSingleStepAdd);
  }
}
