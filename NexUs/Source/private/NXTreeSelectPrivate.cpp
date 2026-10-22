#include "NXTreeSelectPrivate.h"

#include <QApplication>
#include <QLayout>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QScreen>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include "NXLineEdit.h"
#include "NXTheme.h"
#include "NXTreeSelect.h"
#include "NXTreeView.h"

NXTreeSelectPopupWidget::NXTreeSelectPopupWidget(QWidget *parent)
    : QWidget(parent)
{
}

void NXTreeSelectPopupWidget::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);
  painter.setPen(QPen(NXThemeColor(_themeMode, PopupBorder), 1));
  painter.setBrush(NXThemeColor(_themeMode, PopupBase));
  QRectF bgRect(0, 0, width(), height());
  QPainterPath path;
  path.addRoundedRect(bgRect, _borderRadius, _borderRadius);
  painter.drawPath(path);
}

NXTreeSelectPrivate::NXTreeSelectPrivate(QObject *parent)
    : QObject { parent }
{
}

NXTreeSelectPrivate::~NXTreeSelectPrivate()
{
}

bool NXTreeSelectPrivate::eventFilter(QObject *watched, QEvent *event)
{
  if (_isPopupVisible && event->type() == QEvent::MouseButtonPress) {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPoint globalPos = mouseEvent->globalPosition().toPoint();
#else
    QPoint globalPos = mouseEvent->globalPos();
#endif
    Q_Q(NXTreeSelect);
    QRect popupRect(_popupContainer->mapToGlobal(QPoint(0, 0)), _popupContainer->size());
    QRect triggerRect(q->mapToGlobal(QPoint(0, 0)), q->size());
    if (!popupRect.contains(globalPos) && !triggerRect.contains(globalPos)) {
      _hidePopup();
    }
  }
  return QObject::eventFilter(watched, event);
}

void NXTreeSelectPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXTreeSelect);
  _themeMode                  = themeMode;
  _popupContainer->_themeMode = themeMode;
  q->update();
}

void NXTreeSelectPrivate::onItemClicked(const QModelIndex &index)
{
  Q_Q(NXTreeSelect);
  if (!index.isValid()) {
    return;
  }
  QModelIndex sourceIndex = index;
  if (_proxyModel) {
    sourceIndex = _proxyModel->mapToSource(index);
  }
  if (!sourceIndex.isValid()) {
    return;
  }
  if (_model && _model->hasChildren(sourceIndex)) {
    return;
  }
  _currentIndex = sourceIndex;
  _updateDisplayText();
  _hidePopup();
  Q_EMIT q->currentIndexChanged(_currentIndex);
  Q_EMIT q->currentTextChanged(_currentText);
}

void NXTreeSelectPrivate::onSearchTextChanged(const QString &text)
{
  if (_proxyModel) {
    _proxyModel->setFilterFixedString(text);
    if (!text.isEmpty()) {
      _treeView->expandAll();
    }
    _updatePopupHeight();
  }
}

void NXTreeSelectPrivate::onTreeExpanded(const QModelIndex &index)
{
  Q_UNUSED(index)
  _updatePopupHeight();
}

void NXTreeSelectPrivate::onTreeCollapsed(const QModelIndex &index)
{
  Q_UNUSED(index)
  _updatePopupHeight();
}

void NXTreeSelectPrivate::_showPopup()
{
  Q_Q(NXTreeSelect);
  if (_isPopupVisible || !_model) {
    return;
  }
  _isPopupVisible = true;

  int popupHeight      = _calculatePopupHeight();
  QPoint globalPos     = q->mapToGlobal(QPoint(0, 0));
  QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();

  bool showAbove = (globalPos.y() + q->height() + popupHeight > screenGeometry.bottom()) &&
                   (globalPos.y() - popupHeight >= screenGeometry.top());

  QPoint containerPos;
  if (showAbove) {
    containerPos = QPoint(globalPos.x(), globalPos.y() - popupHeight - 3);
  } else {
    containerPos = QPoint(globalPos.x(), globalPos.y() + q->height() + 3);
  }

  _popupContainer->setFixedWidth(q->width());
  _popupContainer->move(containerPos);
  _popupContainer->setFixedHeight(1);
  _popupContainer->show();
  _popupContainer->raise();

  if (_searchEdit && _pIsSearchVisible) {
    _searchEdit->setVisible(true);
    _searchEdit->clear();
    _searchEdit->setFocus();
  }

  qApp->installEventFilter(this);

  QPropertyAnimation *heightAnimation = new QPropertyAnimation(_popupContainer, "maximumHeight");
  connect(heightAnimation, &QPropertyAnimation::valueChanged, q, [=](const QVariant &value) {
    _popupContainer->setFixedHeight(value.toInt());
  });
  heightAnimation->setStartValue(1);
  heightAnimation->setEndValue(popupHeight);
  heightAnimation->setEasingCurve(QEasingCurve::OutCubic);
  heightAnimation->setDuration(400);
  heightAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  QPropertyAnimation *rotateAnimation = new QPropertyAnimation(this, "");
  connect(rotateAnimation, &QPropertyAnimation::valueChanged, q, [=](const QVariant &value) {
    _expandIconRotate = value.toReal();
    q->update();
  });
  rotateAnimation->setStartValue(_expandIconRotate);
  rotateAnimation->setEndValue(-180.0);
  rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
  rotateAnimation->setDuration(300);
  rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  q->update();
}

void NXTreeSelectPrivate::_hidePopup()
{
  Q_Q(NXTreeSelect);
  if (!_isPopupVisible) {
    return;
  }
  _isPopupVisible = false;

  qApp->removeEventFilter(this);

  int currentHeight                   = _popupContainer->height();
  QPropertyAnimation *heightAnimation = new QPropertyAnimation(_popupContainer, "maximumHeight");
  connect(heightAnimation, &QPropertyAnimation::valueChanged, q, [=](const QVariant &value) {
    _popupContainer->setFixedHeight(value.toInt());
  });
  connect(heightAnimation, &QPropertyAnimation::finished, q, [=]() {
    _popupContainer->hide();
    if (_proxyModel) {
      _proxyModel->setFilterFixedString(QStringLiteral(""));
    }
  });
  heightAnimation->setStartValue(currentHeight);
  heightAnimation->setEndValue(1);
  heightAnimation->setEasingCurve(QEasingCurve::InCubic);
  heightAnimation->setDuration(300);
  heightAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  QPropertyAnimation *rotateAnimation = new QPropertyAnimation(this, "");
  connect(rotateAnimation, &QPropertyAnimation::valueChanged, q, [=](const QVariant &value) {
    _expandIconRotate = value.toReal();
    q->update();
  });
  rotateAnimation->setStartValue(_expandIconRotate);
  rotateAnimation->setEndValue(0.0);
  rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
  rotateAnimation->setDuration(300);
  rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  q->update();
}

void NXTreeSelectPrivate::_updateDisplayText()
{
  if (_currentIndex.isValid() && _model) {
    QStringList pathParts;
    QModelIndex index = _currentIndex;
    while (index.isValid()) {
      pathParts.prepend(index.data(Qt::DisplayRole).toString());
      index = index.parent();
    }
    _currentText = pathParts.join(QStringLiteral(" / "));
  } else {
    _currentText.clear();
  }
}

void NXTreeSelectPrivate::_updatePopupHeight()
{
  Q_Q(NXTreeSelect);
  if (!_isPopupVisible) {
    return;
  }
  int newHeight                       = _calculatePopupHeight();
  QPropertyAnimation *heightAnimation = new QPropertyAnimation(_popupContainer, "maximumHeight");
  connect(heightAnimation, &QPropertyAnimation::valueChanged, q, [=](const QVariant &value) {
    _popupContainer->setFixedHeight(value.toInt());
  });
  heightAnimation->setStartValue(_popupContainer->height());
  heightAnimation->setEndValue(newHeight);
  heightAnimation->setEasingCurve(QEasingCurve::OutCubic);
  heightAnimation->setDuration(250);
  heightAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

int NXTreeSelectPrivate::_calculatePopupHeight() const
{
  int visibleCount = _countVisibleItems();
  int maxItems     = _pMaxVisibleItems > 0 ? _pMaxVisibleItems : 8;
  int itemCount    = qMin(visibleCount, maxItems);
  if (itemCount < 1) {
    itemCount = 1;
  }
  int searchHeight = (_pIsSearchVisible && _searchEdit) ? 40 : 0;
  return itemCount * _pItemHeight + searchHeight + 16;
}

int NXTreeSelectPrivate::_countVisibleItems(const QModelIndex &parent) const
{
  QAbstractItemModel *viewModel =
      _proxyModel ? static_cast<QAbstractItemModel *>(_proxyModel) : static_cast<QAbstractItemModel *>(_model);
  if (!viewModel) {
    return 0;
  }
  int count    = 0;
  int rowCount = viewModel->rowCount(parent);
  for (int i = 0; i < rowCount; ++i) {
    count++;
    QModelIndex childIndex = viewModel->index(i, 0, parent);
    if (_treeView && _treeView->isExpanded(childIndex)) {
      count += _countVisibleItems(childIndex);
    }
  }
  return count;
}
