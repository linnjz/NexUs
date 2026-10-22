#include "NXMenuBar.h"

#include <QEvent>
#include <QPainter>
#include <QToolButton>

#include "DeveloperComponents/NXMenuBarStyle.h"
#include "NXIcon.h"
#include "NXMenu.h"

NXMenuBar::NXMenuBar(QWidget *parent)
    : QMenuBar(parent)
{
#ifdef Q_OS_MAC
  setNativeMenuBar(false);
#endif
  setMouseTracking(true);
  setObjectName("NXMenuBar");
  setStyle(new NXMenuBarStyle(style()));
  QToolButton *tool = this->findChild<QToolButton *>();
  if (tool && tool->objectName() == QStringLiteral("qt_menubar_ext_button")) {
    tool->setStyleSheet(QStringLiteral("QToolButton{background-color:transparent; border:none;}"));
    QMenu *oldMenu = tool->menu();
    NXMenu *menu   = new NXMenu(this);
    menu->setObjectName("NXExtendMenu");
    menu->setMenuItemHeight(27);
    if (oldMenu) {
      oldMenu->deleteLater();
    }
    tool->setMenu(menu);
  }
}

NXMenuBar::~NXMenuBar()
{
  delete this->style();
}

QAction * NXMenuBar::addMenu(QMenu *qmenu)
{
  NXMenu *menu = dynamic_cast<NXMenu *>(qmenu);
  if (menu) {
    menu->setMenuItemHeight(27);
  }
  return QMenuBar::addMenu(qmenu);
}

NXMenu * NXMenuBar::addMenu(const QString &title)
{
  NXMenu *menu = new NXMenu(title, this);
  menu->setMenuItemHeight(27);
  QMenuBar::addAction(menu->menuAction());
  return menu;
}

NXMenu * NXMenuBar::addMenu(const QIcon &icon, const QString &title)
{
  NXMenu *menu = new NXMenu(title, this);
  menu->setMenuItemHeight(27);
  menu->setIcon(icon);
  QMenuBar::addAction(menu->menuAction());
  return menu;
}

NXMenu * NXMenuBar::addMenu(NXIconType::IconName icon, const QString &title)
{
  NXMenu *menu = new NXMenu(title, this);
  menu->setMenuItemHeight(27);
  menu->menuAction()->setProperty("NXIconType", QChar((unsigned short) icon));
  menu->menuAction()->setIcon(NXIcon::getInstance()->getNXIcon(NXIconType::Broom, 1));
  QMenuBar::addAction(menu->menuAction());
  return menu;
}

QAction * NXMenuBar::addNXIconAction(NXIconType::IconName icon, const QString &text)
{
  QAction *action = new QAction(text, this);
  action->setProperty("NXIconType", QChar((unsigned short) icon));
  action->setIcon(NXIcon::getInstance()->getNXIcon(NXIconType::Broom, 1));
  QMenuBar::addAction(action);
  return action;
}

QAction * NXMenuBar::addNXIconAction(NXIconType::IconName icon, const QString &text, const QKeySequence &shortcut)
{
  QAction *action = new QAction(text, this);
  action->setShortcut(shortcut);
  action->setProperty("NXIconType", QChar((unsigned short) icon));
  action->setIcon(NXIcon::getInstance()->getNXIcon(NXIconType::Broom, 1));
  QMenuBar::addAction(action);
  return action;
}
