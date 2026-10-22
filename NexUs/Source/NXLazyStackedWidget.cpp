#include "NXLazyStackedWidget.h"

#include <QEvent>
#include <utility>

#include "NXLazyStackedLayout.h"
#include "private/NXLazyStackedWidgetPrivate.h"

NXLazyStackedWidget::NXLazyStackedWidget(QWidget *parent)
    : QFrame(parent)
    , d_ptr(new NXLazyStackedWidgetPrivate())
{
  Q_D(NXLazyStackedWidget);
  d->q_ptr   = this;
  d->_layout = new NXLazyStackedLayout(this);

  connect(d->_layout, &NXLazyStackedLayout::widgetRemoved, this, &NXLazyStackedWidget::widgetRemoved);
  connect(d->_layout, &NXLazyStackedLayout::currentChanged, this, &NXLazyStackedWidget::currentChanged);
  connect(d->_layout, &NXLazyStackedLayout::widgetCreated, this, &NXLazyStackedWidget::widgetCreated);
}

NXLazyStackedWidget::~NXLazyStackedWidget()
{
}

int NXLazyStackedWidget::addWidget(QWidget *widget)
{
  Q_D(NXLazyStackedWidget);
  return d->_layout->addWidget(widget);
}

int NXLazyStackedWidget::insertWidget(int index, QWidget *widget)
{
  Q_D(NXLazyStackedWidget);
  return d->_layout->insertWidget(index, widget);
}

void NXLazyStackedWidget::removeWidget(QWidget *widget)
{
  Q_D(NXLazyStackedWidget);
  d->_layout->removeWidget(widget);
}

int NXLazyStackedWidget::addLazyWidget(std::function<QWidget *()> &&lazyFactory)
{
  Q_D(NXLazyStackedWidget);
  return d->_layout->addLazyWidget(std::move(lazyFactory));
}

int NXLazyStackedWidget::insertLazyWidget(int index, std::function<QWidget *()> &&lazyFactory)
{
  Q_D(NXLazyStackedWidget);
  return d->_layout->insertLazyWidget(index, std::move(lazyFactory));
}

QWidget * NXLazyStackedWidget::ensureWidget(int index)
{
  Q_D(NXLazyStackedWidget);
  return d->_layout->ensureWidget(index);
}

bool NXLazyStackedWidget::isWidgetCreated(int index) const
{
  Q_D(const NXLazyStackedWidget);
  return d->_layout->isWidgetCreated(index);
}

QWidget * NXLazyStackedWidget::currentWidget() const
{
  Q_D(const NXLazyStackedWidget);
  return d->_layout->currentWidget();
}

int NXLazyStackedWidget::currentIndex() const
{
  Q_D(const NXLazyStackedWidget);
  return d->_layout->currentIndex();
}

int NXLazyStackedWidget::indexOf(const QWidget *widget) const
{
  Q_D(const NXLazyStackedWidget);
  return d->_layout->indexOf(widget);
}

QWidget * NXLazyStackedWidget::widget(int index) const
{
  Q_D(const NXLazyStackedWidget);
  return d->_layout->widget(index);
}

int NXLazyStackedWidget::count() const
{
  Q_D(const NXLazyStackedWidget);
  return d->_layout->count();
}

void NXLazyStackedWidget::setCurrentIndex(int index)
{
  Q_D(NXLazyStackedWidget);
  d->_layout->setCurrentIndex(index);
}

void NXLazyStackedWidget::setCurrentWidget(QWidget *widget)
{
  Q_D(NXLazyStackedWidget);
  d->_layout->setCurrentWidget(widget);
}

bool NXLazyStackedWidget::event(QEvent *event)
{
  return QFrame::event(event);
}
