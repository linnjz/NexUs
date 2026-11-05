#ifndef NXMENUBAR_H
#define NXMENUBAR_H

#include <QMenuBar>

#include "NXDef.h"
#include "NXProperty.h"

class NXMenu;

class NX_EXPORT NXMenuBar : public QMenuBar
{
  Q_OBJECT

public:
  explicit NXMenuBar(QWidget *parent = nullptr);
  ~NXMenuBar();

  QAction *addMenu(QMenu *menu);
  NXMenu *addMenu(QString const &title);
  NXMenu *addMenu(QIcon const &icon, QString const &title);
  NXMenu *addMenu(NXIconType::IconName, QString const &title);

  QAction *addNXIconAction(NXIconType::IconName icon, QString const &text);
  QAction *addNXIconAction(NXIconType::IconName icon, QString const &text, QKeySequence const &shortcut);
};

#endif // NXMENUBAR_H
