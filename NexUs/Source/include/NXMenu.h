#ifndef NXMENU_H
#define NXMENU_H

#include <QMenu>
#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"
class NXMenuPrivate;

class NX_EXPORT NXMenu : public QMenu
{
  Q_OBJECT
  Q_Q_CREATE(NXMenu)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, MenuItemHeight)

public:
  explicit NXMenu(QWidget *parent = nullptr);
  explicit NXMenu(QString const &title, QWidget *parent = nullptr);
  ~NXMenu();

  QAction *addMenu(QMenu *menu);
  NXMenu *addMenu(QString const &title);
  NXMenu *addMenu(QIcon const &icon, QString const &title);
  NXMenu *addMenu(NXIconType::IconName icon, QString const &title);

  QAction *addNXIconAction(NXIconType::IconName icon, QString const &text);
  QAction *addNXIconAction(NXIconType::IconName icon, QString const &text, QKeySequence const &shortcut);

  bool isHasParentMenu() const;
  bool isHasChildMenu() const;
  bool isHasIcon() const;
  Q_SIGNAL void menuShow();

protected:
  void showEvent(QShowEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXMENU_H
