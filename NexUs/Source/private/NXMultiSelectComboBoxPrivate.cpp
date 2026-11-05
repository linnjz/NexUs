#include "NXMultiSelectComboBoxPrivate.h"

#include <QEvent>
#include <QFrame>
#include <QPropertyAnimation>

#include "DeveloperComponents/NXComboBoxView.h"
#include "NXMultiSelectComboBox.h"

NXMultiSelectComboBoxPrivate::NXMultiSelectComboBoxPrivate(QObject *parent)
    : QObject { parent }
{
}

NXMultiSelectComboBoxPrivate::~NXMultiSelectComboBoxPrivate()
{
}

bool NXMultiSelectComboBoxPrivate::eventFilter(QObject *watched, QEvent *event)
{
  if (event->type() == QEvent::Hide) {
    Q_Q(NXMultiSelectComboBox);
    if (watched == q->findChild<QFrame *>()) {
      q->_resetIndicatorAnimations();
    }
  }
  return QObject::eventFilter(watched, event);
}

void NXMultiSelectComboBoxPrivate::onItemPressed(QModelIndex const &index)
{
  Q_Q(NXMultiSelectComboBox);
  if (!m_ComboView->selectionModel()->isSelected(index)) {
    m_ItemSelection[index.row()] = true;
  } else {
    m_ItemSelection[index.row()] = false;
  }
  _refreshCurrentIndexs();
  QPropertyAnimation *markAnimation = new QPropertyAnimation(this, "ExpandMarkWidth");
  connect(markAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    q->update();
  });
  markAnimation->setDuration(300);
  markAnimation->setEasingCurve(QEasingCurve::InOutSine);
  markAnimation->setStartValue(m_ExpandMarkWidth);
  qreal step = (q->width() / 2 - m_BorderRadius) / q->count();
  markAnimation->setEndValue(step * m_SelectedTextList.count());
  markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXMultiSelectComboBoxPrivate::_refreshCurrentIndexs()
{
  Q_Q(NXMultiSelectComboBox);
  QString str;
  _adjustSelectedVector();
  QVector<bool> selectedIndexVector;
  for (int i = 0; i < q->count(); i++) {
    // 该位选中
    QModelIndex index = q->model()->index(i, 0);
    if (m_ItemSelection[i]) {
      m_ComboView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
      if (!str.isEmpty()) {
        str.append(QStringLiteral(", "));
      }
      str.append(q->itemText(i));
      selectedIndexVector.append(true);
    } else {
      m_ComboView->selectionModel()->select(index, QItemSelectionModel::Deselect);
      selectedIndexVector.append(false);
    }
  }
  if (str != m_CurrentText) {
    q->update();
    m_CurrentText = str;
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    m_SelectedTextList = m_CurrentText.split(QStringLiteral(","), Qt::SkipEmptyParts);
#else
    m_SelectedTextList = m_CurrentText.split(QStringLiteral(","));
    if (m_SelectedTextList.count() == 1 && m_SelectedTextList[0].isEmpty()) {
      m_SelectedTextList.clear();
    }
#endif
    q->setCurrentIndex(-1);
    Q_EMIT q->itemSelectionChanged(selectedIndexVector);
    Q_EMIT q->currentTextListChanged(m_SelectedTextList);
  }
}

void NXMultiSelectComboBoxPrivate::_adjustSelectedVector()
{
  Q_Q(NXMultiSelectComboBox);
  while (m_ItemSelection.count() < q->count()) {
    m_ItemSelection.append(false);
  }
}
