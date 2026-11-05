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
  QModelIndexList selectedIndexes = m_SourceListView->selectionModel()->selectedIndexes();
  if (selectedIndexes.isEmpty()) {
    return;
  }
  QList<int> rows;
  for (auto const &index : selectedIndexes) {
    rows.append(index.row());
  }
  std::sort(rows.begin(), rows.end(), std::greater<int>());

  for (int row : rows) {
    QStandardItem *item = m_SourceModel->takeItem(row);
    if (item) {
      QStandardItem *newItem = new QStandardItem(item->text());
      newItem->setForeground(NXThemeColor(m_ThemeMode, BasicText));
      m_TargetModel->appendRow(newItem);
      m_SourceModel->removeRow(row);
      delete item;
    }
  }
  m_SourceListView->clearSelection();
  _refreshCountText();
  Q_EMIT q->transferChanged(q->getSourceItems(), q->getTargetItems());
}

void NXTransferPrivate::onMoveToSource()
{
  Q_Q(NXTransfer);
  QModelIndexList selectedIndexes = m_TargetListView->selectionModel()->selectedIndexes();
  if (selectedIndexes.isEmpty()) {
    return;
  }
  QList<int> rows;
  for (auto const &index : selectedIndexes) {
    rows.append(index.row());
  }
  std::sort(rows.begin(), rows.end(), std::greater<int>());

  for (int row : rows) {
    QStandardItem *item = m_TargetModel->takeItem(row);
    if (item) {
      QStandardItem *newItem = new QStandardItem(item->text());
      newItem->setForeground(NXThemeColor(m_ThemeMode, BasicText));
      m_SourceModel->appendRow(newItem);
      m_TargetModel->removeRow(row);
      delete item;
    }
  }
  m_TargetListView->clearSelection();
  _refreshCountText();
  Q_EMIT q->transferChanged(q->getSourceItems(), q->getTargetItems());
}

void NXTransferPrivate::onSourceSearchTextChanged(QString const &text)
{
  for (int i = 0; i < m_SourceModel->rowCount(); ++i) {
    QStandardItem *item = m_SourceModel->item(i);
    if (item) {
      bool match = text.isEmpty() || item->text().contains(text, Qt::CaseInsensitive);
      m_SourceListView->setRowHidden(i, !match);
    }
  }
}

void NXTransferPrivate::onTargetSearchTextChanged(QString const &text)
{
  for (int i = 0; i < m_TargetModel->rowCount(); ++i) {
    QStandardItem *item = m_TargetModel->item(i);
    if (item) {
      bool match = text.isEmpty() || item->text().contains(text, Qt::CaseInsensitive);
      m_TargetListView->setRowHidden(i, !match);
    }
  }
}

void NXTransferPrivate::onSourceSelectAllChanged(int state)
{
  if (state == Qt::Checked) {
    m_SourceListView->selectAll();
  } else {
    m_SourceListView->clearSelection();
  }
}

void NXTransferPrivate::onTargetSelectAllChanged(int state)
{
  if (state == Qt::Checked) {
    m_TargetListView->selectAll();
  } else {
    m_TargetListView->clearSelection();
  }
}

void NXTransferPrivate::_refreshCountText()
{
  int sourceSelected = m_SourceListView->selectionModel()->selectedIndexes().size();
  int sourceTotal    = m_SourceModel->rowCount();
  m_SourceCountText->setText(QString(QStringLiteral("%1/%2")).arg(sourceSelected).arg(sourceTotal));

  int targetSelected = m_TargetListView->selectionModel()->selectedIndexes().size();
  int targetTotal    = m_TargetModel->rowCount();
  m_TargetCountText->setText(QString(QStringLiteral("%1/%2")).arg(targetSelected).arg(targetTotal));
}
