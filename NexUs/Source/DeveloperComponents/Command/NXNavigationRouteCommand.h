#ifndef NXFRAMEWORK_NXNAVIGATIONROUTECOMMAND_H
#define NXFRAMEWORK_NXNAVIGATIONROUTECOMMAND_H

#include "NXActionCommander.h"
class NXNavigationBar;

class NXNavigationRouteCommand : public NXActionCommand
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QPS_SCREF(QString), UndoPageKey)
  Q_PRIVATE_CREATE(QPS_SCREF(QString), RedoPageKey)
  Q_PRIVATE_CREATE(NXNavigationBar *, NavigationBar)

public:
  explicit NXNavigationRouteCommand(QObject *parent = nullptr);
  ~NXNavigationRouteCommand();

  void undo() override;
  void redo() override;
};

#endif //NXFRAMEWORK_NXNAVIGATIONROUTECOMMAND_H
