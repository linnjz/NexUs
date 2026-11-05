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
  d->m_MenuStyle = new NXMenuStyle(style());
  setStyle(d->m_MenuStyle);
  d->m_AnimationImagePosY = 0;
}

NXMenu::NXMenu(QString const &title, QWidget *parent)
    : NXMenu(parent)
{
  setTitle(title);
}

NXMenu::~NXMenu()
{
  Q_D(NXMenu);
  delete d->m_MenuStyle;
}

void NXMenu::setMenuItemHeight(int menuItemHeight)
{
  Q_D(NXMenu);
  d->m_MenuStyle->setMenuItemHeight(menuItemHeight);
}

int NXMenu::getMenuItemHeight() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXMenu);
  return d->m_MenuStyle->getMenuItemHeight();
}

void NXMenu::setBorderRadius(int borderRadius)
{
  Q_D(NXMenu);
  d->m_MenuStyle->setBorderRadius(borderRadius);
}

int NXMenu::getBorderRadius() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXMenu);
  return d->m_MenuStyle->getBorderRadius();
}

QAction *NXMenu::addMenu(QMenu *menu)
{
  return QMenu::addMenu(menu);
}

NXMenu *NXMenu::addMenu(QString const &title)
{
  NXMenu *menu = new NXMenu(title, this);
  QMenu::addAction(menu->menuAction());
  return menu;
}

NXMenu *NXMenu::addMenu(QIcon const &icon, QString const &title)
{
  NXMenu *menu = new NXMenu(title, this);
  menu->setIcon(icon);
  QMenu::addAction(menu->menuAction());
  return menu;
}

NXMenu *NXMenu::addMenu(NXIconType::IconName icon, QString const &title)
{
  NXMenu *menu = new NXMenu(title, this);
  QMenu::addAction(menu->menuAction());
  menu->menuAction()->setProperty("NXIconType", QChar((unsigned short) icon));
  return menu;
}

QAction *NXMenu::addNXIconAction(NXIconType::IconName icon, QString const &text)
{
  QAction *action = new QAction(text, this);
  action->setProperty("NXIconType", QChar((unsigned short) icon));
  QMenu::addAction(action);
  return action;
}

QAction *NXMenu::addNXIconAction(NXIconType::IconName icon, QString const &text, QKeySequence const &shortcut)
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
  if (!d->m_AnimationPix.isNull()) {
    d->m_AnimationPix = QPixmap();
  }
  d->m_AnimationPix                 = this->grab(this->rect());
  QPropertyAnimation *posAnimation = new QPropertyAnimation(d, "AnimationImagePosY");
  connect(posAnimation, &QPropertyAnimation::finished, this, [=]() {
    d->m_AnimationPix = QPixmap();
    update();
  });
  connect(posAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
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

  if (pos().y() + d->m_MenuStyle->getMenuItemHeight() + 9 >= QCursor::pos().y()) {
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
  if (!d->m_AnimationPix.isNull()) {
    painter.drawPixmap(QRect(0, d->m_AnimationImagePosY, width(), height()), d->m_AnimationPix);
  } else {
    QMenu::paintEvent(event);
  }
}
