#ifndef NXACTIONCOMMANDERVIEW_H
#define NXACTIONCOMMANDERVIEW_H

#include <QTreeView>

#include "NXProperty.h"

class NXActionCommanderViewStyle;

class NXActionCommanderView : public QTreeView
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QPS_SCREF(QString), DomainName)

public:
  explicit NXActionCommanderView(QWidget *parent = nullptr);
  ~NXActionCommanderView() override;

  Q_SLOT void onCustomContextMenuRequested(QPoint const &pos);

Q_SIGNALS:
  Q_SIGNAL void runToCommand(QString const &domainName, int commandIndex);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  NXActionCommanderViewStyle *m_Style { nullptr };
};

#endif //NXACTIONCOMMANDERVIEW_H
