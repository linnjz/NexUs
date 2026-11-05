#ifndef NXTABLEVIEW_H
#define NXTABLEVIEW_H

#include <QTableView>

#include "NXProperty.h"

class NXTableViewPrivate;

class NX_EXPORT NXTableView : public QTableView
{
  Q_OBJECT
  Q_Q_CREATE(NXTableView)
  Q_PROPERTY_CREATE_H(int, HeaderMargin)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, CheckIndicatorWidth)
  Q_PROPERTY_CREATE_H(int, DefaultPadding)
  Q_PROPERTY_CREATE_H(bool, IsHoverRowEffectEnable)

public:
  explicit NXTableView(QWidget *parent = nullptr);
  ~NXTableView();

  void setColumnPadding(int column, int padding);
  int getColumnPadding(int column) const;
  void clearColumnPadding(int column);

  QRect headerCheckIndicatorRect(int section) const;
  QRect checkIndicatorRect(int row, int column) const;

  Q_SIGNAL void tableViewShow();
  Q_SIGNAL void tableViewHide();
  Q_SIGNAL void hoverIndexChanged(QModelIndex const &index);

protected:
  void showEvent(QShowEvent *event) override;
  void hideEvent(QHideEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

#endif // NXTABLEVIEW_H
