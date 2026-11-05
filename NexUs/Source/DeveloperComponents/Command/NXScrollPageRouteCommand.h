#ifndef NXFRAMEWORK_NXSCROLLPAGEROUTECOMMAND_H
#define NXFRAMEWORK_NXSCROLLPAGEROUTECOMMAND_H

#include "NXActionCommander.h"

class NXScrollPagePrivate;

class NXScrollPageRouteCommand : public NXActionCommand
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QPS_SCREF(QStringList), UndoBreadcrumbList)
  Q_PRIVATE_CREATE(QPS_SCREF(QStringList), RedoBreadcrumbList)
  Q_PRIVATE_CREATE(NXScrollPagePrivate *, ScrollPagePrivate)
  Q_PRIVATE_CREATE(int, UndoPageIndex)
  Q_PRIVATE_CREATE(int, RedoPageIndex)
  Q_PRIVATE_CREATE(bool, IsBreadcrumbClicked)

public:
  explicit NXScrollPageRouteCommand(QObject *parent = nullptr);
  ~NXScrollPageRouteCommand();

  void undo() override;
  void redo() override;
};

#endif //NXFRAMEWORK_NXSCROLLPAGEROUTECOMMAND_H
