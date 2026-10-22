#include "NXTransferPrivate.h"

#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include "DeveloperComponents/NXBaseListView.h"
#include "NXCheckBox.h"
#include "NXText.h"
#include "NXTheme.h"
#include "NXTransfer.h"

NXTransferPrivate::NXTransferPrivate(QObject *parent)
    : QObject { parent }
{
}

NXTransferPrivate::~NXTransferPrivate()
{
}

void NXTransferPrivate::onMoveToTarget()
{
  Q_Q(NXTransfer);
  QModelIndexList selectedIndexes = _sourceListView->selectionModel()->selectedIndexes();
  if (selectedIndexes.isEmpty()) {
    return;
  }
  QList<int> rows;
  for (const auto &index : selectedIndexes) {
    rows.append(index.row());
  }
  std::sort(rows.begin(), rows.end(), std::greater<int>());

  for (int row : rows) {
    QStandardItem *item = _sourceModel->takeItem(row);
    if (item) {
      QStandardItem *newItem = new QStandardItem(item->text());
      newItem->setForeground(NXThemeColor(_themeMode, BasicText));
      _targetModel->appendRow(newItem);
      _sourceModel->removeRow(row);
      delete item;
    }
  }
  _sourceListView->clearSelection();
  _refreshCountText();
  Q_EMIT q->transferChanged(q->getSourceItems(), q->getTargetItems());
}

void NXTransferPrivate::onMoveToSource()
{
  Q_Q(NXTransfer);
  QModelIndexList selectedIndexes = _targetListView->selectionModel()->selectedIndexes();
  if (selectedIndexes.isEmpty()) {
    return;
  }
  QList<int> rows;
  for (const auto &index : selectedIndexes) {
    rows.append(index.row());
  }
  std::sort(rows.begin(), rows.end(), std::greater<int>());

  for (int row : rows) {
    QStandardItem *item = _targetModel->takeItem(row);
    if (item) {
      QStandardItem *newItem = new QStandardItem(item->text());
      newItem->setForeground(NXThemeColor(_themeMode, BasicText));
      _sourceModel->appendRow(newItem);
      _targetModel->removeRow(row);
      delete item;
    }
  }
  _targetListView->clearSelection();
  _refreshCountText();
  Q_EMIT q->transferChanged(q->getSourceItems(), q->getTargetItems());
}

void NXTransferPrivate::onSourceSearchTextChanged(const QString &text)
{
  for (int i = 0; i < _sourceModel->rowCount(); ++i) {
    QStandardItem *item = _sourceModel->item(i);
    if (item) {
      bool match = text.isEmpty() || item->text().contains(text, Qt::CaseInsensitive);
      _sourceListView->setRowHidden(i, !match);
    }
  }
}

void NXTransferPrivate::onTargetSearchTextChanged(const QString &text)
{
  for (int i = 0; i < _targetModel->rowCount(); ++i) {
    QStandardItem *item = _targetModel->item(i);
    if (item) {
      bool match = text.isEmpty() || item->text().contains(text, Qt::CaseInsensitive);
      _targetListView->setRowHidden(i, !match);
    }
  }
}

void NXTransferPrivate::onSourceSelectAllChanged(int state)
{
  if (state == Qt::Checked) {
    _sourceListView->selectAll();
  } else {
    _sourceListView->clearSelection();
  }
}

void NXTransferPrivate::onTargetSelectAllChanged(int state)
{
  if (state == Qt::Checked) {
    _targetListView->selectAll();
  } else {
    _targetListView->clearSelection();
  }
}

void NXTransferPrivate::_refreshCountText()
{
  int sourceSelected = _sourceListView->selectionModel()->selectedIndexes().size();
  int sourceTotal    = _sourceModel->rowCount();
  _sourceCountText->setText(QString(QStringLiteral("%1/%2")).arg(sourceSelected).arg(sourceTotal));

  int targetSelected = _targetListView->selectionModel()->selectedIndexes().size();
  int targetTotal    = _targetModel->rowCount();
  _targetCountText->setText(QString(QStringLiteral("%1/%2")).arg(targetSelected).arg(targetTotal));
}
