#ifndef NXTRANSFER_H
#define NXTRANSFER_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXTransferPrivate;

class NX_EXPORT NXTransfer : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXTransfer)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), SourceTitle)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), TargetTitle)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, ItemHeight)
  Q_PROPERTY_CREATE_H(bool, IsSearchVisible)

public:
  explicit NXTransfer(QWidget *parent = nullptr);
  ~NXTransfer();

  void setSourceItems(QStringList const &items);
  void addSourceItem(QString const &text);
  void addSourceItems(QStringList const &items);

  QStringList getSourceItems() const;
  QStringList getTargetItems() const;

  void moveToTarget();
  void moveToSource();
  void moveAllToTarget();
  void moveAllToSource();
  Q_SIGNAL void transferChanged(QStringList const &sourceItems, QStringList const &targetItems);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXTRANSFER_H
