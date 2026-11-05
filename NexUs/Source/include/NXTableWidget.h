#ifndef NXTABLEWIDGET_H
#define NXTABLEWIDGET_H

#include <QTableWidget>

#include "NXProperty.h"

class NXTableWidgetPrivate;

class NX_EXPORT NXTableWidget : public QTableWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXTableWidget)
  Q_PROPERTY_CREATE_H(int, ItemHeight)
  Q_PROPERTY_CREATE_H(int, HeaderMargin)
  Q_PROPERTY_CREATE_H(bool, IsTransparent)

public:
  explicit NXTableWidget(QWidget *parent = nullptr);
  ~NXTableWidget();

  void insertRows(int row, int count);
  void removeRows(int row, int count);
  void insertColumns(int column, int count);
  void removeColumns(int column, int count);

  void setItemText(int row, int column, QString const &text);
  QString getItemText(int row, int column) const;
  void setRowData(int row, QStringList const &data);
  QStringList getRowData(int row) const;
  Q_SIGNAL void tableWidgetShow();
  Q_SIGNAL void tableWidgetHide();

protected:
  void showEvent(QShowEvent *event) override;
  void hideEvent(QHideEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

#endif // NXTABLEWIDGET_H
