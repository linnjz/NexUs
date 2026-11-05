#ifndef NXTREESELECT_H
#define NXTREESELECT_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class QStandardItemModel;
class QStandardItem;
class QModelIndex;
class NXTreeSelectPrivate;

class NX_EXPORT NXTreeSelect : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXTreeSelect)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), PlaceholderText)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, ItemHeight)
  Q_PROPERTY_CREATE_H(int, MaxVisibleItems)
  Q_PROPERTY_CREATE_H(bool, IsSearchVisible)
  Q_PROPERTY_CREATE_H(bool, IsEditable)

public:
  explicit NXTreeSelect(QWidget *parent = nullptr);
  ~NXTreeSelect();

  void setModel(QStandardItemModel *model);
  QStandardItemModel *model() const;

  void setCurrentIndex(QModelIndex const &index);
  QModelIndex currentIndex() const;

  QString currentText() const;

  void expandAll();
  void collapseAll();
  Q_SIGNAL void currentIndexChanged(QModelIndex const &index);
  Q_SIGNAL void currentTextChanged(QString const &text);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEnterEvent *event) override;
#else
  void enterEvent(QEvent *event) override;
#endif
  void leaveEvent(QEvent *event) override;
};

#endif // NXTREESELECT_H
