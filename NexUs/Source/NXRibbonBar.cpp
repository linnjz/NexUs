#include "NXRibbonBar.h"

#include <QAction>
#include <QCursor>
#include <QEasingCurve>
#include <QEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariantAnimation>

#include "NXApplication.h"
#include "NXMenu.h"
#include "NXRibbonGroup.h"
#include "NXRibbonTabBar.h"
#include "NXTheme.h"
#include "NXToolButton.h"
#include "private/NXRibbonBarPrivate.h"

namespace
{
// 内容区高度随全局字号推导 基线fs=13时为88
int ribbonContentHeight()
{
  return nxApp->getFontPixelSize() * 6 + 10;
}
} // namespace

NXRibbonBar::NXRibbonBar(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXRibbonBarPrivate())
{
  Q_D(NXRibbonBar);
  d->q_ptr          = this;
  d->m_CurrentIndex = 0;

  setObjectName("NXRibbonBar");
  setAttribute(Qt::WA_StyledBackground, false);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  d->m_InternalTabBar = new NXRibbonTabBar(this);
  mainLayout->addWidget(d->m_InternalTabBar);

  d->m_Stack = new QStackedWidget(this);
  d->m_Stack->setContentsMargins(0, 0, 0, 0);
  // 页签内容超出宽度时整体横向滚动
  QScrollArea *pageScrollArea = new QScrollArea(this);
  pageScrollArea->setWidgetResizable(true);
  pageScrollArea->setFrameShape(QFrame::NoFrame);
  pageScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  pageScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  pageScrollArea->setWidget(d->m_Stack);
  mainLayout->addWidget(pageScrollArea, 1);

  connect(d->m_InternalTabBar, &NXRibbonTabBar::signalCurrentIndexChanged, this, [=]() {
    if (d->m_ExternalTabBar) {
      return;
    }
    int idx = d->m_InternalTabBar->getCurrentIndex();
    if (idx != d->m_CurrentIndex) {
      d->m_CurrentIndex = idx;
      d->m_Stack->setCurrentIndex(idx);
      if (d->m_IsCollapsed) {
        setCollapsed(false);
      }
      Q_EMIT signalCurrentIndexChanged();
    }
  });
  connect(d->m_InternalTabBar, &NXRibbonTabBar::tabClicked, this, [=](int idx) {
    if (!d->m_ExternalTabBar) {
      Q_EMIT tabClicked(idx);
    }
  });
  connect(d->m_InternalTabBar, &NXRibbonTabBar::tabReclicked, this, [=](int) {
    if (!d->m_ExternalTabBar) {
      setCollapsed(!d->m_IsCollapsed);
    }
  });

  setFixedHeight(d->m_InternalTabBar->height() + ribbonContentHeight());

  d->m_AutoHideTimer = new QTimer(this);
  d->m_AutoHideTimer->setSingleShot(true);
  d->m_AutoHideTimer->setInterval(150);
  connect(d->m_AutoHideTimer, &QTimer::timeout, this, [=]() {
    if (d->m_IsPinned || d->m_IsCollapsed) {
      return;
    }
    QPoint const cursor = QCursor::pos();
    bool over           = this->rect().contains(this->mapFromGlobal(cursor));
    if (!over && d->m_InternalTabBar && d->m_InternalTabBar->isVisible()) {
      over = d->m_InternalTabBar->rect().contains(d->m_InternalTabBar->mapFromGlobal(cursor));
    }
    if (!over && d->m_ExternalTabBar) {
      over = d->m_ExternalTabBar->rect().contains(d->m_ExternalTabBar->mapFromGlobal(cursor));
    }
    if (!over) {
      setCollapsed(true);
    }
  });

  this->installEventFilter(this);
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &QWidget::customContextMenuRequested, this, [=](QPoint const &pos) {
    showPinContextMenu(this->mapToGlobal(pos));
  });
  d->m_InternalTabBar->installEventFilter(this);
  d->m_InternalTabBar->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(d->m_InternalTabBar, &QWidget::customContextMenuRequested, this, [=](QPoint const &pos) {
    if (!d->m_ExternalTabBar) {
      showPinContextMenu(d->m_InternalTabBar->mapToGlobal(pos));
    }
  });

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXRibbonBar::~NXRibbonBar()
{
}

void NXRibbonBar::bindTabBar(NXRibbonTabBar *tabBar)
{
  Q_D(NXRibbonBar);
  if (d->m_ExternalTabBar.data() == tabBar) {
    return;
  }

  if (d->m_ExternalTabBar) {
    disconnect(d->m_ExternalTabBar.data(), nullptr, this, nullptr);
  }

  d->m_ExternalTabBar = tabBar;

  if (tabBar) {
    d->m_InternalTabBar->hide();
    setFixedHeight(ribbonContentHeight());

    tabBar->clear();
    for (int i = 0; i < d->m_InternalTabBar->tabCount(); ++i) {
      tabBar->appendTab(d->m_InternalTabBar->tabText(i));
    }
    if (d->m_CurrentIndex >= 0 && d->m_CurrentIndex < tabBar->tabCount()) {
      tabBar->setCurrentIndex(d->m_CurrentIndex);
    }

    connect(tabBar, &NXRibbonTabBar::signalCurrentIndexChanged, this, [=]() {
      if (!d->m_ExternalTabBar) {
        return;
      }
      int idx = d->m_ExternalTabBar->getCurrentIndex();
      if (idx != d->m_CurrentIndex) {
        d->m_CurrentIndex = idx;
        d->m_Stack->setCurrentIndex(idx);
        if (d->m_IsCollapsed) {
          setCollapsed(false);
        }
        Q_EMIT signalCurrentIndexChanged();
      }
    });
    connect(tabBar, &NXRibbonTabBar::tabClicked, this, [=](int idx) {
      Q_EMIT tabClicked(idx);
    });
    connect(tabBar, &NXRibbonTabBar::tabReclicked, this, [=](int) {
      setCollapsed(!d->m_IsCollapsed);
    });

    tabBar->installEventFilter(this);
    tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tabBar, &QWidget::customContextMenuRequested, this, [=](QPoint const &pos) {
      if (d->m_ExternalTabBar) {
        showPinContextMenu(d->m_ExternalTabBar->mapToGlobal(pos));
      }
    });
  } else {
    d->m_InternalTabBar->show();
    setFixedHeight(d->m_InternalTabBar->height() + ribbonContentHeight());
  }
}

NXRibbonTabBar *NXRibbonBar::tabBar() const
{
  Q_CONST_D(NXRibbonBar);
  return d->activeTabBar();
}

QWidget *NXRibbonBar::addTab(QString const &title)
{
  Q_D(NXRibbonBar);
  QWidget *page = new QWidget(this);
  page->setAttribute(Qt::WA_TranslucentBackground);
  QHBoxLayout *pageLayout = new QHBoxLayout(page);
  pageLayout->setContentsMargins(6, 4, 6, 6);
  pageLayout->setSpacing(0);
  pageLayout->addStretch();
  d->m_Stack->addWidget(page);

  NXRibbonTabBar *active = d->activeTabBar();
  if (active) {
    active->appendTab(title);
    if (d->m_Stack->count() == 1) {
      d->m_CurrentIndex = 0;
      d->m_Stack->setCurrentIndex(0);
      active->setCurrentIndex(0);
    }
  }
  return page;
}

NXRibbonGroup *NXRibbonBar::addGroup(QWidget *page, QString const &title)
{
  if (!page) {
    return nullptr;
  }
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(page->layout());
  if (!layout) {
    return nullptr;
  }
  NXRibbonGroup *group = new NXRibbonGroup(title, page);
  layout->insertWidget(layout->count() - 1, group);
  // 框架级命令总线 统一解析页标题上下文
  connect(group, &NXRibbonGroup::ribbonButtonTriggered, this, [this, group](NXToolButton *button) {
    QString pageTitle;
    if (NXRibbonTabBar *active = d_func()->activeTabBar()) {
      pageTitle = active->tabText(d_func()->m_CurrentIndex);
    }
    Q_EMIT ribbonActionTriggered(pageTitle, group, button ? button->defaultAction() : nullptr);
  });
  return group;
}

void NXRibbonBar::removePage(int index)
{
  Q_D(NXRibbonBar);
  if (index < 0 || index >= d->m_Stack->count()) {
    return;
  }
  QWidget *page = d->m_Stack->widget(index);
  d->m_Stack->removeWidget(page);
  page->deleteLater();
  if (NXRibbonTabBar *active = d->activeTabBar()) {
    active->removeTab(index);
  }
}

void NXRibbonBar::removePage(QWidget *page)
{
  Q_D(NXRibbonBar);
  if (!page) {
    return;
  }
  removePage(d->m_Stack->indexOf(page));
}

void NXRibbonBar::removeGroup(QWidget *page, NXRibbonGroup *group)
{
  if (!page || !group || group->parentWidget() != page) {
    return;
  }
  if (QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(page->layout())) {
    layout->removeWidget(group);
  }
  group->deleteLater();
}

QList<NXRibbonGroup *> NXRibbonBar::getGroups(QWidget *page) const
{
  QList<NXRibbonGroup *> groups;
  QHBoxLayout const *layout = page ? qobject_cast<QHBoxLayout const *>(page->layout()) : nullptr;
  if (!layout) {
    return groups;
  }
  for (int i = 0; i < layout->count(); ++i) {
    if (auto *group = qobject_cast<NXRibbonGroup *>(layout->itemAt(i)->widget())) {
      groups.append(group);
    }
  }
  return groups;
}

void NXRibbonBar::setRibbonPageEnable(int index, bool isEnable)
{
  Q_D(NXRibbonBar);
  if (index < 0 || index >= d->m_Stack->count()) {
    return;
  }
  d->m_Stack->widget(index)->setEnabled(isEnable);
  if (NXRibbonTabBar *active = d->activeTabBar()) {
    active->setTabEnabled(index, isEnable);
  }
}

void NXRibbonBar::setRibbonPageEnable(QWidget *page, bool isEnable)
{
  Q_D(NXRibbonBar);
  if (!page) {
    return;
  }
  setRibbonPageEnable(d->m_Stack->indexOf(page), isEnable);
}

bool NXRibbonBar::isRibbonPageEnable(int index) const
{
  Q_CONST_D(NXRibbonBar);
  if (index < 0 || index >= d->m_Stack->count()) {
    return false;
  }
  return d->m_Stack->widget(index)->isEnabled();
}

int NXRibbonBar::tabCount() const
{
  Q_CONST_D(NXRibbonBar);
  return d->m_Stack->count();
}

QString NXRibbonBar::tabText(int index) const
{
  Q_CONST_D(NXRibbonBar);
  NXRibbonTabBar *active = d->activeTabBar();
  return active ? active->tabText(index) : QString();
}

void NXRibbonBar::setCurrentIndex(int currentIndex)
{
  Q_D(NXRibbonBar);
  if (currentIndex < 0 || currentIndex >= d->m_Stack->count()) {
    return;
  }
  if (d->m_CurrentIndex == currentIndex) {
    return;
  }
  d->m_CurrentIndex = currentIndex;
  d->m_Stack->setCurrentIndex(currentIndex);
  if (NXRibbonTabBar *active = d->activeTabBar()) {
    active->setCurrentIndex(currentIndex);
  }
  Q_EMIT signalCurrentIndexChanged();
}

int NXRibbonBar::getCurrentIndex() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXRibbonBar);
  return d->m_CurrentIndex;
}

void NXRibbonBar::setCollapsed(bool collapsed)
{
  Q_D(NXRibbonBar);
  if (d->m_IsCollapsed == collapsed) {
    return;
  }
  d->m_IsCollapsed = collapsed;

  int const normalH    = d->m_ExternalTabBar ? ribbonContentHeight() : ((d->m_InternalTabBar ? d->m_InternalTabBar->height() : 0) + ribbonContentHeight());
  int const collapsedH = d->m_ExternalTabBar ? 0 : (d->m_InternalTabBar ? d->m_InternalTabBar->height() : 0);
  int const targetH    = collapsed ? collapsedH : normalH;

  if (!d->m_HeightAnim) {
    d->m_HeightAnim = new QVariantAnimation(this);
    d->m_HeightAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(d->m_HeightAnim, &QVariantAnimation::valueChanged, this, [this](QVariant const &v) {
      int const h = v.toInt();
      this->setMinimumHeight(h);
      this->setMaximumHeight(h);
    });
    connect(d->m_HeightAnim, &QVariantAnimation::finished, this, [d]() {
      d->m_Stack->setVisible(!d->m_IsCollapsed);
    });
  }
  d->m_HeightAnim->stop();

  if (!collapsed) {
    d->m_Stack->setVisible(true);
  }
  d->m_HeightAnim->setStartValue(this->height());
  d->m_HeightAnim->setEndValue(targetH);
  d->m_HeightAnim->setDuration(d->m_AnimationDuration);
  d->m_HeightAnim->start();

  Q_EMIT collapsedChanged(collapsed);
}

bool NXRibbonBar::isCollapsed() const
{
  Q_CONST_D(NXRibbonBar);
  return d->m_IsCollapsed;
}

void NXRibbonBar::setPinned(bool pinned)
{
  Q_D(NXRibbonBar);
  if (d->m_IsPinned == pinned) {
    return;
  }
  d->m_IsPinned = pinned;
  if (pinned) {
    d->m_AutoHideTimer->stop();
  }
  Q_EMIT pinnedChanged(pinned);
}

bool NXRibbonBar::isPinned() const
{
  Q_CONST_D(NXRibbonBar);
  return d->m_IsPinned;
}

void NXRibbonBar::setAnimationDuration(int durationMs)
{
  Q_D(NXRibbonBar);
  d->m_AnimationDuration = qMax(0, durationMs);
}

int NXRibbonBar::getAnimationDuration() const
{
  Q_CONST_D(NXRibbonBar);
  return d->m_AnimationDuration;
}

bool NXRibbonBar::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXRibbonBar);
  if (watched == this || watched == d->m_InternalTabBar || watched == d->m_ExternalTabBar.data()) {
    if (event->type() == QEvent::Enter) {
      d->m_AutoHideTimer->stop();
    } else if (event->type() == QEvent::Leave) {
      if (!d->m_IsPinned && !d->m_IsCollapsed) {
        d->m_AutoHideTimer->start();
      }
    }
  }
  return QWidget::eventFilter(watched, event);
}

void NXRibbonBar::showPinContextMenu(QPoint globalPos)
{
  Q_D(NXRibbonBar);
  NXMenu menu(this);
  menu.setMenuItemHeight(27);
  QAction *pinAction = menu.addAction(d->m_IsPinned ? QStringLiteral("取消固定") : QStringLiteral("固定 Ribbon"));
  pinAction->setCheckable(true);
  pinAction->setChecked(d->m_IsPinned);
  QAction *collapseAction =
      menu.addAction(d->m_IsCollapsed ? QStringLiteral("展开 Ribbon") : QStringLiteral("折叠 Ribbon"));
  QAction *selected = menu.exec(globalPos);
  if (!selected) {
    return;
  }
  if (selected == pinAction) {
    setPinned(!d->m_IsPinned);
  } else if (selected == collapseAction) {
    setCollapsed(!d->m_IsCollapsed);
  }
}

void NXRibbonBar::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXRibbonBar);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->m_ThemeMode, WindowBase));
  painter.drawRect(rect());
  if (d->m_ExternalTabBar) {
    painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
    painter.drawLine(0, 0, width(), 0);
  }
}

QSize NXRibbonBar::sizeHint() const
{
  Q_CONST_D(NXRibbonBar);
  int const tabH = d->m_InternalTabBar ? d->m_InternalTabBar->height() : 0;
  int h;
  if (d->m_IsCollapsed) {
    h = d->m_ExternalTabBar ? 0 : tabH;
  } else {
    h = d->m_ExternalTabBar ? ribbonContentHeight() : (tabH + ribbonContentHeight());
  }
  return QSize(800, h);
}

NXRibbonBarPrivate::NXRibbonBarPrivate(QObject *parent)
    : QObject { parent }
{
}

NXRibbonBarPrivate::~NXRibbonBarPrivate()
{
}

NXRibbonTabBar *NXRibbonBarPrivate::activeTabBar() const
{
  if (m_ExternalTabBar) {
    return m_ExternalTabBar.data();
  }
  return m_InternalTabBar;
}
