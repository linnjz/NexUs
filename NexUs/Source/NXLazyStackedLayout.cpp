#include "NXLazyStackedLayout.h"

#include <algorithm>
#include <memory>
#include <utility>

#include <QDebug>
#include <QPointer>
#include <QWidget>

#include "private/NXLazyStackedLayoutPrivate.h"

namespace
{
Q_ALWAYS_INLINE bool
isValidStackIndex(const QList<NXLazyStackedLayoutPrivate::StackItemTag> &itemList, int index)
{
  return index >= 0 && index < itemList.size();
}

Q_ALWAYS_INLINE QWidget *
peekWidget(const NXLazyStackedLayoutPrivate::StackItemTag &itemTag)
{
  return itemTag.Item ? itemTag.Item->widget() : nullptr;
}

Q_ALWAYS_INLINE int
normalizeInsertIndex(int index, int size)
{
  if (index < 0) {
    return size;
  }
  return std::min(index, size);
}
} // namespace

NXLazyStackedLayout::NXLazyStackedLayout()
    : d_ptr(new NXLazyStackedLayoutPrivate())
{
  d_ptr->q_ptr = this;
}

NXLazyStackedLayout::NXLazyStackedLayout(QWidget *parent)
    : QLayout(parent)
    , d_ptr(new NXLazyStackedLayoutPrivate())
{
  d_ptr->q_ptr = this;
}

NXLazyStackedLayout::~NXLazyStackedLayout()
{
  Q_D(NXLazyStackedLayout);
  for (const auto &itemTag : d->_itemList) {
    delete itemTag.Item;
  }
}

int NXLazyStackedLayout::addWidget(QWidget *widget)
{
  Q_D(NXLazyStackedLayout);
  return insertWidget(d->_itemList.size(), widget);
}

int NXLazyStackedLayout::insertWidget(int index, QWidget *widget)
{
  if (!widget) {
    qWarning("NXLazyStackedLayout::insertWidget: Invalid widget.");
    return -1;
  }

  Q_D(NXLazyStackedLayout);
  addChildWidget(widget);

  index = normalizeInsertIndex(index, d->_itemList.size());
  d->_itemList.insert(index, { new QWidgetItem(widget), {} });

  invalidate();
  if (d->_currentIndex < 0) {
    setCurrentIndex(index);
  } else {
    if (index <= d->_currentIndex) {
      ++d->_currentIndex;
    }
    if (d->_stackingMode == StackOne) {
      widget->hide();
    }
    widget->lower();
  }
  return index;
}

int NXLazyStackedLayout::addLazyWidget(std::function<QWidget *()> &&lazyFactory)
{
  Q_D(NXLazyStackedLayout);
  return insertLazyWidget(d->_itemList.size(), std::move(lazyFactory));
}

int NXLazyStackedLayout::insertLazyWidget(int index, std::function<QWidget *()> &&lazyFactory)
{
  if (!lazyFactory) {
    qWarning("NXLazyStackedLayout::insertLazyWidget: Invalid lazy factory.");
    return -1;
  }

  Q_D(NXLazyStackedLayout);
  index = normalizeInsertIndex(index, d->_itemList.size());
  d->_itemList.insert(index, { nullptr, std::move(lazyFactory) });
  invalidate();

  if (d->_currentIndex < 0) {
    d->_currentIndex = index;
  } else if (index <= d->_currentIndex) {
    ++d->_currentIndex;
  }
  return index;
}

QWidget * NXLazyStackedLayout::ensureWidget(int index)
{
  Q_D(NXLazyStackedLayout);
  if (!isValidStackIndex(d->_itemList, index)) {
    return nullptr;
  }

  auto &itemTag = d->_itemList[index];
  if (QWidget *widget = peekWidget(itemTag)) {
    return widget;
  }

  if (!itemTag.LazyFactory) {
    return nullptr;
  }

  std::function<QWidget *()> lazyFactory = std::move(itemTag.LazyFactory);
  QWidget *widget                        = lazyFactory();
  if (!widget) {
    itemTag.LazyFactory = std::move(lazyFactory);
    qWarning("NXLazyStackedLayout::ensureWidget: Lazy factory returned nullptr.");
    return nullptr;
  }
  itemTag.LazyFactory = {};

  addChildWidget(widget);
  itemTag.Item = new QWidgetItem(widget);

  if (geometry().isValid()) {
    widget->setGeometry(geometry());
  }

  if (d->_stackingMode == StackOne) {
    if (index != d->_currentIndex) {
      widget->hide();
      widget->lower();
    } else {
      widget->show();
      widget->raise();
    }
  } else {
    widget->show();
    if (index == d->_currentIndex) {
      widget->raise();
    } else {
      widget->lower();
    }
  }

  invalidate();
  Q_EMIT widgetCreated(index, widget);
  return widget;
}

bool NXLazyStackedLayout::isWidgetCreated(int index) const
{
  Q_D(const NXLazyStackedLayout);
  if (!isValidStackIndex(d->_itemList, index)) {
    return false;
  }
  return peekWidget(d->_itemList.at(index));
}

QWidget * NXLazyStackedLayout::currentWidget() const
{
  Q_D(const NXLazyStackedLayout);
  if (!isValidStackIndex(d->_itemList, d->_currentIndex)) {
    return nullptr;
  }
  return const_cast<NXLazyStackedLayout *>(this)->ensureWidget(d->_currentIndex);
}

int NXLazyStackedLayout::currentIndex() const
{
  Q_D(const NXLazyStackedLayout);
  return d->_currentIndex;
}

QWidget * NXLazyStackedLayout::widget(int index) const
{
  return const_cast<NXLazyStackedLayout *>(this)->ensureWidget(index);
}

int NXLazyStackedLayout::count() const
{
  Q_D(const NXLazyStackedLayout);
  return d->_itemList.size();
}

int NXLazyStackedLayout::indexOf(const QWidget *widget) const
{
  Q_D(const NXLazyStackedLayout);
  if (!widget) {
    return -1;
  }

  for (int i = 0; i < d->_itemList.size(); ++i) {
    if (peekWidget(d->_itemList.at(i)) == widget) {
      return i;
    }
  }
  return -1;
}

NXLazyStackedLayout::StackingMode NXLazyStackedLayout::stackingMode() const
{
  return d_ptr->_stackingMode;
}

void NXLazyStackedLayout::setStackingMode(StackingMode stackingMode)
{
  Q_D(NXLazyStackedLayout);
  if (d->_stackingMode == stackingMode) {
    return;
  }
  d->_stackingMode = stackingMode;

  if (d->_itemList.isEmpty()) {
    return;
  }

  switch (d->_stackingMode) {
  case StackOne : {
    for (int i = 0; i < d->_itemList.size(); ++i) {
      if (QWidget *widget = peekWidget(d->_itemList.at(i))) {
        widget->setVisible(i == d->_currentIndex);
      }
    }
    if (isValidStackIndex(d->_itemList, d->_currentIndex)) {
      if (QWidget *widget = ensureWidget(d->_currentIndex)) {
        widget->show();
        widget->raise();
      }
    }
    break;
  }
  case StackAll : {
    QRect stackedGeometry {};
    if (QWidget *widget = currentWidget()) {
      stackedGeometry = widget->geometry();
    }
    for (int i = 0; i < d->_itemList.size(); ++i) {
      if (QWidget *widget = ensureWidget(i)) {
        if (!stackedGeometry.isNull()) {
          widget->setGeometry(stackedGeometry);
        }
        widget->setVisible(true);
        if (i == d->_currentIndex) {
          widget->raise();
        } else {
          widget->lower();
        }
      }
    }
    break;
  }
  }
}

void NXLazyStackedLayout::addItem(QLayoutItem *item)
{
  std::unique_ptr<QLayoutItem> guard(item);
  if (!item || !item->widget()) {
    qWarning("NXLazyStackedLayout::addItem: Only QWidget is supported.");
    return;
  }
  addWidget(item->widget());
}

QSize NXLazyStackedLayout::sizeHint() const
{
  Q_D(const NXLazyStackedLayout);
  QSize size(0, 0);
  for (const auto &itemTag : d->_itemList) {
    if (QWidget *widget = peekWidget(itemTag)) {
      QSize widgetSize = widget->sizeHint();
      if (widget->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored) {
        widgetSize.setWidth(0);
      }
      if (widget->sizePolicy().verticalPolicy() == QSizePolicy::Ignored) {
        widgetSize.setHeight(0);
      }
      size = size.expandedTo(widgetSize);
    }
  }
  return size;
}

QSize NXLazyStackedLayout::minimumSize() const
{
  Q_D(const NXLazyStackedLayout);
  QSize size(0, 0);
  for (const auto &itemTag : d->_itemList) {
    if (QWidget *widget = peekWidget(itemTag)) {
      size = size.expandedTo(widget->minimumSizeHint());
    }
  }
  return size;
}

QLayoutItem * NXLazyStackedLayout::itemAt(int index) const
{
  Q_D(const NXLazyStackedLayout);
  if (!isValidStackIndex(d->_itemList, index)) {
    return nullptr;
  }
  return d->_itemList.at(index).Item;
}

QLayoutItem * NXLazyStackedLayout::takeAt(int index)
{
  Q_D(NXLazyStackedLayout);
  if (!isValidStackIndex(d->_itemList, index)) {
    return nullptr;
  }

  NXLazyStackedLayoutPrivate::StackItemTag itemTag = d->_itemList.takeAt(index);
  QLayoutItem *item                                = itemTag.Item;
  if (index == d->_currentIndex) {
    d->_currentIndex = -1;
    if (!d->_itemList.isEmpty()) {
      const int newIndex = (index == d->_itemList.size()) ? index - 1 : index;
      setCurrentIndex(newIndex);
    } else {
      Q_EMIT currentChanged(-1);
    }
  } else if (index < d->_currentIndex) {
    --d->_currentIndex;
  }

  Q_EMIT widgetRemoved(index);
  if (item && item->widget()) {
    item->widget()->hide();
  }
  return item;
}

void NXLazyStackedLayout::setGeometry(const QRect &rect)
{
  Q_D(NXLazyStackedLayout);
  QLayout::setGeometry(rect);
  switch (d->_stackingMode) {
  case StackOne : {
    if (isValidStackIndex(d->_itemList, d->_currentIndex)) {
      if (QWidget *widget = ensureWidget(d->_currentIndex)) {
        widget->setGeometry(rect);
      }
    }
    break;
  }
  case StackAll : {
    for (int i = 0; i < d->_itemList.size(); ++i) {
      if (QWidget *widget = ensureWidget(i)) {
        widget->setGeometry(rect);
      }
    }
    break;
  }
  }
}

bool NXLazyStackedLayout::hasHeightForWidth() const
{
  const int itemCount = count();
  for (int i = 0; i < itemCount; ++i) {
    if (QLayoutItem *item = itemAt(i)) {
      if (item->hasHeightForWidth()) {
        return true;
      }
    }
  }
  return false;
}

int NXLazyStackedLayout::heightForWidth(int width) const
{
  const int itemCount = count();
  int hfw             = 0;
  for (int i = 0; i < itemCount; ++i) {
    if (QLayoutItem *item = itemAt(i)) {
      if (QWidget *widget = item->widget()) {
        hfw = qMax(hfw, widget->heightForWidth(width));
      }
    }
  }
  hfw = qMax(hfw, minimumSize().height());
  return hfw;
}

void NXLazyStackedLayout::setCurrentIndex(int index)
{
  Q_D(NXLazyStackedLayout);
  if (!isValidStackIndex(d->_itemList, index)) {
    return;
  }

  QWidget *prev = nullptr;
  if (isValidStackIndex(d->_itemList, d->_currentIndex)) {
    prev = peekWidget(d->_itemList.at(d->_currentIndex));
  }

  bool reenableUpdates = false;
  QWidget *parent      = parentWidget();
  if (parent && parent->updatesEnabled()) {
    reenableUpdates = true;
    parent->setUpdatesEnabled(false);
  }

  QWidget *next = ensureWidget(index);
  if (!next || next == prev) {
    if (reenableUpdates) {
      parent->setUpdatesEnabled(true);
    }
    return;
  }

  QPointer<QWidget> focusWidget = parent ? parent->window()->focusWidget() : nullptr;
  const bool focusWasOnOldPage  = focusWidget && prev && prev->isAncestorOf(focusWidget);

  const int previousIndex = d->_currentIndex;
  if (prev && prev != next) {
    prev->clearFocus();
    if (d->_stackingMode == StackOne) {
      prev->hide();
    }
  }

  d->_currentIndex = index;
  next->setGeometry(geometry());
  next->show();
  next->raise();

  if (parent && focusWasOnOldPage) {
    if (QWidget *nextFocusWidget = next->focusWidget()) {
      nextFocusWidget->setFocus();
    } else if (QWidget *it = focusWidget) {
      while ((it = it->nextInFocusChain()) != focusWidget) {
        const bool canTabFocus = (it->focusPolicy() & Qt::TabFocus) == Qt::TabFocus;
        if (canTabFocus && !it->focusProxy() && it->isVisibleTo(next) && it->isEnabled() && next->isAncestorOf(it)) {
          it->setFocus();
          break;
        }
      }
      if (it == focusWidget) {
        next->setFocus();
      }
    }
  }

  if (reenableUpdates) {
    parent->setUpdatesEnabled(true);
  }

  if (previousIndex != index) {
    Q_EMIT currentChanged(index);
  }
}

void NXLazyStackedLayout::setCurrentWidget(QWidget *widget)
{
  const int index = indexOf(widget);
  if (index < 0) {
    qWarning("NXLazyStackedLayout::setCurrentWidget: Widget is not contained in stack.");
    return;
  }
  setCurrentIndex(index);
}
