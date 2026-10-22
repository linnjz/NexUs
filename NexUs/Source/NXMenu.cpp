#include "NXMenu.h"

#include <QApplication>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QVBoxLayout>

#include "DeveloperComponents/NXMenuStyle.h"
#include "private/NXMenuPrivate.h"

NXMenu::NXMenu(QWidget *parent)
    : QMenu(parent)
    , d_ptr(new NXMenuPrivate())
{
  Q_D(NXMenu);
  d->q_ptr = this;
#ifdef Q_OS_MACOS
  setAttribute(Qt::WA_Hover);
#endif
  setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setObjectName("NXMenu");
  d->_menuStyle = new NXMenuStyle(style());
  setStyle(d->_menuStyle);
  d->_pAnimationImagePosY = 0;
}

NXMenu::NXMenu(const QString &title, QWidget *parent)
    : NXMenu(parent)
{
  setTitle(title);
}

NXMenu::~NXMenu()
{
  Q_D(NXMenu);
  delete d->_menuStyle;
}

void NXMenu::setMenuItemHeight(int menuItemHeight)
{
  Q_D(NXMenu);
  d->_menuStyle->setMenuItemHeight(menuItemHeight);
}

int NXMenu::getMenuItemHeight() const
{
  Q_D(const NXMenu);
  return d->_menuStyle->getMenuItemHeight();
}

void NXMenu::setBorderRadius(int borderRadius)
{
  Q_D(NXMenu);
  d->_menuStyle->setBorderRadius(borderRadius);
}

int NXMenu::getBorderRadius() const
{
  Q_D(const NXMenu);
  return d->_menuStyle->getBorderRadius();
}

QAction * NXMenu::addMenu(QMenu *menu)
{
  return QMenu::addMenu(menu);
}

NXMenu * NXMenu::addMenu(const QString &title)
{
  NXMenu *menu = new NXMenu(title, this);
  QMenu::addAction(menu->menuAction());
  return menu;
}

NXMenu * NXMenu::addMenu(const QIcon &icon, const QString &title)
{
  NXMenu *menu = new NXMenu(title, this);
  menu->setIcon(icon);
  QMenu::addAction(menu->menuAction());
  return menu;
}

NXMenu * NXMenu::addMenu(NXIconType::IconName icon, const QString &title)
{
  NXMenu *menu = new NXMenu(title, this);
  QMenu::addAction(menu->menuAction());
  menu->menuAction()->setProperty("NXIconType", QChar((unsigned short) icon));
  return menu;
}

QAction * NXMenu::addNXIconAction(NXIconType::IconName icon, const QString &text)
{
  QAction *action = new QAction(text, this);
  action->setProperty("NXIconType", QChar((unsigned short) icon));
  QMenu::addAction(action);
  return action;
}

QAction * NXMenu::addNXIconAction(NXIconType::IconName icon, const QString &text, const QKeySequence &shortcut)
{
  QAction *action = new QAction(text, this);
  action->setShortcut(shortcut);
  action->setProperty("NXIconType", QChar((unsigned short) icon));
  QMenu::addAction(action);
  return action;
}

bool NXMenu::isHasParentMenu() const
{
  return qobject_cast<QMenu *>(parentWidget()) != nullptr;
}

bool NXMenu::isHasChildMenu() const
{
  QList<QAction *> actionList = this->actions();
  for (auto action : actionList) {
    if (action->isSeparator()) {
      continue;
    }
    if (action->menu()) {
      return true;
    }
  }
  return false;
}

bool NXMenu::isHasIcon() const
{
  QList<QAction *> actionList = this->actions();
  for (auto action : actionList) {
    if (action->isSeparator()) {
      continue;
    }
    QMenu *menu = action->menu();
    if (menu && (!menu->icon().isNull() || !menu->property("NXIconType").toString().isEmpty())) {
      return true;
    }
    if (!action->icon().isNull() || !action->property("NXIconType").toString().isEmpty()) {
      return true;
    }
  }
  return false;
}

void NXMenu::showEvent(QShowEvent *event)
{
  Q_EMIT menuShow();
  Q_D(NXMenu);
  if (QMenu *parentMenu = qobject_cast<QMenu *>(parentWidget())) {
    QList<QAction *> actions = parentMenu->actions();
    if (!actions.isEmpty()) {
      QAction *firstAction       = actions.first();
      QPoint parentItemGlobalPos = parentMenu->mapToGlobal(parentMenu->actionGeometry(firstAction).topRight());
      move(parentItemGlobalPos.x() - 1, parentItemGlobalPos.y() - 9);
    }
  } else {
    //消除阴影偏移
    move(this->pos().x() - 6, this->pos().y());
  }
  updateGeometry();
  if (!d->_animationPix.isNull()) {
    d->_animationPix = QPixmap();
  }
  d->_animationPix                 = this->grab(this->rect());
  QPropertyAnimation *posAnimation = new QPropertyAnimation(d, "pAnimationImagePosY");
  connect(posAnimation, &QPropertyAnimation::finished, this, [=]() {
    d->_animationPix = QPixmap();
    update();
  });
  connect(posAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
    update();
  });
  posAnimation->setEasingCurve(QEasingCurve::OutCubic);
  posAnimation->setDuration(400);
  int targetPosY = height();
  if (targetPosY > 160) {
    if (targetPosY < 320) {
      targetPosY = 160;
    } else {
      targetPosY /= 2;
    }
  }

  if (pos().y() + d->_menuStyle->getMenuItemHeight() + 9 >= QCursor::pos().y()) {
    posAnimation->setStartValue(-targetPosY);
  } else {
    posAnimation->setStartValue(targetPosY);
  }

  posAnimation->setEndValue(0);
  posAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  QMenu::showEvent(event);
}

void NXMenu::paintEvent(QPaintEvent *event)
{
  Q_D(NXMenu);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);
  if (!d->_animationPix.isNull()) {
    painter.drawPixmap(QRect(0, d->_pAnimationImagePosY, width(), height()), d->_animationPix);
  } else {
    QMenu::paintEvent(event);
  }
}
