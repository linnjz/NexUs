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
  d->m_Layout = new NXLazyStackedLayout(this);

  connect(d->m_Layout, &NXLazyStackedLayout::widgetRemoved, this, &NXLazyStackedWidget::widgetRemoved);
  connect(d->m_Layout, &NXLazyStackedLayout::currentChanged, this, &NXLazyStackedWidget::currentChanged);
  connect(d->m_Layout, &NXLazyStackedLayout::widgetCreated, this, &NXLazyStackedWidget::widgetCreated);
}

NXLazyStackedWidget::~NXLazyStackedWidget()
{
}

int NXLazyStackedWidget::addWidget(QWidget *widget)
{
  Q_D(NXLazyStackedWidget);
  return d->m_Layout->addWidget(widget);
}

int NXLazyStackedWidget::insertWidget(int index, QWidget *widget)
{
  Q_D(NXLazyStackedWidget);
  return d->m_Layout->insertWidget(index, widget);
}

void NXLazyStackedWidget::removeWidget(QWidget *widget)
{
  Q_D(NXLazyStackedWidget);
  d->m_Layout->removeWidget(widget);
}

int NXLazyStackedWidget::addLazyWidget(std::function<QWidget *()> &&lazyFactory)
{
  Q_D(NXLazyStackedWidget);
  return d->m_Layout->addLazyWidget(std::move(lazyFactory));
}

int NXLazyStackedWidget::insertLazyWidget(int index, std::function<QWidget *()> &&lazyFactory)
{
  Q_D(NXLazyStackedWidget);
  return d->m_Layout->insertLazyWidget(index, std::move(lazyFactory));
}

QWidget *NXLazyStackedWidget::ensureWidget(int index)
{
  Q_D(NXLazyStackedWidget);
  return d->m_Layout->ensureWidget(index);
}

bool NXLazyStackedWidget::isWidgetCreated(int index) const
{
  Q_CONST_D(NXLazyStackedWidget);
  return d->m_Layout->isWidgetCreated(index);
}

QWidget *NXLazyStackedWidget::currentWidget() const
{
  Q_CONST_D(NXLazyStackedWidget);
  return d->m_Layout->currentWidget();
}

int NXLazyStackedWidget::currentIndex() const
{
  Q_CONST_D(NXLazyStackedWidget);
  return d->m_Layout->currentIndex();
}

int NXLazyStackedWidget::indexOf(QWidget const *widget) const
{
  Q_CONST_D(NXLazyStackedWidget);
  return d->m_Layout->indexOf(widget);
}

QWidget *NXLazyStackedWidget::widget(int index) const
{
  Q_CONST_D(NXLazyStackedWidget);
  return d->m_Layout->widget(index);
}

int NXLazyStackedWidget::count() const
{
  Q_CONST_D(NXLazyStackedWidget);
  return d->m_Layout->count();
}

void NXLazyStackedWidget::setCurrentIndex(int index)
{
  Q_D(NXLazyStackedWidget);
  d->m_Layout->setCurrentIndex(index);
}

void NXLazyStackedWidget::setCurrentWidget(QWidget *widget)
{
  Q_D(NXLazyStackedWidget);
  d->m_Layout->setCurrentWidget(widget);
}

bool NXLazyStackedWidget::event(QEvent *event)
{
  return QFrame::event(event);
}
