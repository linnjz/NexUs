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
  painter.setPen(QPen(NXThemeColor(m_ThemeMode, PopupBorder), 1));
  painter.setBrush(NXThemeColor(m_ThemeMode, PopupBase));
  QRectF bgRect(0, 0, width(), height());
  QPainterPath path;
  path.addRoundedRect(bgRect, m_BorderRadius, m_BorderRadius);
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
  if (m_IsPopupVisible && event->type() == QEvent::MouseButtonPress) {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPoint globalPos = mouseEvent->globalPosition().toPoint();
#else
    QPoint globalPos = mouseEvent->globalPos();
#endif
    Q_Q(NXTreeSelect);
    QRect popupRect(m_PopupContainer->mapToGlobal(QPoint(0, 0)), m_PopupContainer->size());
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
  m_ThemeMode                  = themeMode;
  m_PopupContainer->m_ThemeMode = themeMode;
  q->update();
}

void NXTreeSelectPrivate::onItemClicked(QModelIndex const &index)
{
  Q_Q(NXTreeSelect);
  if (!index.isValid()) {
    return;
  }
  QModelIndex sourceIndex = index;
  if (m_ProxyModel) {
    sourceIndex = m_ProxyModel->mapToSource(index);
  }
  if (!sourceIndex.isValid()) {
    return;
  }
  if (m_Model && m_Model->hasChildren(sourceIndex)) {
    return;
  }
  m_CurrentIndex = sourceIndex;
  _updateDisplayText();
  _hidePopup();
  Q_EMIT q->currentIndexChanged(m_CurrentIndex);
  Q_EMIT q->currentTextChanged(m_CurrentText);
}

void NXTreeSelectPrivate::onSearchTextChanged(QString const &text)
{
  if (m_ProxyModel) {
    m_ProxyModel->setFilterFixedString(text);
    if (!text.isEmpty()) {
      m_TreeView->expandAll();
    }
    _updatePopupHeight();
  }
}

void NXTreeSelectPrivate::onTreeExpanded(QModelIndex const &index)
{
  Q_UNUSED(index)
  _updatePopupHeight();
}

void NXTreeSelectPrivate::onTreeCollapsed(QModelIndex const &index)
{
  Q_UNUSED(index)
  _updatePopupHeight();
}

void NXTreeSelectPrivate::_showPopup()
{
  Q_Q(NXTreeSelect);
  if (m_IsPopupVisible || !m_Model) {
    return;
  }
  m_IsPopupVisible = true;

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

  m_PopupContainer->setFixedWidth(q->width());
  m_PopupContainer->move(containerPos);
  m_PopupContainer->setFixedHeight(1);
  m_PopupContainer->show();
  m_PopupContainer->raise();

  if (m_SearchEdit && m_IsSearchVisible) {
    m_SearchEdit->setVisible(true);
    m_SearchEdit->clear();
    m_SearchEdit->setFocus();
  }

  qApp->installEventFilter(this);

  QPropertyAnimation *heightAnimation = new QPropertyAnimation(m_PopupContainer, "maximumHeight");
  connect(heightAnimation, &QPropertyAnimation::valueChanged, q, [=](QVariant const &value) {
    m_PopupContainer->setFixedHeight(value.toInt());
  });
  heightAnimation->setStartValue(1);
  heightAnimation->setEndValue(popupHeight);
  heightAnimation->setEasingCurve(QEasingCurve::OutCubic);
  heightAnimation->setDuration(400);
  heightAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  QPropertyAnimation *rotateAnimation = new QPropertyAnimation(this, "");
  connect(rotateAnimation, &QPropertyAnimation::valueChanged, q, [=](QVariant const &value) {
    m_ExpandIconRotate = value.toReal();
    q->update();
  });
  rotateAnimation->setStartValue(m_ExpandIconRotate);
  rotateAnimation->setEndValue(-180.0);
  rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
  rotateAnimation->setDuration(300);
  rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  q->update();
}

void NXTreeSelectPrivate::_hidePopup()
{
  Q_Q(NXTreeSelect);
  if (!m_IsPopupVisible) {
    return;
  }
  m_IsPopupVisible = false;

  qApp->removeEventFilter(this);

  int currentHeight                   = m_PopupContainer->height();
  QPropertyAnimation *heightAnimation = new QPropertyAnimation(m_PopupContainer, "maximumHeight");
  connect(heightAnimation, &QPropertyAnimation::valueChanged, q, [=](QVariant const &value) {
    m_PopupContainer->setFixedHeight(value.toInt());
  });
  connect(heightAnimation, &QPropertyAnimation::finished, q, [=]() {
    m_PopupContainer->hide();
    if (m_ProxyModel) {
      m_ProxyModel->setFilterFixedString(QStringLiteral(""));
    }
  });
  heightAnimation->setStartValue(currentHeight);
  heightAnimation->setEndValue(1);
  heightAnimation->setEasingCurve(QEasingCurve::InCubic);
  heightAnimation->setDuration(300);
  heightAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  QPropertyAnimation *rotateAnimation = new QPropertyAnimation(this, "");
  connect(rotateAnimation, &QPropertyAnimation::valueChanged, q, [=](QVariant const &value) {
    m_ExpandIconRotate = value.toReal();
    q->update();
  });
  rotateAnimation->setStartValue(m_ExpandIconRotate);
  rotateAnimation->setEndValue(0.0);
  rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
  rotateAnimation->setDuration(300);
  rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  q->update();
}

void NXTreeSelectPrivate::_updateDisplayText()
{
  if (m_CurrentIndex.isValid() && m_Model) {
    QStringList pathParts;
    QModelIndex index = m_CurrentIndex;
    while (index.isValid()) {
      pathParts.prepend(index.data(Qt::DisplayRole).toString());
      index = index.parent();
    }
    m_CurrentText = pathParts.join(QStringLiteral(" / "));
  } else {
    m_CurrentText.clear();
  }
}

void NXTreeSelectPrivate::_updatePopupHeight()
{
  Q_Q(NXTreeSelect);
  if (!m_IsPopupVisible) {
    return;
  }
  int newHeight                       = _calculatePopupHeight();
  QPropertyAnimation *heightAnimation = new QPropertyAnimation(m_PopupContainer, "maximumHeight");
  connect(heightAnimation, &QPropertyAnimation::valueChanged, q, [=](QVariant const &value) {
    m_PopupContainer->setFixedHeight(value.toInt());
  });
  heightAnimation->setStartValue(m_PopupContainer->height());
  heightAnimation->setEndValue(newHeight);
  heightAnimation->setEasingCurve(QEasingCurve::OutCubic);
  heightAnimation->setDuration(250);
  heightAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

int NXTreeSelectPrivate::_calculatePopupHeight() const
{
  int visibleCount = _countVisibleItems();
  int maxItems     = m_MaxVisibleItems > 0 ? m_MaxVisibleItems : 8;
  int itemCount    = qMin(visibleCount, maxItems);
  if (itemCount < 1) {
    itemCount = 1;
  }
  int searchHeight = (m_IsSearchVisible && m_SearchEdit) ? 40 : 0;
  return itemCount * m_ItemHeight + searchHeight + 16;
}

int NXTreeSelectPrivate::_countVisibleItems(QModelIndex const &parent) const
{
  QAbstractItemModel *viewModel =
      m_ProxyModel ? static_cast<QAbstractItemModel *>(m_ProxyModel) : static_cast<QAbstractItemModel *>(m_Model);
  if (!viewModel) {
    return 0;
  }
  int count    = 0;
  int rowCount = viewModel->rowCount(parent);
  for (int i = 0; i < rowCount; ++i) {
    count++;
    QModelIndex childIndex = viewModel->index(i, 0, parent);
    if (m_TreeView && m_TreeView->isExpanded(childIndex)) {
      count += _countVisibleItems(childIndex);
    }
  }
  return count;
}
